#include "PCISPH.h"

namespace cs224 {

// @Func : Constructor of PCISPH class.
//         1. Initialize simulation parameters (defined structs) based on settings.
//		   2. Generate both fluid and boundary particles.
//         3. Build Kernel.
//         4. Initialize and build both fluid and boundary grids.
//         5. Massify boundary particles.
//         6. Do basic setup for the simulation.
// @Params scene : parsed scene object.
// @Tested : false
PCISPH::PCISPH(const Scene &scene) {

   loadParams(scene.settings);
   
   // Build the scene based on loaded parameters.
   buildScene(scene);
   
   // Allowcate memories to vectors and matricies.
   allocMemory(currentFluidPosition.size(), boundaryPositions.size());

   // Build Kernel.
   W.buildKernel(kernelParams.radius);

   // Initialize grids.
   buildBoundaryGrids();          
   buildFluidGrids();    

   // Massify boundary particles.
   massifyBoundary();

   // Do basic setup for the simulation.
   basicSimSetup();
}

PCISPH::~PCISPH() {

}

// @Func : Load paramters from settings.
// TODO: Implement Settings.h, Settings.h should be a member of Scene.h
void PCISPH::loadParams(const Settings &settings) {

  float _restDensity = settings.getFloat("restDensity", 1000.f);
  timeStep = settings.getFloat("timeStep", 0.001f);

  // Compute derived constants
  particleParams.init(settings.getFloat("particleRadius", 0.01f), _restDensity);
  kernelParams.init(KERNEL_SCALE, particleParams.radius, particleParams.diameter);
  simConstParams.init(_restDensity,
                      settings.getFloat("surfaceTension", 1.f),
                      settings.getFloat("viscosity", 0.f),
                      settings.getFloat("compressionThreshold", 0.02f),
                      settings.getVector3("gravity", Vector3f(0.f, -9.8f, 0.f))
                      );
  
  averageDensityVarianceTh = simConstParams.maxCompression * simConstParams.restDensity;
  maximumDensityVarianceTh = averageDensityVarianceTh * 10.f;
   
} 

// @Func : Build the simulation scene based on the parsed scene file.
// @Params scene : parsed scene object.
// @Tested : false
void PCISPH::buildScene(const Scene &scene) {

}

// TODO: Implement Particle.h
void PCISPH::generateFluidParticles(const Particle::Volume &volume) {

}

void PCISPH::generateBoundaryParticles(const Particle::Boundary &boundary) {

}

// Compute the approximate mass of boundary particles based on [4] equation 4 and 5
void SPH::massifyBoundary() {
     ConcurrentUtils::ccLoop(boundaryPositions.size(), 
     [this] (size_t i) {
        float weight = 0.f;
        boundaryGrid.query(kernelParams.radius, boundaryPositions, boundaryPositions[i], [this, &weight] (size_t j, const Vector3f &r, float r2) {
            weight += W.poly6(r2);
        });
        boundaryMass[i] = simConstParams.restDensity / (W.poly6C * weight) / 1.17f;
    });
}

// @Func : Setup the simulation 
//       1. Initialize particle densities.
//       2. Initialize position and velocity buffers for shocks.
//       3. Relax the particle distributions.
//       4. Reset clocks.
// @Tested : true
void PCISPH::basicSimSetup() {
    
    initDensities();
    initShocks();
    relax();
}

// @Func : Initialize shock buffers.
// @Tested : true
void PCISPH::initShocks() {
	fluidPositionBeforeShock = currentFluidPosition;
	fluidVelocityBeforeShock = currentFluidVelocity;
}


// @Func : 1. Relax the initial particle distributions.
//         2. Reset clocks.
// @Tested : true
void PCISPH::relax() {
   
   simulate(RELAX_ITERATION);
   for (Vector3f &v : currentFluidVelocity) {
       v = Vector3f(0.f);
   }
   currentTime = 0.f;
   timeBeforeShock = 0.f;
}


// TODO: Build fluid grid
void PCISPH::buildFluidGrids() {

}

// @Func : Test the whether a boundary particle is alive.
//         A boundary particle is considered alive iff
//         it has at least one neighbour.
//  @Tested : true
void PCISPH::testBoundary() {
    ConcurrentUtils::ccLoop(boundaryPositions.size(), [this] (size_t i) {
        boundaryAlive[i] = fluidGrid.isIsolate(kernelParams.radius, currentFluidPosition, boundaryPositions[i]);
    });
}

// @Func : This function is used for initializing the fluid & boundary particle densities.
//         This function will be called once per frame, before goes into correction loop.
//         The density calculation is optimized for handling the fluid particles with deficient
//         surronding fluid particle neighbours by taking the volume of surronding boundary particles
//         into account.
//         The volume of a boundary particle is defined as the weighted kernel sum of surrounding boundary particles.
void PCISPH::initDensities() {
    
    // Calcuate the boundary particle densities
    ConcurrentUtils::ccLoop(boundaryPositions.size(), 
    [this] (int i) {
        float fluidTerm = 0.f;
        float boundaryTerm = 0.f; 
        
        // Query the surrounding fluid particles.
        fluidGrid.query(kernelParams.radius, currentFluidPosition, boundaryPositions[i],
        [this, &fluidTerm] (int j, Vector3f &r, float squaredR){
             fluidTerm += W.poly6(squaredR) * particleParams.mass;
        });
        
        // Query the surrounding boundary particles.
        boundaryGrid.query(kernelParams.radius, boundaryPositions, boundaryPositions[i],
        [this, &boundaryTerm] (int j, Vector3f &r, float squaredR){
            boundaryTerm += W.poly6(squaredR) * boundaryMass[j];
        });

        boundaryDensities[i] = W.poly6C * (fluidTerm + boundaryTerm);
    });

    // Calculate the fluid particle densities
    ConcurrentUtils::ccLoop(currentFluidPosition.size(),
    [this] (int i) {
         float fluidTerm = 0.f;
         float boundaryTerm = 0.f;

         // Query the surrounding fluid particles.
        fluidGrid.query(kernelParams.radius, currentFluidPosition, currentFluidPosition[i],
        [this, &fluidTerm] (int j, Vector3f &r, float squaredR){
             fluidTerm += W.poly6(squaredR) * particleParams.mass;
        });

        // Query the surrounding boundary particles.
        boundaryGrid.query(kernelParams.radius, boundaryPositions, currentFluidPosition[i],
        [this, &boundaryTerm] (int j, Vector3f &r, float squaredR){
            boundaryTerm += W.poly6(squaredR) * boundaryMass[j];
        });

        fluidDensities[i] = W.poly6C * (fluidTerm + boundaryTerm);
    });
    
}

// @Func : Compute the normal of a fluid particle, the magnitude of the normal
//         is proportional to its surface curvature, its value is close to 0
//         for inner fluid particles and big at the surface area where the curvature
//         is significant. The normal information will be used in computing force
//         which can counteract the surface curvature.
void PCISPH::initNormals() {

   ConcurrentUtils::ccLoop(currentFluidPosition.size(), 
   [this] (int i) {
        Vector3f normal;
        fluidGrid.query(kernelParams.radius, currentFluidPosition, currentFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
            normal += W.poly6Grad(r, r2) / fluidDensities[j];
        });
        normal *= kernelParams.radius * particleParams.mass * W.poly6Grad1;
        fluidNormals[i] = normal;
    });
}

// In this implementation the initial force is mainly based on three components
// 1. Viscosity
// 2. Surface tension : Cohesive term + Curvature term
// 3. Gravity : F = mg 
// After calculating the intial force, set pressure and pressure force to 0
// according to the PCISPH algorithm.
void PCISPH::initForces() {

	 ConcurrentUtils::ccLoop(currentFluidPosition.size(), [&] (size_t i) {
	    
	    // Terms for computing F(v,g,ext) in the paper algorithm.
	    Vector3f viscocity;
	    Vector3f cohesion; 
	    Vector3f curvature; 
	    
	    // First of all, we have to iterate through the space to fetch all 
	    // adjacent particles that within the range of the kernel, which 
	    // local at the center of the current particle.
	    fluidGrid.query(kernelParams.radius, currentFluidPosition, currentFluidPosition[i], 
      [&] (int j, const Vector3f &r, float r2) {
	     
	        if (r2 < EPSILON) {
	            return;
	        }
	 
	        float absxij = std::sqrt(r2);

	        viscocity -= (currentFluidVelocity[i] - currentFluidVelocity[j]) * (W.viscosityLaplace(absxij) / fluidDensities[j]);
	        
	        // K(i,j) is the surface tension constant 
	        // Basically F(sf) = K(i,j) * (F(cohesion)+F(curvature))
	        float Kij  = 2.f * simConstParams.restDensity / (fluidDensities[i] + fluidDensities[j]);
	        cohesion  += Kij * (r / absxij) * W.surfaceTension(absxij);
	        curvature += Kij * (fluidNormals[i] - fluidNormals[j]);
	    });

	    viscocity *=  simConstParams.viscosity * particleParams.squaredMass * W.viscosityGrad2 / fluidDensities[i];
	    cohesion  *= -simConstParams.surfaceTension * particleParams.squaredMass * W.surfaceTensionConstant;
	    curvature *= -simConstParams.surfaceTension * particleParams.mass;
	    
	    // The overall force F(p,i)
	    Vector3f force;
	    force += cohesion + curvature + viscocity;
	    force += particleParams.mass * simConstParams.gravity;

	    fluidForces[i] = force;
	    fluidPressures[i] = 0.f;
	    fluidPressureForces[i] = Vector3f(0.f);
	});

}

// @Func : This function predicts the velocity and position of the current particle
//         at the next time step.
// @Govern equation : F(net) = ma (F is the net force that affects on the current particle)
//                    F(net) = surface tension + gravity + pressure force + viscosity.
//                    v(new) = v(old) + a * dt
//                    x(new) = x(old) + v * dt
void PCISPH::predictVelocityAndPosition() {

    ConcurrentUtils::ccLoop(currentFluidPosition.size(), 
    [&] (int i) {
        Vector3f acceleration = particleParams.inverseMass * (fluidForces[i] + fluidPressureForces[i]);
        newFluidVelocity[i] = currentFluidVelocity[i] + acceleration * timeStep;
        newFluidPosition[i] = currentFluidPosition[i] + newFluidVelocity[i] * timeStep;
    });
}

// @Func : This function calculates the density scaling factor that is applied to every 
//         particle in the current iteration, scaling factor is essential for dealing 
//         with the particles that have insufficient neighbours, which, otherwise would
//         produce falsified results. The density vairance scale factor is generally based
//         on all neighbour particles within the same kernel.
//         The density variance scale factor will be updated in each iteration.
void PCISPH::updateDensityVarianceScale() {

	Vector3f gradientSum = Vector3f(0.f,0.f,0.f);
    float sumSquaredGradient = 0.f; // Sum(W*W)
    for (float x = -kernelParams.radius - particleParams.radius; x <= kernelParams.radius + particleParams.radius; x += 2.f * particleParams.radius) {
        for (float y = -kernelParams.radius - particleParams.radius; y <= kernelParams.radius + particleParams.radius; y += 2.f * particleParams.radius) {
            for (float z = -kernelParams.radius - particleParams.radius; z <= kernelParams.radius + particleParams.radius; z += 2.f * particleParams.radius) {
                Vector3f r = Vector3f(x, y, z);
                float squaredR = r.squaredNorm();
                if (squaredR < kernelParams.squaredRadius) {
                    Vector3f gradient = W.poly6Grad1 * W.poly6Grad(r, squaredR);
                    gradientSum += gradient;
                    sumSquaredGradient += gradient.dot(gradient);
                }
            }
        }
    }
    
    float squaredSumGradient = gradientSum.dot(gradientSum);  // Sum(W) * Sum(W)
    float beta = 2.f * pow2((particleParams.mass * timeStep) / simConstParams.restDensity);
    densityVarianceScale = -1.f / (beta * (-squaredSumGradient - sumSquaredGradient));

}

void PCISPH::updatePressures() {

  ConcurrentUtils::ccLoop(currentFluidPosition.size(), 
  [this] (int i) {

        float fluidDensity = 0.f;
        fluidGrid.queryPair(kernelParams.radius,newFluidPosition, currentFluidPosition[i], newFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
            fluidDensity += W.poly6(r2);
        });
        float density = W.poly6C * particleParams.mass * fluidDensity;

        float boundaryDensity = 0.f;
        boundaryGrid.query(kernelParams.radius,boundaryPositions, newFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
            boundaryDensity += W.poly6(r2) * boundaryMass[j];;
        });
        density += W.poly6C * boundaryDensity;

        float densityVariation = std::max(0.f, density - simConstParams.restDensity);

        fluidPressures[i] += densityVarianceScale * densityVariation;
    });
   
}

void PCISPH::updatePressureForces() {

   ConcurrentUtils::ccLoop(currentFluidPosition.size(), 
   [&] (int i) {
        Vector3f pressureForce;

        fluidGrid.query(kernelParams.radius, currentFluidPosition, currentFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
            if (r2 < EPSILON) {
                return;
            }

            float rn = std::sqrt(r2);
            const float &density_i = fluidDensities[i];
            const float &density_j = fluidDensities[j];
            const float &pressure_i = fluidPressures[i];
            const float &pressure_j = fluidPressures[j];

            pressureForce -= particleParams.squaredMass * (pressure_i / pow2(density_i) + pressure_j / pow2(density_j)) * W.spikyGrad1 * W.spikyGrad(r, rn);
        });

        boundaryGrid.query(kernelParams.radius, boundaryPositions, currentFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
            if (r2 < EPSILON) {
                return;
            }

            float rn = std::sqrt(r2);

            const float &density_i = fluidDensities[i];
            const float &density_j = boundaryDensities[j];
            const float &pressure_i = fluidPressures[i];
            const float &pressure_j = fluidPressures[i];       
            pressureForce -= particleParams.mass * boundaryMass[j] * (pressure_i / pow2(density_i) + pressure_j / pow2(density_j)) * W.spikyGrad1 * W.spikyGrad(r, rn);
        });

        fluidPressureForces[i] = pressureForce;
    });

}

// @Func : Set the new fluid position and velocity
//         This function is called after the PCISPH 
//         loop is terminated.
//         After set the new positions and velocities, 
//         swap the current and predicted buffer.
void PCISPH::setVelocityAndPosition() {

   ConcurrentUtils::ccLoop(currentFluidPosition.size(), 
   [this] (int i) {
        Vector3f force = fluidForces[i] + fluidPressureForces[i];
        newFluidVelocity[i] = currentFluidVelocity[i] + particleParams.inverseMass * force * timeStep;
        newFluidPosition[i] = currentFluidPosition[i] + newFluidVelocity[i] * timeStep;
    });
    
    // Swap the buffer
    std::swap(newFluidPosition, currentFluidPosition);
    std::swap(newFluidVelocity, currentFluidVelocity);
}

// @Func : This function handles the situation when a particle collides with a 
//         boundary or fluid particle.
void PCISPH::adjustParticles() {

    handleCollisions(
    [&] (int i, const Vector3f &n, float d) {
        float c = 0.5f;
        currentFluidPosition[i] += n * d;
        currentFluidVelocity[i] -= (1 + c) * currentFluidVelocity[i].dot(n) * n;
    });

}

void PCISPH::adjustTimestep() {

}

void PCISPH::detectShocks() {

}

void SPH::handleCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler) {
    for (size_t i = 0; i < currentFluidPosition.size(); ++i) {
        const auto &p = currentFluidPosition[i];
        if (p.x() < boundaryBox.min.x()) {
            handler(i, Vector3f(1.f, 0.f, 0.f), boundaryBox.min.x() - p.x());
        }
        if (p.x() > boundaryBox.max.x()) {
            handler(i, Vector3f(-1.f, 0.f, 0.f), p.x() - boundaryBox.max.x());
        }
        if (p.y() < boundaryBox.min.y()) {
            handler(i, Vector3f(0.f, 1.f, 0.f), boundaryBox.min.y() - p.y());
        }
        if (p.y() > boundaryBox.max.y()) {
            handler(i, Vector3f(0.f, -1.f, 0.f), p.y() - boundaryBox.max.y());
        }
        if (p.z() < boundaryBox.min.z()) {
            handler(i, Vector3f(0.f, 0.f, 1.f), boundaryBox.min.z() - p.z());
        }
        if (p.z() > boundaryBox.max.z()) {
            handler(i, Vector3f(0.f, 0.f, -1.f), p.z() - boundaryBox.max.z());
        }
    }
}


// @Func : This method implements algorithm 2 proposed in paper.
void PCISPH::simulate(int maxIterations) {

	buildFluidGrids();
	testBoundary();
	initDensities();
	initNormals();
	initForces();      // Compute F(v,g,ext)

	int iterations = 0;
    
	while(iterations < maxIterations) {
       predictVelocityAndPosition();
       updateDensityVarianceScale();
       updatePressures();
       updatePressureForces();
       if((++k >= MIN_ITERATION) && maximumDensityVariance < maximumDensityVarianceTh) {
       	  break;
       }
	}

	setVelocityAndPosition();
	adjustParticles();
	adjustTimestep();
	detectShocks();
 
	currentTime += timeStep;
}

// @Func  Initialize bounding boxes based on the parsed scene data.
// @Tested : true
void PCISPH::initBoundary() {

  // Clear the existing boundary box data first.	
  boundaryBox.reset();  
  
  // Expand the bounding boxes based on the settings.
  for (const Vector3f &pos : boundaryPositions) {
  	 boundaryBox.expandBy(p);
  }

}

// @Func   Allowcate memories to matricies. 
// @Params fluidSize : size of fluid particles
// @Params boundarySize : size of boundary particles
// @Tested : true
void PCISPH::allocMemory(int fluidSize, int boundarySize) {
    
    // Fluid
    fluidDensities.resize(fluidSize);
    fluidPressures.resize(fluidSize);
    fluidForces.resize(fluidSize);
    fluidPressureForces.resize(fluidSize);
    
    newFluidPosition.resize(fluidSize);
	  currentFluidVelocity.resize(fluidSize);
	  newFluidVelocity.resize(fluidSize);
    
    fluidPositionBeforeShock.resize(fluidSize);
    fluidVelocityBeforeShock.resize(fluidSize);
    fluidNormals.resize(fluidSize);
   
    // Boundary
    boundaryAlive.resize(boundarySize);
	  boundaryMass.resize(boundarySize);
    boundaryDensities.resize(boundarySize);
    boundaryPressures.resize(boundarySize);
    initBoundary();
}

// @Func  : Foward the simulation by deltaT from the current time
//          and simulate this time interval.
// @Tested : truefn
void PCISPH::update(float deltaT) {
	float targetTime = currentTime + deltaT;
    while (currentTime < targetTime) {
        simulate();
    }
}

void SPH::buildFluidGrids() {
    fluidGrid.update(currentFluidPosition, 
    [&] (int i, int j) {
        std::swap(currentFluidPosition[i], currentFluidPosition[j]);
        std::swap(currentFluidVelocity[i], currentFluidVelocity[j]);
    });
}

void SPH::buildBoundaryGrids() {
    boundaryGrid.update(boundaryPositions, 
    [this] (int i, int j) {
        std::swap(boundaryPositions[i], boundaryPositions[j]);
        std::swap(boundaryNormals[i], boundaryNormals[j]);
    });
}

}
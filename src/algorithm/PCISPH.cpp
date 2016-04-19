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
   // TODO : Implement Scene.h
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

} 

// @Func : Build the simulation scene based on the parsed scene file.
// @Params scene : parsed scene object.
// @Tested : false
void PCISPH::buildScene(const Scene &scene) {

}

void PCISPH::buildFluidGrids() {

}

void PCISPH::buildBoundaryGrids() {

}

// TODO: Implement Particle.h
void PCISPH::generateFluidParticles(const Particle::Volume &volume) {

}

void PCISPH::generateBoundaryParticles(const Particle::Boundary &boundary) {

}

void PCISPH::massifyBoundary() {

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
        boundaryStatus[i] = fluidGrid.isIsolate(kernelParams.radius, currentFluidPosition, boundaryPositions[i]);
    });
}


void PCISPH::initDensities() {

}

void PCISPH::initNormals() {

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
	    
	    // First of all, we have to iterate through the grid to fetch all 
	    // adjacent particles that within the range of the kernel, which 
	    // local at the center of the current particle.
	    fluidGrid.query(kernelParams.radius, currentFluidPosition, currentFluidPosition[i], [&] (size_t j, const Vector3f &r, float r2) {
	        
	     
	        if (r2 < EPSILON) {
	            return;
	        }
	 
	        float absxij = std::sqrt(r2);

	        viscocity -= (currentFluidVelocity[i] - currentFluidVelocity[j]) * (W.viscosityLaplace(absxij) / fluidDensities[j]);
	        
	        // K(i,j) is the surface tension constant 
	        // Basically F(sf) = K(i,j) * (F(cohesion)+F(curvature))
	        float Kij = 2.f * simConstParams.restDensity / (fluidDensities[i] + fluidDensities[j]);
	        cohesion += Kij * (r / absxij) * W.surfaceTension(absxij);
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

void PCISPH::predictVelocityAndPosition() {

}

void PCISPH::updateDensityVarianceScale() {

}

void PCISPH::updatePressures() {

}

void PCISPH::updatePressureForces() {

}

void PCISPH::setVelocityAndPosition() {

}

void PCISPH::adjustParticles() {

}

void PCISPH::adjustTimestep() {

}

void PCISPH::detectShocks() {

}

void PCISPH::handleCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler) {

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
    boundaryStatus.resize(boundarySize);
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

}
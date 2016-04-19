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

void PCISPH::initForces() {

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
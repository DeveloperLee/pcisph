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
   // TODO : Load parameters from the scene file.
   
   
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

// @Func : Build the simulation scene based on the parsed scene file.
// @Params scene : parsed scene object.
// @Tested : false
void PCISPH::buildScene(const Scene &scene) {

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



}
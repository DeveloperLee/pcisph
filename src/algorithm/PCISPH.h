//**
/* This class implements the PCI-SPH fluid simulation algorithm based on paper : 
/* "Predictive-Corrective Incompressible SPH (PCISPH)" : https://graphics.ethz.ch/~sobarbar/papers/Sol09/Sol09.pdf
/* Besides impelementing PCI-SPH, we also integrated extra features such as handling boundary particles, 
/* dynamically adjusted timesteps and surface tension approximations.
/*
/* Zihao Li 2016/4/16.
**/

#pragma once

#include "Kernel.h"

#include "visualization/grid/Grid.h"

#include "utils/STD.h"
#include "utils/Vector.h"
#include "utils/Box.h"
#include "utils/Math.h"
#include "utils/ConcurrentUtils.h"

#include <vector>
#include <numeric>

#define KERNEL_SCALE 4.f
#define RELAX_ITERATION 10000
#define MIN_ITERATION 3
#define EPSILON 1e-7f
namespace cs224 {

class PCISPH {

typedef std::vector<Mesh>   PCIMeshM;

public: 

     // --------- Particle physical properties ---------
	 struct ParticleParameters {
        
         float radius;
         float diameter;
         float mass;
         float squaredMass; 
         float inverseMass; //  1 / mass

         void init(float r, float restDensity) {
           radius = r;
           diameter = 2.f * r;
           mass = restDensity * pow3(diameter) / 1.1f;
           squaredMass = pow2(mass);
           inverseMass = 1.f / mass;
       }   
	 };
	 ParticleParameters particleParams;
	 const ParticleParameters &getParticleParams() const {return particleParams;}


	 // --------- Kernel properties ---------
	 struct KernelParameters {
         float scale = 4.f;
         float radius;
         float squaredRadius;
         int   capacity;

         void init(float s, float r, float d) {
         	scale = s;
         	radius = scale * r;
         	squaredRadius = pow2(radius);
         	capacity = int(std::ceil(4.f / 3.f * PI * pow3(radius)) / pow3(d));
         }
	 };
	 KernelParameters kernelParams;
	 const KernelParameters &getKernelParams() const {return kernelParams;}


	 // --------- Constant Simulation parameters ---------
	 // These parameters should be constant during the simulation.
	 struct SimConstParameters { 
         float restDensity;         // Incompressible fluid property: reference density, used for determining the density error.
         float surfaceTension;      // The scale of surface tension, the bigger this parameter, the bigger the surface tension is.
         float viscosity;           // The scale of viscosity.
         float maxCompressionf;     // The maximum compression that the simulation allows. 
         Vector3f gravity;          // The gravity of the simulation space.
         
         void init(float _resD, float _surfT, float _vis, float _maxComp, Vector3f _g) {
            restDensity = _resD;
            surfaceTension = _surfT;
            viscosity = _vis;
            maxCompression = _maxComp;
            gravity = _g;
         }
	 };
	 SimConstParameters simConstParams;
	 const SimConstParameters &getSimConstParams() const {return simConstParams;}


	 // --------- Simulation parameters ---------
	 // These parameters are dynamically changing during the simulation.
	 // Struct is not efficient here.
	 // Values:
 	 float densityVarianceScale;
     float maximumDensityVariance;
     float averageDensityVariance;
     float maximumDensityVarianceTh;
     float averageDensityVarianceTh;
     float previousMaxDensityVariance;
     float maximumVelocity;
     float maximumForce;
     float timeStep;
     float currentTime;
     float timeBeforeShock;


     // --------- Public functions ---------
     // Following functions will be invoked before the simulation loop
     PCISPH(const Scene &scene);         // The simulation must be initialized with a parsed scene.
     virtual ~PCISPH();
     void loadParams(Settings &settings);
     void buildScene(const Scene &scene);  // Build the parsed scene.
     void allocMemory(int fluidSize, int boundarySize); 
     void buildBoundaryGrids();          // Build boundary grids before the simulation start.
     void buildFluidGrids();             // Build fluid grids. 
     void massifyBoundary();             // Give every boundary particle a mass.
     void basicSimSetup();    
     void generateFluidParticles(const ParticleGenerator::Volume &volume);
     void generateBoundaryParticles(const ParticleGenerator::Boundary &boundary);
     void initDensities();        		 // Initialize densities for both fluid and boundary particles.
     void initShocks();                  // Initialize shock buffers.
     void initNormals();          		 // Initialize normal vectors for fluid particles.
     void initForces();           		 // Initialize internal and external forces.
     void initBoundary();                // Initialize bounding boxes.
     void testBoundary();         		 // Test the validness of each particle.
   
     // Simulation trigger function.
     void simulate(int maxIterations = 100); 
     void update(float deltaT);        

     // Following functions will be invoked during the simulation loop.
     void predictVelocityAndPosition();  // Predict the velocity and position after a short time step.
     void updateDensityVarianceScale();  // Update the density variance scale parameter.
     void updatePressures();
     void updatePressureForces();

     // Following functions will be invoked after the simulation loop.
     void setVelocityAndPosition();
     void setDebugger();
     void adjustParticles(); 
     void adjustTimestep();
     void detectShocks();
     void handleCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler);    
   
     void relax();                 // Relax the particle distribution.
   
     // Getter functions
     const Box3f &getBounds() const { return boundaryBox; }
     float getTimeStep() const { return timeStep; }
     float getCurrentTime() const { return currentTime; }

     const PCI3Mf 	 &getFluidPositions()    const { return currentFluidPosition; }
           PCI3Mf 	 &getFluidVelocities()         { return currentFluidVelocity; }
     const PCI3Mf 	 &getBoundaryPositions() const { return boundaryPositions; }
     const PCI3Mf 	 &getBoundaryNormals()   const { return boundaryNormals; }
     const PCIMeshM  &getBoundaryMeshes()    const { return boundaryMeshes; }


private:

     // N * M (row * column) vector array:
     // Fluid particles:
     PCI1Mf fluidDensities;
     PCI1Mf fluidPressures;
     PCI3Mf fluidForces;
     PCI3Mf fluidPressureForces;

     PCI3Mf currentFluidPosition;
     PCI3Mf newFluidPosition;
     PCI3Mf currentFluidVelocity;
     PCI3Mf newFluidVelocity;

     PCI3Mf fluidPositionBeforeShock;
     PCI3Mf fluidVelocityBeforeShock;
     PCI3Mf fluidNormals;

     // Boundary particles:
     PCI1Mi boundaryAlive;
     PCI1Mf boundaryMass;
     PCI1Mf boundaryDensities;
     PCI1Mf boundaryPressures;
     PCI3Mf boundaryPositions;
     PCI3Mf boundaryNormals;
     PCIMeshM boundaryMeshes;
     

     // --------- Dependencies --------- 
     Grid fluidGrid;
     Grid boundaryGrid;
     Kernel W;
     Box3f boundaryBox;  // Bounding box for the whole scene

};

}
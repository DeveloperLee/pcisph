//**
/* This class implements the PCI-SPH fluid simulation algorithm based on paper : 
/* "Predictive-Corrective Incompressible SPH (PCISPH)" : https://graphics.ethz.ch/~sobarbar/papers/Sol09/Sol09.pdf
/* Besides impelementing PCI-SPH, we also integrated extra features such as handling boundary particles, 
/* dynamically adjusted timesteps and surface tension approximations.
/*
/* Zihao Li 2016/4/16.
**/

#pragma once 

#include "basics/Common.h"
#include "basics/Vector.h"
#include "basics/Box.h"

#include <tbb/tbb.h>
#include <vector>
#include <numeric>

#define PI 3.141593

namespace cs224 {
    
    // Define some useful data structures for storing
    // fluid/boundary particle properties and status.
	typedef std::vector<Vector3f>  PCI3Mf;
	typedef std::vector<float>     PCI1Mf;
	typedef std::vector<int>       PCI1Mi;
	typedef std::vector<Mesh>      PCIMeshM;


class PCISPH {


public: 

     // --------- Particle physical properties ---------
	 struct ParticleParameters {
        
         float radius = 0.01f;
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
         	radius = r;
         	squaredRadius = pow2(r);
         	capacity = int(std::ceil(4.f / 3.f * PI * pow3(radius)) / pow3(d));
         }
	 };
	 KernelParameters kernelParams;
	 const KernelParameters &getKernelParams() const {return kernelParams;}


	 // --------- Constant Simulation parameters ---------
	 // These parameters should be constant during the simulation.
	 struct SimConstParameters { 
         float restDensity = 1000.f;   // Incompressible fluid property: reference density, used for determining the density error.
         float surfaceTension = 1.f;   // The scale of surface tension, the bigger this parameter, the bigger the surface tension is.
         float viscosity = 0.f;        
         float maxCompression = 0.02f; // The maximum compression that the simulation allows. 
         Vector3f gravity = Vector3f(0.f, -9.8f, 0.f);  // The gravity of the simulation space.

	 };
	 SimConstParameters simConstParams;
	 const SimConstParameters &getKernelParams() const {return simConstParams;}


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
     float timeStep = 0.001f; 
     float currentTime;
     float timeBeforeShock;



     // --------- Public functions ---------
     // Following functions will be invoked before the simulation loop
     PCISPH(const Scene &scene);         // The simulation must be initialized with a parsed scene.
     virtual ~PCISPH();
     void initPCISPH();
     void buildScene(const Scene &scene);  // Build the parsed scene.
     void generateFluidParticles(const ParticleGenerator::Volume &volume);
     void generateBoundaryParticles(const ParticleGenerator::Boundary &boundary);
     void buildBoundaryGrids();          // Build boundary grids before the simulation start.
     void buildFluidGrids();      		 // Build fluid grids.
     void massifyBoundary();       		 // Give every boundary particle a mass.
     void initDensities();        		 // Initialize densities for both fluid and boundary particles.
     void initNormals();          		 // Initialize normal vectors for fluid particles.
     void initForces();           		 // Initialize internal and external forces.
     void testBoundary();         		 // Test the validness of each particle.
   
     // Simulation trigger function.
     void simulate(); 
     void update(float deltaT);
     void updateStep();          

     // Following functions will be invoked during the simulation loop.
     void predictVelocityAndPosition();  // Predict the velocity and position after a short time step.
     void updateDensityVarianceScale();  // Update the density variance scale parameter.
     void updatePressures();
     void updatePressureForces();

     // Following functions will be invoked after the simulation loop.
     void setVelocityAndPosition();
     void setDebugger();
     void adjustParticles(); 
     void handleCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler);
     void adjustTimestep();        
   

     void getMinMaxDensity(float &min, float &max);
     void relax();                 // Relax the particle distribution.
     void resetVelocity();
     void resetPosition(); 
     void resetTime();
     
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
     PCI1Mi boundaryStatus;
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
     Box3f boundaryBox;

};

}
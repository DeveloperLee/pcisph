#pragma once

#include "Kernel.h"

#include "visualization/scene/Scene.h"
#include "visualization/grid/Grid.h"
#include "visualization/mesh/Mesh.h"
#include "visualization/objLoader/ObjLoader.h"
#include "visualization/geometry/Voxelizer.h"
#include "visualization/geometry/VoxelGrid.h"
#include "visualization/particle/Particle.h"

#include "utils/Def.h"
#include "utils/Settings.h"
#include "utils/ConcurrentUtils.h"

#include <vector>
#include <numeric>

#define KERNEL_SCALE 4.f
#define RELAX_ITERATION 10000
#define MIN_ITERATION 3
#define EPSILON 1e-7f
#define PCI_INFINITY std::numeric_limits<float>::infinity()

namespace cs224 {

typedef std::vector<Mesh>      PCIMeshM;

class SPH {
public:

    struct ParticleParameters {
       float radius;
       float diameter;
       float mass;
       float squaredMass;
       float inverseMass;

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
         float scale;
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
         float restDensity;   // Incompressible fluid property: reference density, used for determining the density error.
         float surfaceTension;   // The scale of surface tension, the bigger this parameter, the bigger the surface tension is.
         float viscosity;
         float maxCompression; // The maximum compression that the simulation allows.
         Vector3f gravity;  // The gravity of the simulation space.
         Vector3f initVelocity;

         void init(float _resD, float _surfT, float _vis, float _maxComp, Vector3f _g, Vector3f _iv) {
            restDensity = _resD;
            surfaceTension = _surfT;
            viscosity = _vis;
            maxCompression = _maxComp;
            gravity = _g;
            initVelocity = _iv;
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
     float currentTime = 0.f;
     float timeBeforeShock;

    SPH(const Scene &scene);
    void simulate(int maxIterations = 100);

    // Get the current simulation loop status
    const Box3f &getBounds() const { return boundaryBox; }
    float getTimeStep() const { return timeStep; }
    float getCurrentTime() const { return currentTime; }

    const PCI3Mf 	 &getFluidPositions()    const { return currentFluidPosition; }
          PCI3Mf 	 &getFluidVelocities()         { return currentFluidVelocity; }
    const PCI3Mf 	 &getBoundaryPositions() const { return boundaryPositions; }
    const PCI3Mf 	 &getBoundaryNormals()   const { return boundaryNormals; }
    const PCIMeshM   &getBoundaryMeshes()    const { return boundaryMeshes; }

private:

    void basicSimSetup();

    // Shared update methods
    void testBoundary();
    void buildBoundaryGrids();
    void massifyBoundary();
    void initDensities();
    void initNormals();
    void initBoundary();
    void loadParams(const Settings &settings);
    void relax();
    void allocMemory(int fluidSize, int boundarySize);

    void buildFluidGrids();
    void updateDensityVarianceScale();
    void initForces();
    void predictVelocityAndPosition();
    void updatePressures();
    void updatePressureForces();
    void setVelocityAndPosition();

    void handleCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler);
    void adjustParticles();
    void adjustTimeStep();
    bool isShock();
    void handleShock();

    void buildScene(const Scene &scene);
    void generateFluidParticles(const ParticleGenerator::Volume &volume);
    void generateBoundaryParticles(const ParticleGenerator::Boundary &boundary);

     // N * M (row * column) vector array:
     // Fluid particles:
     PCI1Mf fluidDensities;
     PCI1Mf fluidPressures;
     PCI3Mf fluidForces;
     PCI3Mf fluidPressureForces;
     PCI3Mf fluidNormals;

     // Fluid buffers:
     PCI3Mf currentFluidPosition;
     PCI3Mf newFluidPosition;
     PCI3Mf currentFluidVelocity;
     PCI3Mf newFluidVelocity;
     PCI3Mf fluidPositionBeforeShock;
     PCI3Mf fluidVelocityBeforeShock;


     // Boundary particles:
     PCI1Mi boundaryAlive;
     PCI1Mf boundaryMass;
     PCI1Mf boundaryDensities;
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

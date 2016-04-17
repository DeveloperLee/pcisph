#pragma once

#include "Scene.h"
#include "Grid.h"
#include "Kernel.h"

#include "basics/Common.h"
#include "basics/Vector.h"
#include "basics/Box.h"
#include "basics/AlignedAllocator.h"
#include "basics/Timer.h"

#include "utils/Math.h"
#include "utils/ConcurrentUtils.h"
#include "utils/Exception.h"

#include "geo/Mesh.h"
#include "geo/ObjReader.h"
#include "geo/Voxelizer.h"
#include "geo/VoxelGrid.h"
#include "geo/ParticleGenerator.h"

#include <tbb/tbb.h>

#include <vector>
#include <numeric>

namespace cs224 {

// This is a generic n * 3 vector for storing 
// particle status and properties.     
typedef std::vector<Vector3f> SPHVector3f;
typedef std::vector<float>    SPHVector1f;
typedef std::vector<Mesh>     SPHVectorM;
typedef std::vector<int>      SPHVector1i;

class SPH {
public:   
    
    // Particle params
    float pRadius = 0.01f;
    float pDiameter;
    float pMass, pMassSqr, pMassInverse;  // All particles have identical mass
    
    // Kernel params
    int kScale = 4;  // This controlls the range of the kernel
    float kRadius, kRadiusSqr;
    int kCapacity;  // How many particles can the kernel contains
    
    // Simulation params
    float sRestDensity = 1000.f;  // Imcompressible : rest density 
    float sSurfaceTension = 1.f;  // Scale of surface tension force
    float sViscosity = 0.f;       // Scale of viscosity force
    float sTimeStep = 0.001f;     // Initial simulation time step
    float sMaxCompression = 0.02f; // The maximum compression that allows to occur

    float maxDVT, avgDVT;
    float DVS;
    float maxDV, avgDV;
    float prevMaxDV = 1000.f;
    float maxV, maxF;
    
     // By default, the gravity force points toward negative y axis.
    Vector3f G = Vector3f(0.f, -9.81f, 0.f);
  
    // Simulation parameters
    struct Parameters {
        float particleRadius;
        float particleDiameter;
        float kernelRadius;
        int   kernelSupportParticles;
        float particleMass;
        float restDensity;
    };
    const Parameters &parameters() const { return params; }

    SPH(const Scene &scene);

    void update(float dt);
    void updateStep();
    int calcKernelCapacity();

    const Box3f &bounds() const { return boundary; }
    float timeStep() const { return sTimeStep; }
    float time() const { return T; }
    
    // Getters
    const SPHVector3f &fluidPositions() const { return fluidPosOld; }
          SPHVector3f &fluidVelocities()       { return fluidVOld; }
    const SPHVector3f &boundaryPositions() const { return boundPos; }
    const SPHVector3f &boundaryNormals() const { return boundNor; }
    const SPHVectorM &boundaryMeshes() const { return boundMeshes; }

private:

    void init();

    // Shared update methods
    void activateBoundaryParticles();
    void updateBoundaryGrid();
    void updateBoundaryMasses();
    void updateDensities();
    void updateNormals();
    void pcisphUpdate(int maxIterations = 100);
    void pcisphUpdateGrid();
    void updateDVS();
    void initForces();
    void predictVP();
    void updatePressures();
    void updatePressureF();
    void updateVP();   
  
    void computeCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler);
    void enforceBounds();

    void buildScene(const Scene &scene);
    void addFluidParticles(const ParticleGenerator::Volume &volume);
    void addBoundaryParticles(const ParticleGenerator::Boundary &boundary);

    float T = 0.f;
    float timeBeforeShock;

    // Fluid particle properties.
    SPHVector3f fluidPosOld, fluidPosNew;
    SPHVector3f fluidVOld, fluidVNew;
    SPHVector3f fluidPosBeforeShock;
    SPHVector3f fluidVBeforeShock;
    SPHVector3f fluidNor;
    SPHVector3f fluidF;
    SPHVector3f fluidPF;
    SPHVector1f fluidD;
    SPHVector1f fluidP;
    Grid fluidGrid;

    // Boundary particle properties.
    SPHVector3f boundPos;
    SPHVector3f boundNor;
    SPHVector1f boundD;
    SPHVector1f boundP;
    SPHVector1f boundM;
    SPHVector1i boundActive;
    Grid boundGrid;

    SPHVectorM boundMeshes;
    Parameters params;
    Kernel W;
    Box3f boundary;
};
} 

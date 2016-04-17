#include "SPH.h"

#include "basics/DebugMonitor.h"

#include <tbb/enumerable_thread_specific.h>

// [1] Weakly compressible SPH for free surface flows
// [2] Predictive-Corrective Incompressible SPH
// [3] Versatile Surface Tension and Adhesion for SPH Fluids
// [4] Versatile Rigid-Fluid Coupling for Incompressible SPH

namespace cs224 {


SPH::SPH(const Scene &scene) {
    // Load scene settings
    pRadius = scene.settings.getFloat("particleRadius", pRadius);
    sRestDensity = scene.settings.getFloat("restDensity", sRestDensity);
    G = scene.settings.getVector3("gravity", G);
    sSurfaceTension = scene.settings.getFloat("surfaceTension", sSurfaceTension);
    sViscosity = scene.settings.getFloat("viscosity", sViscosity);
    sTimeStep = scene.settings.getFloat("timeStep", sTimeStep);
    sMaxCompression = scene.settings.getFloat("compressionThreshold", sMaxCompression);

    // Compute derived constants
    pDiameter = 2.f * pRadius;

    kRadius = kScale * pRadius;
    kRadiusSqr = pow2(kRadius);
    kCapacity = calcKernelCapacity();

    pMass = sRestDensity * pow3(pDiameter) / 1.15f;
    pMassSqr = pow2(pMass);
    pMassInverse = 1.f / pMass;

    avgDVT = sMaxCompression * sRestDensity;
    maxDVT = avgDVT * 10.f;

    // Store parameters
    params.particleRadius = pRadius;
    params.particleDiameter = pDiameter;
    params.kernelRadius = kRadius;
    params.kernelSupportParticles = kCapacity;
    params.particleMass = pMass;
    params.restDensity = sRestDensity;

    buildScene(scene);

    // Compute bounds
    boundary.reset();
    for (const auto &p : boundPos) {
        boundary.expandBy(p);
    }

    fluidVOld.resize(fluidPosOld.size());
    fluidPosNew.resize(fluidPosOld.size());
    fluidVNew.resize(fluidPosOld.size());
    fluidPosBeforeShock.resize(fluidPosOld.size());
    fluidVBeforeShock.resize(fluidPosOld.size());
    fluidNor.resize(fluidPosOld.size());
    fluidF.resize(fluidPosOld.size());
    fluidPF.resize(fluidPosOld.size());
    fluidD.resize(fluidPosOld.size());
    fluidP.resize(fluidPosOld.size());

    boundD.resize(boundPos.size());
    boundP.resize(boundPos.size());
    boundM.resize(boundPos.size());
    boundActive.resize(boundPos.size());

    W.init(kRadius);
    fluidGrid.init(boundary, kRadius);
    boundGrid.init(boundary, kRadius);

    // Preprocessing
    updateBoundaryGrid();
    updateBoundaryMasses();

    //PRINT("Initializing simulation ...");
    // Initialize pci-sph simulation
     init();
}

void SPH::update(float dt) {
    float targetTime = T + dt;
    while (T < targetTime) {
        updateStep();
    }
}

void SPH::updateStep() {
    pcisphUpdate();
}

int SPH::calcKernelCapacity() {
    return  int(std::ceil((4.f / 3.f * M_PI * pow3(kRadius)) / pow3(pDiameter)));
}

// Activate all boundary particles that are nearby fluid particles
void SPH::activateBoundaryParticles() {
     ConcurrentUtils::ccLoop(boundPos.size(), [this] (size_t i) {
        boundActive[i] = fluidGrid.isIsolate(kRadius, fluidPosOld, boundPos[i]);
    });
}

void SPH::updateBoundaryGrid() {
    boundGrid.update(boundPos, [this] (size_t i, size_t j) {
        std::swap(boundPos[i], boundPos[j]);
        std::swap(boundNor[i], boundNor[j]);
    });
}

// Compute the approximate mass of boundary particles based on [4] equation 4 and 5
void SPH::updateBoundaryMasses() {
     ConcurrentUtils::ccLoop(boundPos.size(), [this] (size_t i) {
        float weight = 0.f;
        boundGrid.query(kRadius, boundPos, boundPos[i], [this, &weight] (size_t j, const Vector3f &r, float r2) {
            weight += W.poly6(r2);
        });
        boundM[i] = sRestDensity / (W.poly6C * weight) / 1.17f;
    });
}

void SPH::updateDensities() {

     ConcurrentUtils::ccLoop(boundPos.size(), [this] (size_t i) {
        
        // If a particle doesn't have neighbours, skip.
        if (!boundActive[i]) {
            return;
        }
        float fluidDensity = 0.f;
        fluidGrid.query(kRadius, fluidPosOld, boundPos[i], [this, &fluidDensity] (size_t j, const Vector3f &r, float r2) {
            fluidDensity += W.poly6(r2);
        });
        float boundaryDensity = 0.f;
        boundGrid.query(kRadius, boundPos, boundPos[i], [this, &boundaryDensity] (size_t j, const Vector3f &r, float r2) {
            boundaryDensity += W.poly6(r2) * boundM[j];
        });
        float density = W.poly6C * pMass * fluidDensity;
        density += W.poly6C * boundaryDensity;

        boundD[i] = density;
    });

     ConcurrentUtils::ccLoop(fluidPosOld.size(), [this] (size_t i) {
        float fluidDensity = 0.f;
        fluidGrid.query(kRadius, fluidPosOld, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            fluidDensity += W.poly6(r2);
        });
        float density = W.poly6C * pMass * fluidDensity;
        float boundaryDensity = 0.f;
        boundGrid.query(kRadius, boundPos, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            boundaryDensity += W.poly6(r2) * boundM[j];;
        });
        density += W.poly6C * boundaryDensity;

        fluidD[i] = density;
    });
}

// Compute normals based on [3]
void SPH::updateNormals() {
     ConcurrentUtils::ccLoop(fluidPosOld.size(), [this] (size_t i) {
        Vector3f normal;
        fluidGrid.query(kRadius, fluidPosOld, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            normal += W.poly6Grad(r, r2) / fluidD[j];
        });
        normal *= kRadius * pMass * W.poly6Grad1;
        fluidNor[i] = normal;
    });
}

void SPH::computeCollisions(std::function<void(size_t i, const Vector3f &n, float d)> handler) {
    for (size_t i = 0; i < fluidPosOld.size(); ++i) {
        const auto &p = fluidPosOld[i];
        if (p.x() < boundary.min.x()) {
            handler(i, Vector3f(1.f, 0.f, 0.f), boundary.min.x() - p.x());
        }
        if (p.x() > boundary.max.x()) {
            handler(i, Vector3f(-1.f, 0.f, 0.f), p.x() - boundary.max.x());
        }
        if (p.y() < boundary.min.y()) {
            handler(i, Vector3f(0.f, 1.f, 0.f), boundary.min.y() - p.y());
        }
        if (p.y() > boundary.max.y()) {
            handler(i, Vector3f(0.f, -1.f, 0.f), p.y() - boundary.max.y());
        }
        if (p.z() < boundary.min.z()) {
            handler(i, Vector3f(0.f, 0.f, 1.f), boundary.min.z() - p.z());
        }
        if (p.z() > boundary.max.z()) {
            handler(i, Vector3f(0.f, 0.f, -1.f), p.z() - boundary.max.z());
        }
    }
}

void SPH::enforceBounds() {
    computeCollisions([&] (size_t i, const Vector3f &n, float d) {
        float c = 0.5f;
        fluidPosOld[i] += n * d;
        fluidVOld[i] -= (1 + c) * fluidVOld[i].dot(n) * n;
    });
}

void SPH::pcisphUpdateGrid() {
    fluidGrid.update(fluidPosOld, [&] (size_t i, size_t j) {
        std::swap(fluidPosOld[i], fluidPosOld[j]);
        std::swap(fluidVOld[i], fluidVOld[j]);
    });
}

// This function calculates the density scaling factor that is applied to every 
// particle in the current iteration, scaling factor is essential for dealing 
// with the particles that have insufficient neighbours, which, otherwise would
// produce falsified results.
void SPH::updateDVS() {
   
    Vector3f gradSum;
    float gradDotSum = 0.f;
    for (float x = -kRadius - pRadius; x <= kRadius + pRadius; x += 2.f * pRadius) {
        for (float y = -kRadius - pRadius; y <= kRadius + pRadius; y += 2.f * pRadius) {
            for (float z = -kRadius - pRadius; z <= kRadius + pRadius; z += 2.f * pRadius) {
                Vector3f r = Vector3f(x, y, z);
                float r2 = r.squaredNorm();
                if (r2 < kRadiusSqr) {
                    Vector3f grad = W.poly6Grad1 * W.poly6Grad(r, r2);
                    gradSum += grad;
                    gradDotSum += grad.dot(grad);
                }
            }
        }
    }

    float beta = 2.f * pow2((pMass * sTimeStep) / sRestDensity);
    DVS = -1.f / (beta * (-gradSum.dot(gradSum) - gradDotSum));
}

// In this implementation the initial force is mainly based on three components
// 1. Viscosity
// 2. Surface tension : Cohesive term + Curvature term
// 3. Gravity : F = mg 
// After calculating the intial force, set pressure and pressure force to 0
// according to the PCISPH algorithm.
void SPH::initForces() {
     ConcurrentUtils::ccLoop(fluidPosOld.size(), [&] (size_t i) {
        
        // Terms for computing F(v,g,ext) in the paper algorithm.
        Vector3f viscocity;
        Vector3f cohesion; 
        Vector3f curvature; 
        
        // First of all, we have to iterate through the grid to fetch all 
        // adjacent particles that within the range of the kernel, which 
        // local at the center of the current particle.
        fluidGrid.query(kRadius, fluidPosOld, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            const Vector3f &v_i = fluidVOld[i];
            const Vector3f &v_j = fluidVOld[j];
            const Vector3f &n_i = fluidNor[i];
            const Vector3f &n_j = fluidNor[j];
            const float &density_i = fluidD[i];
            const float &density_j = fluidD[j];
     
            if (r2 < 1e-7f) {
                return;
            }
     
            float absxij = std::sqrt(r2);

            viscocity -= (v_i - v_j) * (W.viscosityLaplace(absxij) / density_j);
            
            // K(i,j) is the surface tension constant 
            // Basically F(sf) = K(i,j)*(F(cohesion)+F(curvature))
            float Kij = 2.f * sRestDensity / (density_i + density_j);
            cohesion += Kij * (r / absxij) * W.surfaceTension(absxij);
            curvature += Kij * (n_i - n_j);
        });

        viscocity *= sViscosity * pMassSqr * W.viscosityGrad2 / fluidD[i];
        cohesion  *= -sSurfaceTension * pMassSqr * W.surfaceTensionConstant;
        curvature *= -sSurfaceTension * pMass;
        
        // The overall force F(p,i)
        Vector3f force;
        force += cohesion + curvature + viscocity;
        force += pMass * G;

        fluidF[i] = force;
        fluidP[i] = 0.f;
        fluidPF[i] = Vector3f(0.f);
    });
}

void SPH::predictVP() {
     ConcurrentUtils::ccLoop(fluidPosOld.size(), [&] (size_t i) {
        Vector3f a = pMassInverse * (fluidF[i] + fluidPF[i]);
        fluidVNew[i] = fluidVOld[i] + a * sTimeStep;
        fluidPosNew[i] = fluidPosOld[i] + fluidVNew[i] * sTimeStep;
    });
}

void SPH::updatePressures() {
    tbb::enumerable_thread_specific<float> maxDensityVariation(-std::numeric_limits<float>::infinity());
    tbb::enumerable_thread_specific<float> accDensityVariation(0.f);

     ConcurrentUtils::ccLoop(fluidPosOld.size(), [&] (size_t i) {
        float fluidDensity = 0.f;
        fluidGrid.queryPair(kRadius,fluidPosNew, fluidPosOld[i], fluidPosNew[i], [&] (size_t j, const Vector3f &r, float r2) {
            fluidDensity += W.poly6(r2);
        });
        float density = W.poly6C * pMass * fluidDensity;

        float boundaryDensity = 0.f;
        boundGrid. query(kRadius,boundPos, fluidPosNew[i], [&] (size_t j, const Vector3f &r, float r2) {
            boundaryDensity += W.poly6(r2) * boundM[j];;
        });
        density += W.poly6C * boundaryDensity;

        float densityVariation = std::max(0.f, density - sRestDensity);
        maxDensityVariation.local() = std::max(maxDensityVariation.local(), densityVariation);
        accDensityVariation.local() += densityVariation;

        fluidP[i] += DVS * densityVariation;
    });

    maxDV = std::accumulate(maxDensityVariation.begin(), maxDensityVariation.end(), 0.f, [] (float a, float b) { return std::max(a, b); });
    avgDV = std::accumulate(accDensityVariation.begin(), accDensityVariation.end(), 0.f) / fluidPosOld.size();

}

void SPH::updatePressureF() {
     ConcurrentUtils::ccLoop(fluidPosOld.size(), [&] (size_t i) {
        Vector3f pressureForce;

        fluidGrid.query(kRadius, fluidPosOld, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            if (r2 < 1e-5f) {
                return;
            }

            float rn = std::sqrt(r2);
            const float &density_i = fluidD[i];
            const float &density_j = fluidD[j];
            const float &pressure_i = fluidP[i];
            const float &pressure_j = fluidP[j];

            pressureForce -= pMassSqr * (pressure_i / pow2(density_i) + pressure_j / pow2(density_j)) * W.spikyGrad1 * W.spikyGrad(r, rn);
        });

        boundGrid.query(kRadius, boundPos, fluidPosOld[i], [&] (size_t j, const Vector3f &r, float r2) {
            if (r2 < 1e-5f) {
                return;
            }

            float rn = std::sqrt(r2);

            const float &density_i = fluidD[i];
            const float &density_j = boundD[j];
            const float &pressure_i = fluidP[i];
            const float &pressure_j = fluidP[i];       
            pressureForce -= pMass * boundM[j] * (pressure_i / pow2(density_i) + pressure_j / pow2(density_j)) * W.spikyGrad1 * W.spikyGrad(r, rn);
        });

        fluidPF[i] = pressureForce;
    });
}

// This is the final step of the algorithm, which updates particle's 
// position and velocity based on the result of the current iteration.
void SPH::updateVP() {
    tbb::enumerable_thread_specific<float> maxVelocity(0.f);
    tbb::enumerable_thread_specific<float> maxForce(0.f);

     ConcurrentUtils::ccLoop(fluidPosOld.size(), [&] (size_t i) {
        Vector3f force = fluidF[i] + fluidPF[i];
        maxForce.local() = std::max(maxForce.local(), force.squaredNorm());
        fluidVNew[i] = fluidVOld[i] + pMassInverse * force * sTimeStep;
        fluidPosNew[i] = fluidPosOld[i] + fluidVNew[i] * sTimeStep;
        maxVelocity.local() = std::max(maxVelocity.local(), fluidVOld[i].squaredNorm());
    });

    maxV = std::sqrt(std::accumulate(maxVelocity.begin(), maxVelocity.end(), 0.f, [] (float a, float b) { return std::max(a, b); }));
    maxF = std::sqrt(std::accumulate(maxForce.begin(), maxForce.end(), 0.f, [] (float a, float b) { return std::max(a, b); }));
    
    // Swap the buffer
    std::swap(fluidPosNew, fluidPosOld);
    std::swap(fluidVNew, fluidVOld);
}

void SPH::init() {
    // Compute densities
    pcisphUpdateGrid();
    updateDensities();
    
    float mind = std::numeric_limits<float>::infinity();
    float maxd = -std::numeric_limits<float>::infinity();
    for (auto d : fluidD) {
        mind = std::min(mind, d);
        maxd = std::max(maxd, d);
    }

    fluidPosBeforeShock = fluidPosOld;
    fluidVBeforeShock = fluidVOld;

    // Relax initial particle distribution and reset velocities
    pcisphUpdate(10000);
    for (auto &v : fluidVOld) {
        v = Vector3f(0.f);
    }

    T = 0.f;
    timeBeforeShock = 0.f;
}


// This function implements the pci-sph algorithm mentioned in
// Alogrithm 2 in the paper.
void SPH::pcisphUpdate(int maxIterations) {
    DebugMonitor::clear();   
    pcisphUpdateGrid();
    activateBoundaryParticles();
    updateDensities();
    updateNormals();
    initForces();
    
    int k = 0;
    while (k < maxIterations) {
        predictVP();
        updateDVS();
        updatePressures();
        updatePressureF();
        ++k;
        if (k >= 3 && maxDV < maxDVT) {
            break;
        }
    }
    
    DebugMonitor::addItem("pressureIterations", "%d", k);
    updateVP();
    enforceBounds();
    DebugMonitor::addItem("fluidParticles", "%d", fluidPosOld.size());
    DebugMonitor::addItem("boundaryParticles", "%d", boundPos.size());

    DebugMonitor::addItem("maxDensityVariation", "%.1f", maxDV);
    DebugMonitor::addItem("avgDensityVariation", "%.1f", avgDV);
    DebugMonitor::addItem("maxVelocity", "%.3f", maxV);
    DebugMonitor::addItem("maxForce", "%.3f", maxF);

    // Prevent divison by zero
    maxV = std::max(1e-8f, maxV);
    maxF = std::max(1e-8f, maxF);

    // Adjust timestep
    if ((0.19f * std::sqrt(kRadius / maxF) > sTimeStep) &&
        (maxDV < 4.5f * avgDVT) &&
        (avgDV < 0.9f * avgDVT) &&
        (0.39f * kRadius / maxV > sTimeStep)) {
        sTimeStep *= 1.002f;
    }
    if ((0.2f * std::sqrt(kRadius / maxF) < sTimeStep) ||
        (maxDV > 5.5f * avgDVT) ||
        (avgDV >= avgDVT) ||
        (0.4f * kRadius / maxV <= sTimeStep)) {
        sTimeStep *= 0.998f;
    }

    // Detect shock
    if ((maxDV - prevMaxDV > maxDVT) ||
        (maxDV > maxDVT) ||
        (0.45f * kRadius / maxV < sTimeStep)) {
        if (maxDV - prevMaxDV > maxDVT) {
            //PRINT("shock due to [1]");
        }
        if (maxDV > maxDVT) {
            //PRINT("shock due to [2]");
        }
        if (0.45f * kRadius / maxV < sTimeStep) {
            //PRINT("shock due to [3]");
        }
        sTimeStep = std::min(0.2f * std::sqrt(kRadius / maxF), 0.25f * kRadius / maxV);

        // Go back two timesteps
        T = timeBeforeShock;
        fluidPosOld = fluidPosBeforeShock;
        fluidVOld = fluidVBeforeShock;

        DebugMonitor::addItem("shock", "yes");
    } else {
        prevMaxDV = maxDV;
        DebugMonitor::addItem("shock", "no");
    }

    // store time, positions and velocities of two timesteps back
    // "new" buffer holds positions/velocities before current integration step!
    timeBeforeShock = T;
    std::swap(fluidPosNew, fluidPosBeforeShock);
    std::swap(fluidVNew, fluidVBeforeShock);

    T += sTimeStep;

    DebugMonitor::addItem("timeStep", "%.5f", sTimeStep);
    DebugMonitor::addItem("time", "%.5f", T);
}


void SPH::buildScene(const Scene &scene) {
    for (const auto &sceneBox : scene.boxes) {
        switch (sceneBox.type) {
        case Scene::Fluid:
            addFluidParticles(ParticleGenerator::generateVolumeBox(sceneBox.bounds, pRadius));
            break;
        case Scene::Boundary:
            addBoundaryParticles(ParticleGenerator::generateBoundaryBox(sceneBox.bounds, pRadius));
            boundMeshes.emplace_back(Mesh::createBox(sceneBox.bounds));
            break;
        }
    }
    for (const auto &sceneSphere : scene.spheres) {
        switch (sceneSphere.type) {
        case Scene::Fluid:
            addFluidParticles(ParticleGenerator::generateVolumeSphere(sceneSphere.position, sceneSphere.radius, pRadius));
            break;
        case Scene::Boundary:
            addBoundaryParticles(ParticleGenerator::generateBoundarySphere(sceneSphere.position, sceneSphere.radius, pRadius));
            boundMeshes.emplace_back(Mesh::createSphere(sceneSphere.position, sceneSphere.radius));
            break;
        }
    }
    for (const auto &sceneMesh : scene.meshes) {
        Mesh mesh = ObjReader::load(sceneMesh.filename);
        switch (sceneMesh.type) {
        case Scene::Fluid:
            addFluidParticles(ParticleGenerator::generateVolumeMesh(mesh, pRadius));
            break;
        case Scene::Boundary:
            addBoundaryParticles(ParticleGenerator::generateBoundaryMesh(mesh, pRadius));
            boundMeshes.emplace_back(mesh);
            break;
        }
    }

    addBoundaryParticles(ParticleGenerator::generateBoundaryBox(scene.world.bounds, pRadius, true));
}

void SPH::addFluidParticles(const ParticleGenerator::Volume &volume) {
    fluidPosOld.insert(fluidPosOld.end(), volume.positions.begin(), volume.positions.end());
}

void SPH::addBoundaryParticles(const ParticleGenerator::Boundary &boundary) {
    boundPos.insert(boundPos.end(), boundary.positions.begin(), boundary.positions.end());
    boundNor.insert(boundNor.end(), boundary.normals.begin(), boundary.normals.end());
}



} 

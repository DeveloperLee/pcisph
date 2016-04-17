#pragma once

#include "basics/Common.h"

namespace cs224 {

class Mesh;

// Creates an isosurface from a list of particles.
class ParticleMesher {
public:
    struct Parameters {
        float particleRadius;
        float particleDiameter;
        float kernelRadius;
        int kernelSupportParticles;
        float particleMass;
        float restDensity;
        float isoLevel = 0.5f;
    };

    static Mesh createMeshIsotropic(const MatrixXf &positions, const Box3f &bounds, const Vector3i &cells, const Parameters &params);

};

}

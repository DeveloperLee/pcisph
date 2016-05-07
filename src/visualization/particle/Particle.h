#pragma once

#include "utils/Def.h"
#include "utils/Math.h"

#include <vector>

namespace cs224 {

class Mesh;

// Helpers to generate particle for boundaries and volumes.
class ParticleGenerator {
public:
    // boundary particles
    struct Boundary {
        std::vector<Vector3f> positions;
        std::vector<Vector3f> normals;
    };

    static Boundary generateFromBoundaryBox(const Box3f &box, float particleRadius, bool innerNormal = false);
    static Boundary generateFromBoundaryMesh(const Mesh &mesh, float particleRadius, int cells = 100);

    // fluid particles
    struct Volume {
        std::vector<Vector3f> positions;
    };

    static Volume generateFromVolumeBox(const Box3f &box, float particleRadius);
    static Volume generateFromVolumeSphere(const Vector3f &center, float radius, float particleRadius);
    static Volume generateFromVolumeMesh(const Mesh &mesh, float particleRadius);
};

} // namespace cs224

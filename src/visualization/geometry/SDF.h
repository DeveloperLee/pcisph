#pragma once

#include "visualization/mesh/Mesh.h"
#include "visualization/geometry/VoxelGrid.h"

#include "utils/Math.h"

namespace cs224 {

// Signed distance field builder.
class SDF {
public:
    // Generates a signed distance field from a mesh. Absolute distances will be nearly correct
    // for triangle soup, but a closed mesh is needed for accurate signs. Distances for all grid
    // cells within exact_band cells of a triangle should be exact, further away a distance is
    // calculated but it might not be to the closest triangle - just one nearby.
    static void build(const Mesh &mesh, VoxelGrid<float> &sdf, const int exact_band = 1);
};

} // namespace cs224

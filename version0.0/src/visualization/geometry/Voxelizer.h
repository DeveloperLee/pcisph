#pragma once

#include "VoxelGrid.h"

#include "utils/STD.h"
#include "utils/Box.h"

namespace cs224 {

class Mesh;

class Voxelizer {
public:
    struct Result {
        float cellSize;
        Box3f bounds;
        VoxelGrid<bool> grid;
    };

    static void voxelize(const Mesh &mesh, float cellSize, Result &result);
    static void voxelize(const Mesh &mesh, float cellSize, std::vector<Vector3f> &positions);

};

} 

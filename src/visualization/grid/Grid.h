#pragma once

#include "utils/Def.h"
#include <vector>

namespace cs224 {

class Grid {
public:
    void init(const Box3f &bounds, float cs) {
        boundingBox = bounds;
        cellSize = cs;
        inverseCellSize = 1.f / cellSize;

        size = Vector3i(
            nextPowerOfTwo(int(std::floor(boundingBox.extents().x() / cellSize)) + 1),
            nextPowerOfTwo(int(std::floor(boundingBox.extents().y() / cellSize)) + 1),
            nextPowerOfTwo(int(std::floor(boundingBox.extents().z() / cellSize)) + 1)
        );

        offset.resize(size.prod() + 1);
    }

    inline Vector3i index(const Vector3f &pos) const {
        return Vector3i(
            int(std::floor((pos.x() - boundingBox.min.x()) * inverseCellSize)),
            int(std::floor((pos.y() - boundingBox.min.y()) * inverseCellSize)),
            int(std::floor((pos.z() - boundingBox.min.z()) * inverseCellSize))
        );
    }

    inline size_t indexLinear(const Vector3f &pos) const {
        Vector3i i = index(pos);
        return i.z() * (size.x() * size.y()) + i.y() * size.x() + i.x();
    }
    
    template<typename SwapFunc>
    void update(const PCI3Mf &positions, SwapFunc swap) {
        PCI1Mi cellCount(size.prod(), 0);
        PCI1Mi cellIndex(size.prod(), 0);

        size_t count = positions.size();

        PCI1Mi indices(count);

        // Update particle index and count number of particles per cell
        for (size_t i = 0; i < count; ++i) {
            size_t index = indexLinear(positions[i]);
            indices[i] = index;
            ++cellCount[index];
        }

        // Initialize cell indices & offsets
        size_t index = 0;
        for (size_t i = 0; i < cellIndex.size(); ++i) {
            cellIndex[i] = index;
            offset[i] = index;
            index += cellCount[i];
        }
        offset.back() = index;

        // Sort particles by index
        for (int i = 0; i < count; ++i) {
            while (i >= cellIndex[indices[i]] || i < offset[indices[i]]) {
                int j = cellIndex[indices[i]]++;
                std::swap(indices[i], indices[j]);
                swap(i, j);
            }
        }
    }
    
    // Method for querying the surrounding sphere geometry within the same grid.
    template<typename Func>
    void lookup(const Vector3f &pos, float radius, Func func) const {
        Vector3i min = index(pos - Vector3f(radius)).cwiseMax(Vector3i(0));
        Vector3i max = index(pos + Vector3f(radius)).cwiseMin(size - Vector3i(1));
        for (int z = min.z(); z <= max.z(); ++z) {
            for (int y = min.y(); y <= max.y(); ++y) {
                for (int x = min.x(); x <= max.x(); ++x) {
                    int i = z * (size.x() * size.y()) + y * size.x() + x;
                    for (size_t j = offset[i]; j < offset[i + 1]; ++j) {
                        if (!func(j)) { return; }
                    }
                }
            }
        }
    }
    
    template<typename Func>
    inline void query(const float kRadius, const PCI3Mf &positions, const Vector3f &p, Func func) {
        lookup(p, kRadius, [&] (size_t j) {
            Vector3f r = p - positions[j];
            float r2 = r.squaredNorm();
            if (r2 < pow2(kRadius)) {
                func(j, r, r2);
            }
            return true;
        });
    }
    
     // iterate over all neighbours around p, calling func(j, r, r2)
    template<typename Func>
    inline void queryPair(const float kRadius, const PCI3Mf &positionsNew, const Vector3f &p, const Vector3f &pNew, Func func) {
        lookup(p, kRadius, [&] (size_t j) {
            Vector3f r = pNew - positionsNew[j];
            float r2 = r.squaredNorm();
            if (r2 < pow2(kRadius)) {
                func(j, r, r2);
            }
            return true;
        });
    }

    // Method for detecting whether the current particle is isolated.
    // A particle is considered as isolated iff it has no neighbouring 
    // particles within a given range.
    inline bool isAlive(const float kRadius, const PCI3Mf &positions, const Vector3f &p) {
        bool result = false;
        lookup(p, kRadius, [&] (size_t j) {
            if ((p - positions[j]).squaredNorm() < pow2(kRadius)) {
                result = true;
                return false;
            } else {
                return true;
            }
        });
        return result;
    }

private:
        
    Box3f boundingBox;
    float cellSize;
    float inverseCellSize;
    Vector3i size;
    std::vector<size_t> offset;
};

} 

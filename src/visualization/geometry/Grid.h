// This class creates a 3D-Space Grid for querying the spacial information
// by the pcisph kernel.

#pragma once

#include "utils/STD.h"
#include "utils/Vector.h"
#include "utils/Box.h"
#include "utils/Math.h"
#include "utils/Exception.h"

#include <vector>


namespace cs224 {

class Grid {

public:
    
    // Initialize a grid by specifying a bounding box and 
    // a cell size.
	void init(const Box3f &bounds, float cs) {
         
         boundingBox = bounds;
         cellSize = cs;
         inverseCellSize = 1.f / cellSize;
         
	}

	template<typename SwapFunc>
    void update(const PCI3Mf &positions, SwapFunc swap) {

    	// Fill the array with 0.
        PCI1Mi cellCount(size.prod(), 0);
        PCI1Mi cellIndex(size.prod(), 0);

        int count = positions.size();

        PCI1Mi indices(count);

        // Update particle index and count number of particles per cell
        for (int i = 0; i < count; ++i) {
            int index = indexLinear(positions[i]);
            indices[i] = index;
            ++cellCount[index];
        }

        // Initialize cell indices & offsets
        int index = 0;
        for (int i = 0; i < cellIndex.size(); ++i) {
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

    template<typename Func> 
    void lookup(const Vector3f &pos, float radius, Func func) const {

    }
    
    // @Func : This function queries the neighbouring particles and can perform any
    //         kinds of function based on index, differnce vector and the normalized 
    //         squared distance between the center particle and referencing particle.
    // @Params : kRadius : kernel radius
    // @Params : positions : Array of spatial positions (x,y,z).
    // @Params : p : position of center particle
    // @Params : func : Function to be executed based on above three parameters.
    template<typename Func>
    inline void query(const float kRadius, const PCI3Mf &positions, const Vector3f &p, Func func) {
        lookup(p, kRadius, [&] (int j) {
            Vector3f r = p - positions[j];
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
        lookup(p, kRadius, [&] (int j) {
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

	Box3f boundingBox; // The bounding box of this grid.
    float cellSize;
    float inverseCellSize;
    Vector3i size;
    std::vector<int> offset;
};

}
#include "ParticleMesher.h"
#include "Mesh.h"
#include "VoxelGrid.h"
#include "MarchingCubes.h"

#include "basics/Box.h"
#include "basics/Vector.h"
#include "basics/Timer.h"

#include "algorithm/Kernel.h"
#include "utils/ConcurrentUtils.h"

#include <tbb/tbb.h>

#include <Eigen/Geometry>
#include <Eigen/SVD>

#include <atomic>

namespace cs224 {

// Uniform grid for indexing particles.
class Grid {
public:
    template<typename Func>
    Grid(const MatrixXf &positions, const Box3f &bounds, const Vector3i &cells, Func func) :
        _bounds(bounds),
        _cells(cells),
        _cellSize(_bounds.extents().cwiseProduct(Vector3f(_cells.x(), _cells.y(), _cells.z()).cwiseInverse())),
        _invCellSize(_cellSize.cwiseInverse())
    {
        // Compute particle bounds
        std::vector<Box3i> particleBounds(positions.cols());
        ConcurrentUtils::ccLoop(positions.cols(), [this, func, &positions, &particleBounds] (size_t i) {
            Box3f box = func(positions.col(i));
            particleBounds[i] = Box3i(index(box.min).cwiseMax(0), index(box.max).cwiseMin(_cells - Vector3i(1)));
        });

        std::atomic<size_t> *cellCount = new std::atomic<size_t>[_cells.prod()];
        std::atomic<size_t> *cellIndex = new std::atomic<size_t>[_cells.prod()];
        for (size_t i = 0; i < size_t(_cells.prod()); ++i) {
            cellCount[i] = 0;
            cellIndex[i] = 0;
        }
        _cellOffset.resize(_cells.prod() + 1);

        // Count number of particles per cell
         ConcurrentUtils::ccLoop(particleBounds.size(), [this, &particleBounds, &cellCount] (size_t i) {
            iterate(particleBounds[i], [&] (const Vector3i &index) {
                ++cellCount[linearize(index)];
            });
        });

        // Initialize cell indices & offsets
        size_t index = 0;
        for (size_t i = 0; i < size_t(_cells.prod()); ++i) {
            cellIndex[i] = index;
            _cellOffset[i] = index;
            index += cellCount[i];
        }
        _cellOffset.back() = index;

        // Put particles into cells
        _indices.resize(_cellOffset.back());
         ConcurrentUtils::ccLoop(particleBounds.size(), [this, &particleBounds, &cellIndex] (size_t i) {
            iterate(particleBounds[i], [&] (const Vector3i &index) {
                _indices[cellIndex[linearize(index)]++] = i;
            });
        });

        delete [] cellCount;
        delete [] cellIndex;
    }

    template<typename Func>
    void lookup(const Vector3f &pos, Func func) {
        size_t i = linearize(index(pos).cwiseMax(0).cwiseMin(_cells - Vector3i(1)));
        for (size_t j = _cellOffset[i]; j < _cellOffset[i + 1]; ++j) {
            func(_indices[j]);
        }
    }

private:
    inline size_t linearize(const Vector3i &index) {
        return index.z() * (_cells.x() * _cells.y()) + index.y() * _cells.x() + index.x();
    }

    inline Vector3i index(const Vector3f &pos) {
        return Vector3i(
            int(std::floor((pos.x() - _bounds.min.x()) * _invCellSize.x())),
            int(std::floor((pos.y() - _bounds.min.y()) * _invCellSize.y())),
            int(std::floor((pos.z() - _bounds.min.z()) * _invCellSize.z()))
        );
    }

    inline size_t indexLinear(const Vector3f &pos) {
        return linearize(index(pos));
    }

    template<typename Func>
    inline void iterate(const Box3i &range, Func func) {
        for (int z = range.min.z(); z <= range.max.z(); ++z) {
            for (int y = range.min.y(); y <= range.max.y(); ++y) {
                for (int x = range.min.x(); x <= range.max.x(); ++x) {
                    func(Vector3i(x, y, z));
                }
            }
        }
    }


    Box3f _bounds;
    Vector3i _cells;
    Vector3f _cellSize;
    Vector3f _invCellSize;

    std::vector<size_t> _cellOffset;
    std::vector<size_t> _indices;
};

Mesh ParticleMesher::createMeshIsotropic(const MatrixXf &positions, const Box3f &bounds, const Vector3i &cells, const Parameters &params) {
    float kernelRadius = params.kernelRadius;
    float kernelRadius2 = pow2(kernelRadius);

    Timer timer;
    Timer timerTotal;

    Vector3i gridCells(cells / 4);

    timer.reset();
    Grid grid(positions, bounds, gridCells, [kernelRadius] (const Vector3f &p) { return Box3f(p - Vector3f(kernelRadius), p + Vector3f(kernelRadius)); });

    Kernel kernel;
    kernel.init(kernelRadius);
    timer.reset();
    std::vector<float> densities(positions.cols());
     ConcurrentUtils::ccLoop(positions.cols(), [&] (size_t i) {
        float density = 0;
        grid.lookup(positions.col(i), [&] (size_t j) {
            float r2 = (positions.col(i) - positions.col(j)).squaredNorm();
            if (r2 < kernelRadius2) {
                density += kernel.poly6(r2);
            }
        });
        densities[i] = params.particleMass * kernel.poly6C * density;
    });
    timer.reset();
    VoxelGridf voxelGrid(cells + Vector3i(1));
    Vector3f min = bounds.min;
    Vector3f extents = bounds.extents();
     ConcurrentUtils::ccLoop((cells + Vector3i(1)).prod(), [&] (size_t i) {
        int x = i % (cells.x() + 1);
        int y = (i / (cells.x() + 1)) % (cells.y() + 1);
        int z = (i / ((cells.x() + 1) * (cells.y() + 1))) % (cells.z() + 1);
        Vector3f p = Vector3f(float(x) / cells.x(), float(y) / cells.y(), float(z) / cells.z()).cwiseProduct(extents) + min;
        float c = 0.f;
        grid.lookup(p, [&] (size_t j) {
            float r2 = (p - positions.col(j)).squaredNorm();
            if (r2 < kernelRadius2) {
                c += kernel.poly6(r2) / densities[j];
            }
        });
        voxelGrid(x, y, z) = c * params.particleMass * kernel.poly6C;
    });

    timer.reset();
    MarchingCubes<float> mc;
    Mesh mesh = mc.generateIsoSurface(voxelGrid.data(), params.isoLevel, bounds, cells);

    return std::move(mesh);
}
} 

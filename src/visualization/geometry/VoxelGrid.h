#pragma once

#include "utils/Def.h"

#include <vector>

namespace cs224 {

// simple 3D voxel grid.
template<typename T>
class VoxelGrid {
public:
    VoxelGrid() {}

    VoxelGrid(const Vector3i &size) {
        resize(size);
    }

    VoxelGrid(const Vector3i &size, const T &value) {
        resize(size);
        fill(value);
    }

    // resize the voxel grid
    void resize(const Vector3i &size) {
        m_size = size;
        m_x = m_size.x();
        m_xy = m_size.x() * m_size.y();
        _voxels.resize(m_size.prod());
    }

    // resize the voxel grid
    void resize(int x, int y, int z) {
        resize(Vector3i(x, y, z));
    }

    // fills the grid with the given value
    void fill(const T &value) {
        std::fill(_voxels.begin(), _voxels.end(), value);
    }

    // specifies the size of the voxel grid (number of voxels)
    const Vector3i &size() const { return m_size; }

    // specifies the origin in world space
    const Vector3f &origin() const { return _origin; }
    void setOrigin(const Vector3f &origin) { _origin = origin; }

    // specifies the cell size in world space
    float cellSize() const { return m_cellSize; }
    void setCellSize(float cellSize) { m_cellSize = cellSize; }

    // transforms a point in world space to voxel space
    inline Vector3f toVoxelSpace(const Vector3f &vsP) const {
        return (vsP - _origin) * (1.f / m_cellSize);
    }

    // transforms a point in voxel space to world space
    inline Vector3f toWorldSpace(const Vector3f &wsP) const {
        return _origin + wsP * m_cellSize;
    }

    // voxel data accessor
    const T &operator()(const Vector3i &index) const { return _voxels[linearize(index)]; }
          T &operator()(const Vector3i &index)       { return _voxels[linearize(index)]; }

    const T &operator()(int x, int y, int z) const { return _voxels[linearize(Vector3i(x, y, z))]; }
          T &operator()(int x, int y, int z)       { return _voxels[linearize(Vector3i(x, y, z))]; }

    T value(const Vector3i &index) const { return _voxels[linearize(index)]; }
    T value(int x, int y, int z) const { return _voxels[linearize(Vector3i(x, y, z))]; }
    void setValue(const Vector3i &index, const T &value) { _voxels[linearize(index)] = value; }
    void setValue(int x, int y, int z, const T &value) { _voxels[linearize(Vector3i(x, y, z))] = value; }

    // trilinear filtering
    T trilinear(const Vector3f &vsP) const {

        Vector3f uvw(vsP - Vector3f(0.5f));

        int i0 = std::max(0, int(std::floor(uvw.x())));
        int j0 = std::max(0, int(std::floor(uvw.y())));
        int k0 = std::max(0, int(std::floor(uvw.z())));
        int i1 = std::min(int(m_size.x() - 1), i0 + 1);
        int j1 = std::min(int(m_size.y() - 1), j0 + 1);
        int k1 = std::min(int(m_size.z() - 1), k0 + 1);
        uvw -= Vector3f(float(i0), float(j0), float(k0));

        T temp1, temp2;

        temp1 = (*this)(i0,j0,k0) + T(((*this)(i0,j0,k1) - (*this)(i0,j0,k0)) * uvw.z());
        temp2 = (*this)(i0,j1,k0) + T(((*this)(i0,j1,k1) - (*this)(i0,j1,k0)) * uvw.z());
        T result1 = temp1 + T((temp2-temp1) * uvw.y());

        temp1 = (*this)(i1,j0,k0) + T(((*this)(i1,j0,k1) - (*this)(i1,j0,k0)) * uvw.z());
        temp2 = (*this)(i1,j1,k0) + T(((*this)(i1,j1,k1) - (*this)(i1,j1,k0)) * uvw.z());
        T result2 = temp1 + T((temp2 - temp1) * uvw.y());

        return result1 + T(uvw.x() * (result2 - result1));
    }

    // returns the gradient at the given position using central differences
    Vector<T, 3> gradient(const Vector3f &vsP, float eps = 1e-5f) const {
        return Vector<T, 3>(
            trilinear(vsP + Vector3f(eps, 0.f, 0.f)) - trilinear(vsP - Vector3f(eps, 0.f, 0.f)),
            trilinear(vsP + Vector3f(0.f, eps, 0.f)) - trilinear(vsP - Vector3f(0.f, eps, 0.f)),
            trilinear(vsP + Vector3f(0.f, 0.f, eps)) - trilinear(vsP - Vector3f(0.f, 0.f, eps))
        ) * (0.5f / eps);
    }

    // raw data
    const T *data() const { return _voxels.data(); }

private:
    inline size_t linearize(const Vector3i &index) const {
        return index.z() * m_xy + index.y() * m_x + index.x();
    }

    Vector3i m_size;
    int m_x;
    int m_xy;
    Vector3f _origin;
    float m_cellSize = 1.f;
    std::vector<T> _voxels;
};

typedef VoxelGrid<float> VoxelGridf;
typedef VoxelGrid<bool> VoxelGridb;

} // namespace cs224

#pragma once

#include "STD.h"

namespace cs224 {

template <typename _VectorType> struct TBox {

    int Dimension = 3;
    typedef _VectorType                             VectorType;
    typedef typename VectorType::Scalar             Scalar;

    VectorType min;
    VectorType max;

    TBox() {
        reset();
    }

    /// Create a bounding box from two positions
    TBox(const VectorType &min, const VectorType &max)
        : min(min), max(max) {
    }

    /// Return the center point
    VectorType center() const {
        return (max + min) * (Scalar) 0.5f;
    }

    /// Return the dimension index with the largest associated side length
    int majorAxis() const {
        VectorType d = max - min;
        int largest = 0;
        for (int i = 1; i<Dimension; ++i)
            if (d[i] > d[largest])
                largest = i;
        return largest;
    }

    /**
     * \brief Calculate the bounding box extents
     * \return max-min
     */
    VectorType extents() const {
        return max - min;
    }

    /**
     * \brief Mark the bounding box as invalid.
     *
     * This operation sets the components of the minimum
     * and maximum position to \f$\infty\f$ and \f$-\infty\f$,
     * respectively.
     */
    void reset() {
        min.setConstant(std::numeric_limits<Scalar>::max());
        max.setConstant(std::numeric_limits<Scalar>::lowest());
    }

    /// Expand the bounding box to contain another point
    void expandBy(const VectorType &p) {
        min = min.cwiseMin(p);
        max = max.cwiseMax(p);
    }

    /// Expand the bounding box to contain another bounding box
    void expandBy(const TBox &bbox) {
        min = min.cwiseMin(bbox.min);
        max = max.cwiseMax(bbox.max);
    }

    /// Return box expanded by v
    TBox expanded(const VectorType &v) const {
        return TBox(min - v, max + v);
    }

    /// Expands the box in each dimension by s
    TBox expanded(float s) const {
        return expanded(VectorType(s));
    }



};

}

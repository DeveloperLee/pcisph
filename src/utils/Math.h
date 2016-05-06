#pragma once 

#include <cmath>
#include <Eigen/Core>

namespace cs224 {


// A template vector class that can accept scalar type and 
// arbitrary dimension. In Eigen, all matrices and vectors
// are objects of Matrix template class. Vectors are just
// a special case of matrices, with either 1 row or 1 column.
template <typename _Scalar, int _Dimension> 
struct Vector : public Eigen::Matrix<_Scalar, _Dimension, 1> {
public:

    enum {
        Dimension = _Dimension
    };
    
    typedef _Scalar                             Scalar;
    typedef Eigen::Matrix<Scalar, Dimension, 1> Base;

    /// Create a new vector with constant component vlaues
    Vector(Scalar value = (Scalar) 0) { Base::setConstant(value); }

    /// Create a new 2D vector (type error if \c Dimension != 2)
    Vector(Scalar x, Scalar y) : Base(x, y) { }

    /// Create a new 3D vector (type error if \c Dimension != 3)
    Vector(Scalar x, Scalar y, Scalar z) : Base(x, y, z) { }

    /// Construct a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Vector(const Eigen::MatrixBase<Derived>& p)
        : Base(p) { }

    /// Assign a vector from MatrixBase (needed to play nice with Eigen)
    template <typename Derived> Vector &operator=(const Eigen::MatrixBase<Derived>& p) {
        this->Base::operator=(p);
        return *this;
    }
   
};

template <typename _VectorType> struct BoundingBox {

    int Dimension = 3;
    typedef _VectorType                             VectorType;
    typedef typename VectorType::Scalar             Scalar;
    
    VectorType min; 
    VectorType max;  

    BoundingBox() {
        reset();
    }

    /// Create a bounding box from two positions
    BoundingBox(const VectorType &min, const VectorType &max)
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

    VectorType extents() const {
        return max - min;
    }

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
    void expandBy(const BoundingBox &bbox) {
        min = min.cwiseMin(bbox.min);
        max = max.cwiseMax(bbox.max);
    }

    /// Return box expanded by v
    BoundingBox expanded(const VectorType &v) const {
        return BoundingBox(min - v, max + v);
    }

    /// Expands the box in each dimension by s
    BoundingBox expanded(float s) const {
        return expanded(VectorType(s));
    }

};

// Generate a random float number between a and b.
// This function generate random float number between
// (0,1) by default.
static inline float randomFloat(float a = 0.f, float b = 1.f) {
    float ran = (float) rand() / (float) RAND_MAX;
    float diff = std::abs(b - a);
    return ran * diff + std::min(a,b);
}

template<typename T>
static constexpr T pow2(T x) { return x*x; }

template<typename T>
static constexpr T pow3(T x) { return x*x*x; }

template<typename T>
static constexpr T pow6(T x) { return x*x*x*x*x*x; }

template<typename T>
static constexpr T pow9(T x) { return x*x*x*x*x*x*x*x*x; }

template<typename T>
static inline T clamp(T x, T lo, T hi)  {
    return std::max(lo, std::min(hi, x));
}

template<typename S, typename T>
static inline T lerp(S t, const T &a, const T &b) {
    return (S(1) - t) * a + t * b;
}

static inline float unitToRange(float x, float lo, float hi) {
    return lo + clamp(x, 0.f, 1.f) * (hi - lo);
}

static inline float rangeToUnit(float x, float lo, float hi) {
    return clamp((x - lo) / (hi - lo), 0.f, 1.f);
}

static inline uint32_t nextPowerOfTwo(uint32_t x) {
    x--;
    x |= x >> 1;
    x |= x >> 2;
    x |= x >> 4;
    x |= x >> 8;
    x |= x >> 16;
    x++;
    return x;
}
}
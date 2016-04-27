// All define things go here.
#pragma once

#if defined(__APPLE__)
    #define GLFW_INCLUDE_GLCOREARB
#elif defined(_WIN32)
    #include <glad/glad.h>
#else
    #define GL_GLEXT_PROTOTYPES
#endif

#include <Eigen/Core>
#include <GLFW/glfw3.h>
#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <memory>
#include <algorithm>

#include "Math.h"

namespace cs224 {

#define PI 3.1415926536

template <typename T> struct CPP2GL_types;
template <> struct CPP2GL_types <uint32_t> { enum { type = GL_UNSIGNED_INT, integral = 1 }; };
template <> struct CPP2GL_types <int32_t> { enum { type = GL_INT, integral = 1 }; };
template <> struct CPP2GL_types <uint16_t> { enum { type = GL_UNSIGNED_SHORT, integral = 1 }; };
template <> struct CPP2GL_types <int16_t> { enum { type = GL_SHORT, integral = 1 }; };
template <> struct CPP2GL_types <uint8_t> { enum { type = GL_UNSIGNED_BYTE, integral = 1 }; };
template <> struct CPP2GL_types <int8_t> { enum { type = GL_BYTE, integral = 1 }; };
template <> struct CPP2GL_types <double> { enum { type = GL_DOUBLE, integral = 0 }; };
template <> struct CPP2GL_types <float> { enum { type = GL_FLOAT, integral = 0 }; };

typedef Vector<float, 2>       Vector2f;
typedef Vector<float, 3>       Vector3f;
typedef Vector<int, 2>         Vector2i;
typedef Vector<int, 3>         Vector3i;
typedef Vector<uint32_t, 3>    Vector3u;
typedef BoundingBox<Vector3f>          Box3f;
typedef BoundingBox<Vector3i>          Box3i;

// Define some useful data structures for storing
// fluid/boundary particle properties and status.
typedef std::vector<Vector3f>  PCI3Mf;
typedef std::vector<float>     PCI1Mf;
typedef std::vector<int>       PCI1Mi;

typedef Eigen::Matrix<float,    Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;


}
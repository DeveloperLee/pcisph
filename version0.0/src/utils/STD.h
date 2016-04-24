#pragma once

#include <iostream>
#include <vector>
#include <string>
#include <cmath>
#include <Eigen/Core>

#include "Vector.h"

//============================================================
// Constants
//============================================================

#define PI 3.1415926536f

//============================================================
// Types define
//============================================================

namespace cs224 {

//template <typename Scalar, int Dimension>  struct TVector;
template <typename Vector>      struct TBox;

typedef TVector<float, 1>       Vector1f;
typedef TVector<float, 2>       Vector2f;
typedef TVector<float, 3>       Vector3f;
typedef TVector<int, 1>         Vector1i;
typedef TVector<int, 2>         Vector2i;
typedef TVector<int, 3>         Vector3i;
typedef TVector<uint32_t, 1>    Vector1u;
typedef TVector<uint32_t, 2>    Vector2u;
typedef TVector<uint32_t, 3>    Vector3u;
typedef TBox<Vector3f>          Box3f;
typedef TBox<Vector3i>          Box3i;

// Define some useful data structures for storing
// fluid/boundary particle properties and status.
typedef std::vector<Vector3f>  PCI3Mf;
typedef std::vector<float>     PCI1Mf;
typedef std::vector<int>       PCI1Mi;

typedef Eigen::Matrix<float,    Eigen::Dynamic, Eigen::Dynamic> MatrixXf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> MatrixXu;

// A helper function transform std::vector<Vector3f> to Eigen::MatrixXf
Eigen::MatrixXf toMatrix(const std::vector<Vector3f> &data);

} // name space cs224



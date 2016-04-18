#ifndef __DX_PCISPH_FLUID_SIMULATOR_STD__
#define __DX_PCISPH_FLUID_SIMULATOR_STD__

#include <iostream>
#include <vector>
#include <string>

#include <utils/vector.h>


//============================================================
// Types define
//============================================================

template <typename Scalar, int Dimension>  struct TVector;
template <typename Vector>                 struct TBox;

typedef TVector<float, 1>       Vector1f;
typedef TVector<float, 2>       Vector2f;
typedef TVector<float, 3>       Vector3f;
typedef TVector<float, 4>       Vector4f;
typedef TVector<double, 1>      Vector1d;
typedef TVector<double, 2>      Vector2d;
typedef TVector<double, 3>      Vector3d;
typedef TVector<double, 4>      Vector4d;
typedef TVector<int, 1>         Vector1i;
typedef TVector<int, 2>         Vector2i;
typedef TVector<int, 3>         Vector3i;
typedef TVector<int, 4>         Vector4i;
typedef TVector<uint32_t, 1>    Vector1u;
typedef TVector<uint32_t, 2>    Vector2u;
typedef TVector<uint32_t, 3>    Vector3u;
typedef TVector<uint32_t, 4>    Vector4u;
typedef TBox<Vector1f>          Box1f;
typedef TBox<Vector2f>          Box2f;
typedef TBox<Vector3f>          Box3f;
typedef TBox<Vector4f>          Box4f;
typedef TBox<Vector1d>          Box1d;
typedef TBox<Vector2d>          Box2d;
typedef TBox<Vector3d>          Box3d;
typedef TBox<Vector4d>          Box4d;
typedef TBox<Vector1i>          Box1i;
typedef TBox<Vector2i>          Box2i;
typedef TBox<Vector3i>          Box3i;
typedef TBox<Vector4i>          Box4i;

typedef Eigen::Matrix<float,    Eigen::Dynamic, Eigen::Dynamic> Matrixf;
typedef Eigen::Matrix<uint32_t, Eigen::Dynamic, Eigen::Dynamic> Matrixu;

#endif



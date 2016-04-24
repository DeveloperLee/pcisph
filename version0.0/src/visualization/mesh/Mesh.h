#pragma once

#include <utils/STD.h>

#include <istream>
#include <ostream>

namespace cs224 {

class Mesh {
public:
    Mesh();

    const MatrixXf &vertices() const { return m_vertices; }
          MatrixXf &vertices()       { return m_vertices; }

    const MatrixXf &normals() const { return m_normals; }
          MatrixXf &normals()       { return m_normals; }

    const MatrixXf &uvs() const { return m_uvs; }
          MatrixXf &uvs()       { return m_uvs; }

    const MatrixXu &triangles() const { return m_triangles; }
          MatrixXu &triangles()       { return m_triangles; }

    Box3f bound() const;
    static Mesh createBox(const Box3f &box);
    static Mesh createSphere(const Vector3f &position, float radius, int segments = 32);

private:
    void addVertex(const Vector3f &p, const Vector3f &n, const Vector2f &uv);
    void addTriangle(int a, int b, int c);

    MatrixXf m_vertices;
    MatrixXf m_normals;
    MatrixXf m_uvs;
    MatrixXu m_triangles;
};

} // namespace cs224









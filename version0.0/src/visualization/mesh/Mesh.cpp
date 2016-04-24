
#include "Mesh.h"

#include "utils/Vector.h"
#include "utils/Box.h"

namespace cs224 {

Mesh::Mesh() {
	m_vertices.resize(3, 0);
	m_normals.resize(3, 0);
	m_uvs.resize(3, 0);
	m_triangles.resize(3, 0);
}

Box3f Mesh::bound() const {

	Box3f bound;
	for (int i = 0; i < m_vertices.cols(); ++ i) {
		bound.expandBy(m_vertices.col(i));
	}

	return bound;
}

Mesh Mesh::createBox(const Box3f &box) {

    Vector3f center = box.center();
    Vector3f extents = box.extents() * 0.5;

    Mesh mesh;
    int c = 0;

    for (int i = 0; i < 3; ++ i) {
        for (int j = -1; j <= 1; j += 2) {
            Vector3f p(center), n(0.f), u(0.f), v(0.f);
            p[i] += extents[i] * j;
            n[i] = j;
            u[(i + 1) % 3] = extents[(i + 1) % 3];
            v[(i + 2) % 3] = extents[(i + 2) % 3];
            mesh.addVertex(p - u - v, n, Vector2f(0.f, 0.f));
            mesh.addVertex(p + u - v, n, Vector2f(1.f, 0.f));
            mesh.addVertex(p - u + v, n, Vector2f(0.f, 1.f));
            mesh.addVertex(p + u + v, n, Vector2f(1.f, 1.f));
            mesh.addTriangle(c,     c + 1, c + 2);
            mesh.addTriangle(c + 1, c + 2, c + 3);
            c += 4;
        }
    }

    return mesh;
}

Mesh Mesh::createSphere(const Vector3f &position, float radius, int segments) {

	Mesh mesh;
	float fseg = static_cast<float>(segments);

	for (int itheta = 0; itheta <= segments / 2; ++ itheta) {
		for (int iphi = 0; iphi <= segments; ++ iphi) {
			float u = static_cast<float>(iphi) / fseg;
			float v = static_cast<float>(itheta) / (fseg / 2);
			float phi = u * 2.f * PI;
			float theta = v * PI;
			const Vector3f p(
				std::cos(phi) * std::sin(theta),
				std::sin(phi) * std::sin(theta),
				std::cos(theta)
			);
			mesh.addVertex(position + p * radius, p, Vector2f(u, v));
			if (itheta < segments / 2 && iphi < segments) {
				mesh.addTriangle(itheta * (segments + 1) + iphi, itheta * (segments + 1) + iphi + 1, (itheta + 1) * (segments + 1) + iphi);
				mesh.addTriangle(itheta * (segments + 1) + iphi + 1, (itheta + 1) * (segments + 1) + iphi, (itheta + 1) * (segments + 1) + iphi + 1);
			}
		}
	}

	return mesh;
}

void Mesh::addVertex(const Vector3f &p, const Vector3f &n, const Vector2f &uv) {

	m_vertices.conservativeResize(Eigen::NoChange, m_vertices.cols() + 1);
	m_vertices.col(m_vertices.cols() - 1) = p;
	m_normals.conservativeResize(Eigen::NoChange, m_normals.cols() + 1);
	m_normals.col(m_normals.cols() - 1) = n;
	m_uvs.conservativeResize(Eigen::NoChange, m_uvs.cols() + 1);
	m_uvs.col(m_uvs.cols() - 1) = uv;
}

void Mesh::addTriangle(int a, int b, int c) {

	m_triangles.conservativeResize(Eigen::NoChange, m_triangles.cols() + 1);
	m_triangles.col(m_triangles.cols() - 1) = Vector3u(a, b, c);
}

} // namespace cs224


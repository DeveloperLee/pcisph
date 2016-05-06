#include "ObjLoader.h"
#include "visualization/mesh/Mesh.h"

#include <fstream>
#include <sstream>
#include <unordered_map>

namespace cs224 {

Mesh ObjLoader::load(const std::string &path) {

	Mesh mesh;
	std::vector<Vector3f> positions;
	std::vector<Vector3f> normals;
	std::vector<Vector2f> uvs;
	std::vector<uint32_t> indices;
	std::vector<Vertex> vertices;
	std::unordered_map<Vertex, uint32_t, VertexHash> vertexMap;

	std::ifstream objFile(path);
	if (objFile.fail()) {
        std::cout << "Failed to load obj file!"<<std::endl;
    }

	std::string line;

	// parse the obj file
	while (std::getline(objFile, line)) {
		std::istringstream ss(line);
		std::string prefix;
		ss >> prefix;

		if (prefix == "v") {
			Vector3f p;
			ss >> p.x() >> p.y() >> p.z();
			positions.emplace_back(p);
		} else if (prefix == "vt") {
			Vector2f uv;
			ss >> uv.x() >> uv.y();
			uvs.emplace_back(uv);
		} else if (prefix == "vn") {
			Vector3f n;
			ss >> n.x() >> n.y() >> n.z();
			normals.emplace_back(n.normalized());
		} else if (prefix == "f") {
			std::string v1, v2, v3, v4;
			ss >> v1 >> v2 >> v3 >> v4;
			Vertex vs[6];
			int numVs = 3;

			vs[0] = Vertex(v1);
			vs[1] = Vertex(v2);
			vs[2] = Vertex(v3);

			// if is a quad, split into 2 triangles
			if (!v4.empty()) {
				vs[3] = Vertex(v4);
				vs[4] = vs[0];
				vs[5] = vs[2];
				numVs = 6;
			}

			// convert to an indexed vertex list
			for (int i = 0; i < numVs; ++ i) {
				const Vertex &v = vs[i];
				auto it = vertexMap.find(v);
				if (it == vertexMap.end()) {
					vertexMap[v] = static_cast<uint32_t>(vertices.size());
					indices.emplace_back(static_cast<uint32_t>(vertices.size()));
					vertices.emplace_back(v);
				} else {
					indices.emplace_back(it->second);
				}
			}
		}
	}

	// copy everything into mesh
	mesh.triangles().resize(3, indices.size() / 3);
	std::memcpy(mesh.triangles().data(), indices.data(), indices.size() * sizeof(uint32_t));

	mesh.vertices().resize(3, vertices.size());
	for (size_t i = 0; i < vertices.size(); ++ i) {
		mesh.vertices().col(i) = positions[vertices[i].p - 1];
	}

	if (!normals.empty()) {
		mesh.normals().resize(3, vertices.size());
		for (size_t i = 0; i < vertices.size(); ++ i) {
			mesh.normals().col(i) = normals[vertices[i].n - 1];
		}
	}

	if (!uvs.empty()) {
		mesh.uvs().resize(2, vertices.size());
		for (size_t i = 0; i < vertices.size(); ++ i) {
			mesh.uvs().col(i) = uvs[vertices[i].uv - 1];
		}
	}

	return std::move(mesh);
}

} // namespace cs224















#pragma once

#include <utils/StringUtils.h>
#include <utils/STD.h>

#include <string>
#include <vector>

namespace cs224 {

class Mesh;

class ObjLoader {
public:
	static Mesh load(const std::string &filename);

private:
	struct Vertex {
		uint32_t p = static_cast<uint32_t>(-1);
		uint32_t n = static_cast<uint32_t>(-1);
		uint32_t uv = static_cast<uint32_t>(-1);

		inline Vertex() {}
		inline Vertex(const std::string &str) {
			std::vector<std::string> tokens = StringUtils::tokenize(str, "/", true);
			if (tokens.size() < 1 || tokens.size() > 3) {
                std::cout<<"Invalid data!"<<std::endl;
            }
			p = StringUtils::toUInt(tokens[0]);
			if (tokens.size() >= 2 && !tokens[1].empty()) uv = StringUtils::toUInt(tokens[1]);
			if (tokens.size() >= 3 && !tokens[2].empty()) n = StringUtils::toUInt(tokens[2]);
		}

		inline bool operator==(const Vertex &v) const {
			return v.p == p && v.n == n && v.uv == uv;
		}
	};

	// Hash function for Vertex
	struct VertexHash : std::unary_function<Vertex, size_t> {
		std::size_t operator()(const Vertex &v) const {
			size_t hash = std::hash<uint32_t>()(v.p);
			hash = hash * 37 + std::hash<uint32_t>()(v.uv);
			hash = hash * 37 + std::hash<uint32_t>()(v.n);
			return hash;
		}
	};
};


} // namespace cs224
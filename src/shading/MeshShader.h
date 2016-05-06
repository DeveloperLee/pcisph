#pragma once

#include "utils/Def.h"
#include "PCIShader.h"
#include "visualization/mesh/Mesh.h"

namespace cs224 {

struct MeshShader {
   
   PCIShader shader;
   int tris = 0;
    
   MeshShader() {
	   const std::string vert_str = "#version 330\n"
	                                 "uniform mat4 mvp;\n"
	                                 "in vec3 position;\n"
	                                 "in vec3 normal;\n"
	                                 "out vec3 vNormal;\n"
	                                 "void main() {\n"
	                                 "    gl_Position = mvp * vec4(position, 1.0);\n"
	                                 "    vNormal = normal;\n"
	                                 "}";

	    const std::string frag_str = "#version 330\n"
	                              	 "uniform vec4 color;\n"
	                              	 "in vec3 vNormal;\n"
	                               	 "out vec4 out_color;\n"
	                               	 "void main() {\n"
	                               	 "    float s = mix(0.1, 0.9, abs(dot(vNormal, vec3(0.0, 1.0, 0.0))));\n"
	                                 "    out_color = vec4(s * color.rgb, color.a);\n"
	                                 "}";
        shader.buildShader("MeshShader", vert_str, frag_str);
	}
    
    // Set a mesh for the current shader to render
	MeshShader(const Mesh &mesh) : MeshShader() {
        setMesh(mesh);
    }

    void setMesh(const Mesh &mesh) {
        shader.bind();
        shader.updateBindings("position", mesh.vertices());
        shader.updateBindings("normal", mesh.normals());
        shader.updateBindings("indices", mesh.triangles());
        tris = mesh.triangles().cols();
    }

    void draw(const Eigen::Matrix4f &mvp, const Eigen::Vector4f &color) {
        shader.bind();
        shader.setUniform("mvp", mvp);
        shader.setUniform("color", color);
        glEnable(GL_DEPTH_TEST);
        shader.draw(GL_TRIANGLES, 0, tris, 0);
    }

};
}
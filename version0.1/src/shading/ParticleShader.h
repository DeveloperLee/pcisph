#pragma once

#include <utils/Def.h>

#include "PCIShader.h"
#include "FBO.h"
#include "utils/ResourceLoader.h"

namespace cs224 {

struct ParticleShader {

    PCIShader shader;

    ParticleShader()
    {

        // shader programs
        const std::string vert_str = "#version 430\n"
                                 "uniform mat4 mv;\n"
                                 "in vec3 position;\n"
                                 "out vec4 vPosition;\n"
                                 "void main() {\n"
                                 "    vPosition = mv * vec4(position, 1.0);\n"
                                 "}";

        const std::string frag_str = "#version 430\n"
                                   "uniform vec4 color;\n"
                                   "in vec2 gPosition;\n"
                                   "layout (location = 0) out vec4 out_color;\n"
                                   "void main() {\n"
                                   "    vec3 n = vec3(2.0 * gPosition, 0.0);\n"
                                   "    float r2 = dot(n.xy, n.xy);\n"
                                   "    if (r2 > 1.0) discard;\n"
                                   "    n.z = 1.0 - sqrt(r2);\n"
                                   "    vec3 L = normalize(vec3(1.0));\n"
                                   "    float d = max(0.0, dot(L, n));\n"
                                   "    out_color = vec4(d * color.xyz, color.w);\n"
                                   "    //out_color = vec4(d*vec3(1,1,1), color.w);\n"
                                   "}";

        const std::string geo_str = "#version 430\n"
                                   "layout (points) in;\n"
                                   "layout (triangle_strip) out;\n"
                                   "layout (max_vertices = 4) out;\n"
                                   "uniform mat4 proj;\n"
                                   "uniform float particleRadius;\n"
                                   "in vec4 vPosition[];\n"
                                   "out vec2 gPosition;\n"
                                   "void main() {\n"
                                   "    vec4 p = vPosition[0];\n"
                                   "    gPosition = vec2(-1.0, -1.0);\n"
                                   "    gl_Position = proj * vec4(p.xy + gPosition * particleRadius, p.zw);\n"
                                   "    EmitVertex();\n"
                                   "    gPosition = vec2(-1.0, 1.0);\n"
                                   "    gl_Position = proj * vec4(p.xy + gPosition * particleRadius, p.zw);\n"
                                   "    EmitVertex();\n"
                                   "    gPosition = vec2(1.0, -1.0);\n"
                                   "    gl_Position = proj * vec4(p.xy + gPosition * particleRadius, p.zw);\n"
                                   "    EmitVertex();\n"
                                   "    gPosition = vec2(1.0, 1.0);\n"
                                   "    gl_Position = proj * vec4(p.xy + gPosition * particleRadius, p.zw);\n"
                                   "    EmitVertex();\n"
                                   "    EndPrimitive();\n"
                                   "}";
        shader.buildShader("ParticleShader", vert_str, frag_str, geo_str);




       
    }
    void draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, const Eigen::Vector4f &color, float particleRadius = 0.03f) {
        shader.bind();
        shader.updateBindings("position", positions);
        shader.setUniform("mv", mv);
        shader.setUniform("proj", proj);
        shader.setUniform("particleRadius", particleRadius);
        shader.setUniform("color", color);
        //glUniform4f(glGetUniformLocation(shader.m_program,"color"),color.x(),color.y(),color.z(),1);
        glEnable(GL_DEPTH_TEST);
        shader.draw(GL_POINTS, 0, positions.cols(), 1);
    }
};

} // namespace cs224

#pragma once

#include "utils/Def.h"
#include "PCIShader.h"

namespace cs224 {

struct DomainShader {

    PCIShader shader;

    const std::string vertex_str = "#version 330\n"
                                   "uniform mat4 mvp;\n"
                                   "in vec3 position;\n"
                                   "void main() {\n"
                                   "gl_Position = mvp * vec4(position, 1.0);\n"
                                   "}";
    const std::string frag_str   = "#version 330\n"
                                   "out vec4 out_color;\n"
                                   "void main() {\n"
                                   "out_color = vec4(vec3(0.0), 0.4);\n"
                                   "}";

    DomainShader() {
       shader.buildShader("DomainShader", vertex_str, frag_str);
    }

    void draw(const Eigen::Matrix4f &mvp, const Box3f &box) {

        MatrixXf positions(3, 24);
        positions.col(0)  = Vector3f(box.min.x(), box.min.y(), box.min.z());
        positions.col(1)  = Vector3f(box.max.x(), box.min.y(), box.min.z());
        positions.col(2)  = Vector3f(box.min.x(), box.max.y(), box.min.z());
        positions.col(3)  = Vector3f(box.max.x(), box.max.y(), box.min.z());
        positions.col(4)  = Vector3f(box.min.x(), box.min.y(), box.min.z());
        positions.col(5)  = Vector3f(box.min.x(), box.max.y(), box.min.z());
        positions.col(6)  = Vector3f(box.max.x(), box.min.y(), box.min.z());
        positions.col(7)  = Vector3f(box.max.x(), box.max.y(), box.min.z());

        positions.col(8)  = Vector3f(box.min.x(), box.min.y(), box.max.z());
        positions.col(9)  = Vector3f(box.max.x(), box.min.y(), box.max.z());
        positions.col(10) = Vector3f(box.min.x(), box.max.y(), box.max.z());
        positions.col(11) = Vector3f(box.max.x(), box.max.y(), box.max.z());
        positions.col(12) = Vector3f(box.min.x(), box.min.y(), box.max.z());
        positions.col(13) = Vector3f(box.min.x(), box.max.y(), box.max.z());
        positions.col(14) = Vector3f(box.max.x(), box.min.y(), box.max.z());
        positions.col(15) = Vector3f(box.max.x(), box.max.y(), box.max.z());

        positions.col(16) = Vector3f(box.min.x(), box.min.y(), box.min.z());
        positions.col(17) = Vector3f(box.min.x(), box.min.y(), box.max.z());
        positions.col(18) = Vector3f(box.max.x(), box.min.y(), box.min.z());
        positions.col(19) = Vector3f(box.max.x(), box.min.y(), box.max.z());
        positions.col(20) = Vector3f(box.min.x(), box.max.y(), box.min.z());
        positions.col(21) = Vector3f(box.min.x(), box.max.y(), box.max.z());
        positions.col(22) = Vector3f(box.max.x(), box.max.y(), box.min.z());
        positions.col(23) = Vector3f(box.max.x(), box.max.y(), box.max.z());

        shader.bind();
        shader.updateBindings("position", positions);
        shader.setUniform("mvp", mvp);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        shader.draw(GL_LINES, 0, 24, 1);
        glDisable(GL_BLEND);
    }

};

} // namespace cs224

#pragma once

#include <utils/STD.h>
#include <utils/Vector.h>
#include <utils/Box.h>
#include <GL/glew.h>
#include <Eigen/Core>

#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

namespace cs224 {

class DomainShader {
public:
    DomainShader() {

        // init shader program id
        m_program = glCreateProgram();

        // init shader id
        m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // init vao, vbo
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        // shader programs
        const char *vertexCode = "#version 330\n"
                                 "uniform mat4 mvp;\n"
                                 "in vec3 position;\n"
                                 "void main() {\n"
                                 "    gl_Position = mvp * vec4(position, 1.0);\n"
                                 "}";

        const char *fragmentCode = "#version 330\n"
                                   "out vec4 out_color;\n"
                                   "void main() {\n"
                                   "    out_color = vec4(vec3(1.0), 0.4);\n"
                                   "}";

        // add source
        glShaderSource(m_vertexShader, 1, &vertexCode, NULL);
        glShaderSource(m_fragmentShader, 1, &fragmentCode, NULL);

        // complie source
        glCompileShader(m_vertexShader);
        glCompileShader(m_fragmentShader);

        // attach shader
        glAttachShader(m_program, m_vertexShader);
        glAttachShader(m_program, m_fragmentShader);

        // link shader program
        glLinkProgram(m_program);

        // print the info log
        GLint ret = GL_FALSE;
        int infoLen;
        glGetProgramiv(m_program, GL_LINK_STATUS, &ret);
        glGetProgramiv(m_program, GL_INFO_LOG_LENGTH, &infoLen);
        if (infoLen > 0) {
            std::vector<char> info(std::max(infoLen, int(1)));
            glGetProgramInfoLog(m_program, infoLen, NULL, &info[0]);
            std::string str(info.begin(), info.end());
            std::cout << str << std::endl;
        }

        // delete shader
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
    }

    ~DomainShader() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void draw(const Eigen::Matrix4f &mvp, const Box3f &box) {

//        std::cout << "box3f" << std::endl;
//        std::cout << box.extents() << std::endl;

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

        // bind vao
        glBindVertexArray(m_vao);

        // get attribute id
        GLint positionId = glGetAttribLocation(m_program, "position");
        glEnableVertexAttribArray(positionId);

        // store vertex data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(positionId, positions.rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);

        // bind shader program
        glUseProgram(m_program);

        // set mvp uniform
        glUniformMatrix4fv(glGetUniformLocation(m_program, "mvp"), 1, GL_FALSE, mvp.data());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // draw
        glBindVertexArray(m_vao);
        glDrawArrays(GL_LINES, 0, 24);
        glBindVertexArray(0);

        glDisable(GL_BLEND);

        // unbind shader program
        glUseProgram(0);
    }

private:
    GLuint m_program;
    GLuint m_vertexShader, m_fragmentShader;
    GLuint m_vao, m_vbo;
};

} // namespace cs224

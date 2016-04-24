#pragma once

#include <utils/STD.h>
#include <utils/Vector.h>
#include <utils/Box.h>
#include <Eigen/Core>
#include <GL/glew.h>

#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif

namespace cs224 {

class ParticleNormalShader {
public:
    ParticleNormalShader() {

        // init shader program id
        m_program = glCreateProgram();

        // init shader id
        m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        // init vao, vbo
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo1);
        glGenBuffers(1, &m_vbo2);

        // shader programs
        const char *vertexCode = "#version 330\n"
                                 "uniform mat4 mvp;\n"
                                 "uniform float normalLength;\n"
                                 "in vec3 position;\n"
                                 "in vec3 normal;\n"
                                 "out vec4 vPositionA;\n"
                                 "out vec4 vPositionB;\n"
                                 "void main() {\n"
                                 "    vPositionA = mvp * vec4(position, 1.0);\n"
                                 "    vPositionB = mvp * vec4(position + normalLength * normal, 1.0);\n"
                                 "}";

        const char *fragmentCode = "#version 330\n"
                                   "uniform vec4 color;\n"
                                   "out vec4 out_color;\n"
                                   "void main() {\n"
                                   "    out_color = color;\n"
                                   "}";

        const char *geometryCode = "#version 330\n"
                                   "layout (points) in;\n"
                                   "layout (line_strip) out;\n"
                                   "layout (max_vertices = 2) out;\n"
                                   "in vec4 vPositionA[];\n"
                                   "in vec4 vPositionB[];\n"
                                   "void main() {\n"
                                   "    gl_Position = vPositionA[0];\n"
                                   "    EmitVertex();\n"
                                   "    gl_Position = vPositionB[0];\n"
                                   "    EmitVertex();\n"
                                   "    EndPrimitive();\n"
                                   "}";

        // add source
        glShaderSource(m_vertexShader, 1, &vertexCode, NULL);
        glShaderSource(m_fragmentShader, 1, &fragmentCode, NULL);
        glShaderSource(m_geometryShader, 1, &geometryCode, NULL);

        // complie source
        glCompileShader(m_vertexShader);
        glCompileShader(m_fragmentShader);
        glCompileShader(m_geometryShader);

        // attach shader
        glAttachShader(m_program, m_vertexShader);
        glAttachShader(m_program, m_fragmentShader);
        glAttachShader(m_program, m_geometryShader);

        // link shader program
        glLinkProgram(m_program);

        // delete shader
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
        glDeleteShader(m_geometryShader);
    }

    ~ParticleNormalShader() {
        glDeleteBuffers(1, &m_vbo1);
        glDeleteBuffers(1, &m_vbo2);
        glDeleteVertexArrays(1, &m_vao);
    }

    void draw(const Eigen::Matrix4f &mvp, const MatrixXf &positions, const MatrixXf &normals, const Eigen::Vector4f &color, float normalLength = 0.05f) {

        // bind vao
        glBindVertexArray(m_vao);

        // get attribute id
        GLint positionId = glGetAttribLocation(m_program, "position");
        GLint normalId = glGetAttribLocation(m_program, "normal");
        glEnableVertexAttribArray(positionId);
        glEnableVertexAttribArray(normalId);

        // store vertex data into vbo1
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(positionId, positions.rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // store normal data into vbo2
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * normals.size(), normals.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(normalId, normals.rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        glBindVertexArray(0);

        // bind shader program
        glUseProgram(m_program);

        // set mvp uniform
        glUniformMatrix4fv(glGetUniformLocation(m_program, "mvp"), 1, GL_FALSE, mvp.data());
        glUniform4fv(glGetUniformLocation(m_program, "color"), 1, color.data());
        glUniform1f(glGetUniformLocation(m_program, "normalLength"), normalLength);

        // draw
        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, positions.cols());
        glBindVertexArray(0);

        // unbind shader program
        glUseProgram(0);
    }

private:
    GLuint m_program;
    GLuint m_vertexShader, m_fragmentShader, m_geometryShader;
    GLuint m_vao, m_vbo1, m_vbo2;
};

} // namespace cs224

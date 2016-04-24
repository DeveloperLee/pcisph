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

class ParticleShader {
public:
    ParticleShader() {

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
                                   "    out_color = vec4(1.0, 1.0, 0.0);\n"
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

        // delete shader
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
    }

    ~ParticleShader() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void draw(const Eigen::Matrix4f &mvp, const MatrixXf &positions) {

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

        glPointSize(2);

        // draw
        glBindVertexArray(m_vao);
        glDrawArrays(GL_POINTS, 0, positions.cols());
        glBindVertexArray(0);

        glPointSize(1);

        // unbind shader program
        glUseProgram(0);
    }

private:
    GLuint m_program;
    GLuint m_vertexShader, m_fragmentShader;
    GLuint m_vao, m_vbo;
};

} // namespace cs224

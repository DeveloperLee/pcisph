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

class GridShader {
public:
    GridShader(int size = 10, float space = 0.1f) {

        MatrixXf positions(3, (4 * size * 2 + 1));
        int index = 0;

        for (int i = -size; i <= size; ++ i) {
            positions.col(index ++) = Vector3f(i, 0.f, -size) * space;
            positions.col(index ++) = Vector3f(i, 0.f,  size) * space;
            positions.col(index ++) = Vector3f(-size, 0.f, i) * space;
            positions.col(index ++) = Vector3f( size, 0.f, i) * space;
        }

        m_numVertices = index;

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

        // delete shader
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);

        // bind vao
        glBindVertexArray(m_vao);

        GLint positionId = glGetAttribLocation(m_program, "position");
        glEnableVertexAttribArray(positionId);

        // store vertex data into vbo
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * positions.size(), positions.data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(positionId, positions.rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void draw(const Matrix4f &mvp) {

        // bind shader program
        glUseProgram(m_program);

        // set mvp uniform
        glUniformMatrix4fv(glGetUniformLocation(m_program, "mvp"), 1, GL_FALSE, mvp.data());

        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

        // draw
        glBindVertexArray(m_vao);
        glDrawArrays(GL_LINES, 0, m_numVertices);
        glBindVertexArray(0);

        glDisable(GL_BLEND);

        // unbind shader program
        glUseProgram(0);
    }

private:
    GLuint m_program;
    GLuint m_vertexShader, m_fragmentShader;
    GLuint m_vao, m_vbo;

    int m_numVertices = 0;
};



} // namespace cs224

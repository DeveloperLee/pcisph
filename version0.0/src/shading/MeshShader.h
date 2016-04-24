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

class MeshShader {
public:
    MeshShader() {

        // init shader program id
        m_program = glCreateProgram();

        // init shader id
        m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

        // init vao, vbo
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo1);
        glGenBuffers(1, &m_vbo2);
        glGenBuffers(1, &m_vbo3);

        // shader programs
        const char *vertexCode = "#version 330\n"
                                 "uniform mat4 mvp;\n"
                                 "in vec3 position;\n"
                                 "in vec3 normal;\n"
                                 "out vec3 vNormal;\n"
                                 "void main() {\n"
                                 "    gl_Position = mvp * vec4(position, 1.0);\n"
                                 "    vNormal = normal;\n"
                                 "}";

        const char *fragmentCode = "#version 330\n"
                                   "uniform vec4 color;\n"
                                   "in vec3 vNormal;\n"
                                   "out vec4 out_color;\n"
                                   "void main() {\n"
                                   "    float s = mix(0.1, 0.9, abs(dot(vNormal, vec3(0.0, 1.0, 0.0))));\n"
                                   "    out_color = vec4(s * color.rgb, color.a);\n"
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

        // delete shader id
        glDeleteShader(m_vertexShader);
        glDeleteShader(m_fragmentShader);
    }

    MeshShader(const Mesh &mesh) : MeshShader() {

        setMesh(mesh);
    }

    ~MeshShader() {

        glDeleteBuffers(1, &m_vbo1);
        glDeleteBuffers(1, &m_vbo2);
        glDeleteBuffers(1, &m_vbo3);
        glDeleteVertexArrays(1, &m_vao);
    }

    void setMesh(const Mesh &mesh) {

        // bind vao
        glBindVertexArray(m_vao);

        // get attributes id
        GLint positionId = glGetAttribLocation(m_program, "position");
        GLint normalId = glGetAttribLocation(m_program, "normal");
        glEnableVertexAttribArray(positionId);
        glEnableVertexAttribArray(normalId);

        // store vertex data into vbo1
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo1);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.vertices().size(), mesh.vertices().data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(positionId, mesh.vertices().rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // store normal data into vbo2
        glBindBuffer(GL_ARRAY_BUFFER, m_vbo2);
        glBufferData(GL_ARRAY_BUFFER, sizeof(float) * mesh.normals().size(), mesh.normals().data(), GL_DYNAMIC_DRAW);
        glVertexAttribPointer(normalId, mesh.normals().rows(), GL_FLOAT, GL_FALSE, 0, 0);
        glBindBuffer(GL_ARRAY_BUFFER, 0);

        // store index data into vbo3
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_vbo3);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(uint32_t) * mesh.triangles().size(), mesh.triangles().data(), GL_DYNAMIC_DRAW);

        // unbind vao
        glBindVertexArray(0);

        m_numTriangles = mesh.triangles().cols();
    }

    void draw(const Eigen::Matrix4f &mvp, const Eigen::Vector4f &color) {

        // bind shader program
        glUseProgram(m_program);

        // set uniforms
        glUniformMatrix4fv(glGetUniformLocation(m_program, "mvp"), 1, GL_FALSE, mvp.data());
        glUniform4fv(glGetUniformLocation(m_program, "color"), 1, color.data());

        // draw
        glBindVertexArray(m_vao);
        glDrawElements(GL_TRIANGLES, m_numTriangles, GL_UNSIGNED_INT, 0);
        glBindVertexArray(0);

        // unbind shader program
        glUseProgram(0);
    }

private:
    GLuint m_program;
    GLuint m_vertexShader, m_fragmentShader;
    GLuint m_vao, m_vbo1, m_vbo2, m_vbo3;

    int m_numTriangles = 0;
};

} // namespace cs224

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

class ParticleSphereShader {
public:
    ParticleSphereShader() {

        // init shader program id
        m_program = glCreateProgram();

        // init shader id
        m_vertexShader = glCreateShader(GL_VERTEX_SHADER);
        m_fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
        m_geometryShader = glCreateShader(GL_GEOMETRY_SHADER);

        // init vao, vbo
        glGenVertexArrays(1, &m_vao);
        glGenBuffers(1, &m_vbo);

        // shader programs
        const char *vertexCode = "#version 330\n"
                                 "uniform mat4 mv;\n"
                                 "in vec3 position;\n"
                                 "out vec4 vPosition;\n"
                                 "void main() {\n"
                                 "    vPosition = mv * vec4(position, 1.0);\n"
                                 "}";

        const char *fragmentCode = "#version 330\n"
                                   "uniform vec4 color;\n"
                                   "in vec2 gPosition;\n"
                                   "out vec4 out_color;\n"
                                   "void main() {\n"
                                   "    vec3 n = vec3(2.0 * gPosition, 0.0);\n"
                                   "    float r2 = dot(n.xy, n.xy);\n"
                                   "    if (r2 > 1.0) discard;\n"
                                   "    n.z = 1.0 - sqrt(r2);\n"
                                   "    vec3 L = normalize(vec3(1.0));\n"
                                   "    float d = max(0.0, dot(L, n));\n"
                                   "    out_color = vec4(d * color.xyz, color.w);\n"
                                   "}";

        const char *geometryCode = "#version 330\n"
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

    ~ParticleSphereShader() {
        glDeleteBuffers(1, &m_vbo);
        glDeleteVertexArrays(1, &m_vao);
    }

    void draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, const Eigen::Vector4f &color, float particleRadius = 0.03f) {

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
        glUniformMatrix4fv(glGetUniformLocation(m_program, "mv"), 1, GL_FALSE, mv.data());
        glUniformMatrix4fv(glGetUniformLocation(m_program, "proj"), 1, GL_FALSE, proj.data());
        glUniform4fv(glGetUniformLocation(m_program, "color"), 1, color.data());
        glUniform1f(glGetUniformLocation(m_program, "particleRadius"), particleRadius);

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
    GLuint m_vao, m_vbo;
};

} // namespace cs224

#include "PCIShader.h"
#include <iostream>
#include <fstream>

using namespace std;

namespace cs224 {

PCIShader::PCIShader() : m_vertexShader(0), 
                       m_fragmentShader(0), 
                       m_geometryShader(0),
                       m_program(0), 
                       m_vao(0) {
                          
};

bool PCIShader::buildShader(const std::string &name,
                     const std::string &vertex_str,
                     const std::string &fragment_str,
                     const std::string &geometry_str) {

    glGenVertexArrays(1, &m_vao);
    m_shaderName = name;
    m_vertexShader = createShader(GL_VERTEX_SHADER, name, vertex_str);
    m_geometryShader = createShader(GL_GEOMETRY_SHADER, name, geometry_str);
    m_fragmentShader = createShader(GL_FRAGMENT_SHADER, name, fragment_str);
    
    if(!checkShaderProgram(geometry_str)) return false;

    m_program = glCreateProgram();

    glAttachShader(m_program, m_vertexShader);
    glAttachShader(m_program, m_fragmentShader);

    if (m_geometryShader) {
        glAttachShader(m_program, m_geometryShader);
    }

    glLinkProgram(m_program);
    
    return true;
}

// Bind the current shader and vao to opengl.
// This method should be called before draw()
void PCIShader::bind() {
    glUseProgram(m_program);
    glBindVertexArray(m_vao);
}

// Update VAO, VBO and element buffer array information.
void PCIShader::updateBindings(const std::string &name, uint32_t size, int dim,
                            uint32_t compSize, GLuint glType, bool integral,
                            const uint8_t *data) {
    int attribID = 0;
    if (name != "indices") {
        attribID = glGetAttribLocation(m_program, name.c_str());
        if (attribID < 0)
            return;
    }
 
    GLuint bufferID = checkVBOCache(name, size, dim, compSize, glType, integral);

    size_t totalSize = (size_t) size * (size_t) compSize;
    
    // This is an element buffer array, binds to GL_ELEMENT_ARRAY_BUFFER
    if (name == "indices") {
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, totalSize, data, GL_DYNAMIC_DRAW);
    } else {  // This is an VAO buffer, binds to GL_ARRAY_BUFFER.
        glBindBuffer(GL_ARRAY_BUFFER, bufferID);
        glBufferData(GL_ARRAY_BUFFER, totalSize, data, GL_DYNAMIC_DRAW);
        if (size == 0) {
            glDisableVertexAttribArray(attribID);
        } else {
            glEnableVertexAttribArray(attribID);
            glVertexAttribPointer(attribID, dim, glType, integral, 0, 0);
        }
    }
}

GLint PCIShader::getUnifLocation(const std::string &name) const {

    GLint id = glGetUniformLocation(m_program, name.c_str());

    if (id == -1) {
        std::cerr << m_shaderName << ": warning: did not find uniform " << name << std::endl;
    }
    return id;
}

// Buffer_type = 0 --> Draw Element buffer array
// Buffer_type = 1 --> Draw Vertex array buffer
void PCIShader::draw(int type, uint32_t offset, uint32_t count, int buffer_type) {
    
    
    if(buffer_type == 0) {
       int scale = (type == GL_TRIANGLES ? 3 : 2);
       offset *= scale;
       count  *= scale; 
       glDrawElements(type, (GLsizei) count, GL_UNSIGNED_INT, (const void *)(offset * sizeof(uint32_t)));
    }  else {
       glDrawArrays(type, offset, count);
    }
}

// Helper function to create a shader 
GLuint PCIShader::createShader(GLint type, const std::string &name, std::string shader_string) 
{    
    // If the shader string is empty, return.
    if (shader_string.empty()) {
        return (GLuint) 0;
    }
    
    GLuint id = glCreateShader(type);
    const char *shader_string_const = shader_string.c_str();
    glShaderSource(id, 1, &shader_string_const, nullptr);
    glCompileShader(id);

    return id;
}

// Check the completeness of the shader program.
bool PCIShader::checkShaderProgram(const std::string &geometry_str) {

    if (!m_vertexShader || !m_fragmentShader) {
        return false;
    }
    if (!geometry_str.empty() && !m_geometryShader) {
        return false;
    }

    return true;
}

// Check the existing vbo cache.
GLuint PCIShader::checkVBOCache(const std::string &name, uint32_t size, int dim,
                                uint32_t compSize, GLuint glType, bool integral) {
     GLuint vboID;
    
    // If vbo is already cached, get from the cache instead of creating a new one.

    auto it = m_vboCache.find(name);
    if (it != m_vboCache.end()) {
        Buffer &buffer = it->second;
        vboID = it->second.id;
        buffer.size = size;
        buffer.compSize = compSize;
    } else {
        glGenBuffers(1, &vboID);
        Buffer buffer;
        buffer.id = vboID;
        buffer.glType = glType;
        buffer.dim = dim;
        buffer.compSize = compSize;
        buffer.size = size;
        m_vboCache[name] = buffer;
    }

    return vboID;
}

}

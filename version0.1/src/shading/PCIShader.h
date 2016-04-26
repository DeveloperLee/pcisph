// Shader class for creating shader programs.

#pragma once

#include <map>
#include <Eigen/Geometry>
#include <utils/Def.h>

namespace cs224 {

class PCIShader {

public: 

	PCIShader(); 

    bool buildShader(const std::string &name, 
    	             const std::string &vertex_str,
              		 const std::string &fragment_str,
              		 const std::string &geometry_str = "");

    void bind();

    /// Upload an Eigen matrix as a vertex buffer object (refreshing it as needed)
    template <typename Matrix> void updateBindings(const std::string &name, const Matrix &M) {
        uint32_t compSize = sizeof(typename Matrix::Scalar);
        GLuint glType = (GLuint) CPP2GL_types<typename Matrix::Scalar>::type;
        bool integral = (bool) CPP2GL_types<typename Matrix::Scalar>::integral;

        updateBindings(name, (uint32_t) M.size(), (int) M.rows(), compSize,
                     glType, integral, (const uint8_t *) M.data());
    }
    
    // Get the uniform variable location in the shader program
    GLint getUnifLocation(const std::string &name) const;
    
    void draw(int type, uint32_t offset, uint32_t count, int buffer_type);

    /** Set different types of uniform into shader **/ 

    void setUniform(const std::string &name, const Eigen::Matrix4f &mat) {
        glUniformMatrix4fv(getUnifLocation(name), 1, GL_FALSE, mat.data());
    }


    void setUniform(const std::string &name, int value) {
        glUniform1i(getUnifLocation(name), value);
    }

   
    void setUniform(const std::string &name, float value) {
        glUniform1f(getUnifLocation(name), value);
    }

    
    void setUniform(const std::string &name, const Eigen::Vector2f &v) {
        glUniform2f(getUnifLocation(name), v.x(), v.y());
    }

    
    void setUniform(const std::string &name, const Eigen::Vector3f &v) {
        glUniform3f(getUnifLocation(name), v.x(), v.y(), v.z());
    }

    
    void setUniform(const std::string &name, const Eigen::Vector4f &v) {
        glUniform4f(getUnifLocation(name), v.x(), v.y(), v.z(), v.w());
    }


protected:

	void updateBindings(const std::string &name, uint32_t size, int dim,
                       uint32_t compSize, GLuint glType, bool integral, 
                       const uint8_t *data);

	GLuint createShader(GLint type, const std::string &name, std::string shader_string);
    
    GLuint checkVBOCache(const std::string &name, uint32_t size, int dim,
                         uint32_t compSize, GLuint glType, bool integral);

    bool checkShaderProgram(const std::string &geometry_str);

	struct Buffer {
        GLuint id;
        GLuint glType;
        GLuint dim;
        GLuint compSize;
        GLuint size;
	};

	std::string m_shaderName;
    GLuint m_vertexShader;
    GLuint m_fragmentShader;
    GLuint m_geometryShader;
    GLuint m_program;
    GLuint m_vao;
    std::map<std::string, Buffer> m_vboCache;

};
}
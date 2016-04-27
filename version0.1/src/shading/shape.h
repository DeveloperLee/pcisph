#pragma once

#include <utils/Def.h>

namespace cs224{

    class Shape
    {
    public:
        Shape();

        virtual ~Shape();

        /** Initialize the VBO with the given vertex data. */
        void setVertexData(GLfloat *data, GLsizeiptr size, GLenum drawMode, int numVertices);

        /** Enables the specified attribute and calls glVertexAttribPointer with the given arguments. */
        void setAttribute(GLuint index, GLint size, GLenum type, GLboolean normalized,
                          GLsizei stride, size_t pointer);

        /** Draw the initialized geometry. */
        void draw();


    private:
        GLuint m_vboID;    /// ID of the vertex buffer object (VBO)

        GLuint m_vaoID;    /// ID of the vertex array object (VAO)

        int m_numVertices; /// Number of vertices to be drawn.

        GLenum m_drawMode; /// Drawing mode for glDrawArrays (e.g. GL_TRIANGLES, etc)

        int m_perVert;
    };
}



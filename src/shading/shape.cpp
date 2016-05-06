#include "shape.h"
namespace cs224{

    Shape::Shape() :
        m_vboID(0),
        m_vaoID(0),
        m_numVertices(0),
        m_drawMode(GL_TRIANGLES)
    {
        // TODO [Task 1 & 3]: Generate a VBO and a VAO.
        glGenBuffers(1,&m_vboID);
        glGenVertexArrays(1,&m_vaoID);

    }

    Shape::~Shape()
    {
        // TODO [Task 6]: Delete the VBO and VAO.
        glDeleteBuffers(1,&m_vboID);
        glDeleteVertexArrays(1,&m_vboID);
    }


    /**
     * @param data - Array of floats containing the vertex data.
     * @param size - Size of the data array, in bytes.
     * @param drawMode - Drawing mode. (e.g. GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_TRIANGLE_FAN)
     * @param numVertices - Number of vertices to be rendered.
     */
    void Shape::setVertexData(GLfloat *data, GLsizeiptr size, GLenum drawMode, int numVertices)
    {
        // Stores the draw mode and number of vertices, which will be used for drawing later.
        m_drawMode = drawMode;
        m_numVertices = numVertices;

        // TODO [Task 2]: Store data in the VBO.
        glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
        glBufferData(GL_ARRAY_BUFFER,size,data,GL_STATIC_DRAW);
        glBindBuffer(GL_ARRAY_BUFFER,0);

    }


    /**
     * @param index - Index of the vertex attribute.
     * @param size - Number of components for the attribute. (e.g. 3 for a vec3)
     * @param type - Data type of the components. (e.g. GL_FLOAT)
     * @param normalized - Whether integers should be converted to floats (just use GL_FALSE for this)
     * @param stride - Byte offset between each vertex in the data array.
     * @param pointer - Byte offset of this attribute from the beginning of the data array.
     */
    void Shape::setAttribute(
            GLuint index, GLint size, GLenum type, GLboolean normalized, GLsizei stride, size_t pointer)
    {
        // TODO [Task 4]: Enable and define the vertex attribute.
        glBindVertexArray(m_vaoID);
        glBindBuffer(GL_ARRAY_BUFFER,m_vboID);
        glEnableVertexAttribArray(index);
        glVertexAttribPointer(index,size,type,normalized,stride,(void *) pointer);
        glBindVertexArray(0);
        glBindBuffer(GL_ARRAY_BUFFER,0);


    }


    void Shape::draw()
    {
        // TODO [Task 5]: Draw the shape.
        glBindVertexArray(m_vaoID);
        glDrawArrays(m_drawMode,0,m_numVertices);
        glBindVertexArray(0);

    }

}

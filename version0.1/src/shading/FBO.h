#pragma once

#include <utils/Def.h>

#include "PCIShader.h"
#include "shape.h"

namespace cs224 {

class FBO {
public:

    FBO(int num_color_attachments, int sizex, int sizey, bool render_buffer, bool depth_attachment);
    ~FBO();
    void Bind();
    void unBind();
    void renderTextureToFullScreen(int tex_attachment, bool depth, const Eigen::Matrix4f &mat, float near, float far);
private:
    GLuint m_FBOID;
    std::vector<GLuint> m_texIDs;
    GLuint m_RBID;
    GLuint m_depthtex;
    GLuint m_quadShader;
    std::unique_ptr<Shape> m_quad;
};

} // namespace cs224

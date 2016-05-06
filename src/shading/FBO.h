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
    GLuint getDepthTexture();
    GLuint getColorTexture(int tex_attachment);
    void renderTextureToFullScreen(int tex_attachment, bool depth, int render_style, const Eigen::Matrix4f &mat,const Eigen::Matrix4f &v, Shape *quad,float near, float far);
    void replaceColorAttachmentTexture(GLuint new_tex_id,int color_attachment);
    void renderSpecificTextureToFullScreen(GLuint texID, Shape *quad);
private:
    GLuint m_FBOID;
    std::vector<GLuint> m_texIDs;
    GLuint m_RBID;
    GLuint m_depthtex;
    GLuint m_quadShader;

    bool has_depth_attachment;
};

} // namespace cs224

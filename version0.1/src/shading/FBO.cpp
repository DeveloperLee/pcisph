#include "FBO.h"
#include "shape.h"
#include "utils/ResourceLoader.h"
#include <Eigen/Dense>

namespace cs224 {

    FBO::FBO(int num_color_attachments, int sizex, int sizey, bool render_buffer, bool depth_attachment):has_depth_attachment(depth_attachment){
        glGenFramebuffers(1,&m_FBOID);
        glBindFramebuffer(GL_FRAMEBUFFER,m_FBOID);

        for(int i=0;i<num_color_attachments;i++){
            GLuint texID;
            glGenTextures(1,&texID);
            glBindTexture(GL_TEXTURE_2D,	texID);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,	0,	GL_RGBA32F,	sizex,	sizey,
                         0,	GL_RGBA,	GL_FLOAT,	0);

            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_COLOR_ATTACHMENT0 + i,GL_TEXTURE_2D,texID,0);
            m_texIDs.push_back(texID);
        }

        if(render_buffer){
            glGenRenderbuffers(1, &m_RBID);
            glBindRenderbuffer(GL_RENDERBUFFER, m_RBID);
            glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT, sizex, sizey);
            glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_RBID);
        }

        if(depth_attachment){
            glGenTextures(1,&m_depthtex);
            glBindTexture(GL_TEXTURE_2D,	m_depthtex);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
            glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

            glTexImage2D(GL_TEXTURE_2D,	0,	GL_DEPTH_COMPONENT,	sizex,	sizey,
                         0,	GL_DEPTH_COMPONENT,	GL_FLOAT,	0);

            glFramebufferTexture2D(GL_FRAMEBUFFER,GL_DEPTH_ATTACHMENT,GL_TEXTURE_2D,m_depthtex,0);
            glDrawBuffer(GL_NONE);
            glReadBuffer(GL_NONE);
        }



        GLenum status = glCheckFramebufferStatus(GL_FRAMEBUFFER);
        if( status != GL_FRAMEBUFFER_COMPLETE){
            std::cout << "failed init of FBO" << std::endl;
        }

        glBindFramebuffer(GL_FRAMEBUFFER, 0);




        //init quad shader

        const std::string vert_str = ResourceLoader::fileToString(base_directory+"shaders/quad.vert");

        const std::string frag_str = ResourceLoader::fileToString(base_directory+"shaders/quad.frag");

        m_quadShader = ResourceLoader::loadShadersFromText(vert_str.c_str(),frag_str.c_str());
    }

    FBO::~FBO()
    {

    }

    void FBO::Bind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER,m_FBOID);
        GLenum bufs[] = {GL_COLOR_ATTACHMENT0};
        glDrawBuffers(1,bufs);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    }

    void FBO::unBind()
    {
        glBindFramebuffer(GL_FRAMEBUFFER,0);
    }

    GLuint FBO::getDepthTexture()
    {
        if(has_depth_attachment){
            return m_depthtex;
        }else{
            std::cout << "depth texture requested but doesnt have one";
            return 0;
        }
    }

    GLuint FBO::getColorTexture(int tex_attachment)
    {
        if(tex_attachment>=m_texIDs.size()){
            std::cout << "no color attachment to get" << std::endl;
            return 0;
        }else{
            return m_texIDs[tex_attachment];
        }
    }

    void FBO::renderTextureToFullScreen(int tex_attachment, bool depth, int render_style, const Eigen::Matrix4f &mat,const Eigen::Matrix4f &v,Shape *quad, float near=0, float far=0)
    {
        glUseProgram(m_quadShader);
        GLuint texture_to_render;
        if(depth){
            texture_to_render = m_depthtex;

        }else{
            if(tex_attachment>=m_texIDs.size()){
                std::cout << "no color attachment to render" << std::endl;
                return;
            }else{
                texture_to_render = m_texIDs[tex_attachment];
            }
        }

        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
        glActiveTexture(GL_TEXTURE0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        //bind quad shader
        //set uniforms
        glUniform1i(glGetUniformLocation(m_quadShader,"render_style"),render_style);
        glUniform1i(glGetUniformLocation(m_quadShader,"tex"),0);
        glUniform1f(glGetUniformLocation(m_quadShader,"near"),near);
        glUniform1f(glGetUniformLocation(m_quadShader,"far"),far);
        glUniformMatrix4fv(glGetUniformLocation(m_quadShader,"proj"),1,GL_FALSE,mat.data());
        glUniformMatrix4fv(glGetUniformLocation(m_quadShader,"view"),1,GL_FALSE,v.data());

        glBindTexture(GL_TEXTURE_2D, texture_to_render);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        quad->draw();


    }

}

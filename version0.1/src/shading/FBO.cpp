#include "FBO.h"
#include "shape.h"
#include "utils/ResourceLoader.h"
#include <Eigen/Dense>

namespace cs224 {

    FBO::FBO(int num_color_attachments, int sizex, int sizey, bool render_buffer, bool depth_attachment){
        glGenFramebuffers(1,&m_FBOID);
        glBindFramebuffer(GL_FRAMEBUFFER,m_FBOID);

        for(int i=0;i<num_color_attachments;i++){
            GLuint texID;
            glGenTextures(1,&texID);
            glBindTexture(GL_TEXTURE_2D,	texID);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
            glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
            glTexImage2D(GL_TEXTURE_2D,	0,	GL_RGBA,	sizex,	sizey,
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

        m_quad.reset(new Shape());
        GLfloat quadData[30] = {
            -1.f,1.f,0.f, 0.f,1.f,
            -1.f,-1.f,0.f, 0.f,0.f,
            1.f,1.f,0.f, 1.f,1.f,
            1.f,1.f,0.f, 1.f,1.f,
            -1.f,-1.f,0.f, 0.f,0.f,
            1.f,-1.f,0.f, 1.f,0.f

        };
        m_quad->setVertexData(quadData,sizeof(quadData),GL_TRIANGLES,6);
        m_quad->setAttribute(0,3,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 5,0);
        m_quad->setAttribute(1,2,GL_FLOAT,GL_FALSE,sizeof(GLfloat) * 5, (sizeof(GLfloat) * 3));


        //init quad shader

        const std::string vert_str = "#version 330\n"
                     "layout(location = 0) in vec3 position;\n"
                     "layout(location = 1) in vec2 inUV;\n"
                     "out vec2 uv;\n"
                     "void main() {\n"
                     "    uv = inUV;\n"
                     "    gl_Position = vec4(position, 1.0);\n"
                     "}";

        const std::string frag_str = "#version 330\n"
                   "uniform sampler2D tex;\n"
                   "in vec2 uv;\n"
                   "out vec4 fragColor;\n"
                   "uniform float near;\n"
                   "uniform float far;\n"
                   "uniform mat4 proj;\n"
                   "vec3 uvToEye(vec2 texCoord) {\n"
                   "   float depth = texture(tex,texCoord).x;\n"
                   "   vec2 transUV = (texCoord - vec2(.5))*2;\n"
                   "   vec4 sscoord = vec4(transUV,depth,1);\n"
                   "   mat4 invproj = inverse(proj);\n"
                   "   vec4 eyepos = invproj * sscoord;\n"
                   "   return eyepos.xyz/eyepos.w;\n"
                   "}\n"
                   "void main() {\n"
                   "    if(1<0){\n"
                   "        fragColor = texture(tex,uv);\n"
                   "    }else{\n"
                   "        vec2 texelSize = 1.0 / textureSize(tex,0).xy;\n"
                   "        float n = near;\n"
                   "        float f = far;\n"
                   "        float z = texture(tex,uv).x;\n"
                   "        //z = (2.0 * n) / (f + n - z * (f - n));\n"
                   "        vec3 eyepos = uvToEye(uv);\n"
                   "        vec3 ddx = uvToEye(uv+vec2(texelSize.x,0)) - eyepos;\n"
                   "        vec3 ddx2 = eyepos - uvToEye(uv-vec2(texelSize.x,0));\n"
                   "        vec3 ddy = uvToEye(uv+vec2(0,texelSize.y))-eyepos;\n"
                   "        vec3 ddy2 = eyepos - uvToEye(uv-vec2(0,texelSize.y));\n"
                   "        vec3 norm = cross(ddx,ddy);\n"
                   "        fragColor = vec4((normalize(norm)+1.)/2.,1);\n"
                   "        //fragColor = vec4(z,z,z,1);\n"
                   "        //fragColor = texture(tex,uv);\n"
                   "    }\n"
                   "}";


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

    void FBO::renderTextureToFullScreen(int tex_attachment, bool depth, const Eigen::Matrix4f &mat, float near=0, float far=0)
    {
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
        glUseProgram(m_quadShader);
        //bind quad shader
        //set uniforms

        glUniform1i(glGetUniformLocation(m_quadShader,"tex"),0);
        glUniform1f(glGetUniformLocation(m_quadShader,"near"),near);
        glUniform1f(glGetUniformLocation(m_quadShader,"far"),far);
        glUniformMatrix4fv(glGetUniformLocation(m_quadShader,"proj"),1,GL_FALSE,mat.data());

        glBindTexture(GL_TEXTURE_2D, texture_to_render);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
        m_quad->draw();


    }

}

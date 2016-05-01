#include "SSFRenderer.h"

#include "shape.h"

namespace cs224{

SSFRenderer::SSFRenderer() : m_depthFBO(0,1300,700,false,true),
                            m_blurHorizFBO(1,1300,700,true,false),
                            m_blurVertFBO(1,1300,700,true,false),
                            m_cfFBO1(0,1300,700,false,true),
                            m_cfFBO2(0,1300,700,false,true),
                            m_thicknessFBO(1,1300,700,true,false)
{

    // shader programs
    const std::string vert_str = ResourceLoader::fileToString(base_directory+"shaders/particle.vert");

    const std::string frag_str = ResourceLoader::fileToString(base_directory+"shaders/particleDepth.frag");
    const std::string frag_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/particleThickness.frag");

    const std::string geo_str = ResourceLoader::fileToString(base_directory+"shaders/particle.geo");

    shader.buildShader("SSF", vert_str, frag_str, geo_str);

    m_thicknessShader.buildShader("SSFThickness", vert_str, frag_thickness_str, geo_str);



    const std::string vert_blur_str = ResourceLoader::fileToString(base_directory+"shaders/blur.vert");

    const std::string frag_blur_str = ResourceLoader::fileToString(base_directory+"shaders/blur.frag");

    m_blurShader = ResourceLoader::loadShadersFromText(vert_blur_str.c_str(),frag_blur_str.c_str());


    const std::string vert_cf_str = ResourceLoader::fileToString(base_directory+"shaders/blur.vert");

    const std::string frag_cf_str = ResourceLoader::fileToString(base_directory+"shaders/curvatureflow.frag");

    m_curvatureFlowShader = ResourceLoader::loadShadersFromText(vert_cf_str.c_str(),frag_cf_str.c_str());


    const std::string vert_quad_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quad.vert");

    const std::string frag_quad_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quadThickness.frag");

    m_quadThicknessShader = ResourceLoader::loadShadersFromText(vert_quad_thickness_str.c_str(),frag_quad_thickness_str.c_str());


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



}

void SSFRenderer::onKeyPress(int key){
    if(key == GLFW_KEY_Q){
        near += .1;
    }else if(key == GLFW_KEY_A){
        near -= .1;
    }else if(key == GLFW_KEY_W){
        far += .1;
    }else if(key == GLFW_KEY_S){
        far -= .1;
    }
    std::cout << near << " " << far << std::endl;
}

void SSFRenderer::onKeyReleased(int key)
{

}
void SSFRenderer::draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, const Eigen::Vector4f &color, float particleRadius) {
    //depthpass
    m_depthFBO.Bind();
    shader.bind();
    shader.updateBindings("position", positions);
    shader.setUniform("mv", mv);
    shader.setUniform("proj", proj);
    shader.setUniform("particleRadius", particleRadius);
    //shader.setUniform("color", color);
    glUniform4f(glGetUniformLocation(shader.m_program,"color"),color.x(),color.y(),color.z(),1);
    glEnable(GL_DEPTH_TEST);
    shader.draw(GL_POINTS, 0, positions.cols(), 1);
    m_depthFBO.unBind();

    //thickness pass
    m_thicknessFBO.Bind();
    m_thicknessShader.bind();
    m_thicknessShader.updateBindings("position", positions);
    m_thicknessShader.setUniform("mv", mv);
    m_thicknessShader.setUniform("proj", proj);
    m_thicknessShader.setUniform("particleRadius", particleRadius);
    //shader.setUniform("color", color);
    glUniform4f(glGetUniformLocation(m_thicknessShader.m_program,"color"),color.x(),color.y(),color.z(),1);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    m_thicknessShader.draw(GL_POINTS, 0, positions.cols(), 1);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    m_thicknessFBO.unBind();

    m_thicknessFBO.renderTextureToFullScreen(0,false,proj,mv,m_quad.get(),near,far);

    m_blurHorizFBO.Bind();
    glUseProgram(m_blurShader);
    glBindTexture(GL_TEXTURE_2D,m_thicknessFBO.getColorTexture(0));
    glUniform1i(glGetUniformLocation(m_blurShader,"horiz"),1);
    m_quad->draw();

    m_blurVertFBO.Bind();
    glBindTexture(GL_TEXTURE_2D,m_blurHorizFBO.getColorTexture(0));
    glUniform1i(glGetUniformLocation(m_blurShader,"horiz"),0);
    m_quad->draw();

    glUseProgram(m_curvatureFlowShader);
    glUniform2f(glGetUniformLocation(m_curvatureFlowShader,"screen_size"),1300.f,700.f);
    glUniformMatrix4fv(glGetUniformLocation(m_curvatureFlowShader,"p"),1,GL_FALSE,proj.data());

    m_cfFBO1.Bind();
    glBindTexture(GL_TEXTURE_2D,m_depthFBO.getDepthTexture());
    m_quad->draw();

    bool use_first = false;

    for(int i=0;i<40;i++){
        if(use_first){
            m_cfFBO1.Bind();
            glBindTexture(GL_TEXTURE_2D,m_cfFBO2.getDepthTexture());
            m_quad->draw();
        }else{
            m_cfFBO2.Bind();
            glBindTexture(GL_TEXTURE_2D,m_cfFBO1.getDepthTexture());
            m_quad->draw();
        }
        use_first = !use_first;
    }




    //m_cfFBO1.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);





    //m_blurVertFBO.renderTextureToFullScreen(0,false,proj,mv,m_quad.get(),near,far);
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    glClearColor(.8,.8,.8,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,1);


    glUseProgram(m_quadThicknessShader);
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_cfFBO1.getDepthTexture());

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_blurVertFBO.getColorTexture(0));

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"tex"),0);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"thickness"),1);




//    glUniform1i(glGetUniformLocation(m_quadShader,"tex"),0);
//    glUniform1f(glGetUniformLocation(m_quadShader,"near"),near);
//    glUniform1f(glGetUniformLocation(m_quadShader,"far"),far);
    glUniformMatrix4fv(glGetUniformLocation(m_quadThicknessShader,"proj"),1,GL_FALSE,proj.data());
    glUniformMatrix4fv(glGetUniformLocation(m_quadThicknessShader,"view"),1,GL_FALSE,mv.data());

//    glBindTexture(GL_TEXTURE_2D, texture_to_render);

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);
    m_quad->draw();



}

}




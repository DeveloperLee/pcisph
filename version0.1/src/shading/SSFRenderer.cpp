#include "SSFRenderer.h"

#include "shape.h"
#include "SOIL.h"

namespace cs224{

SSFRenderer::SSFRenderer(const Vector2i &size) : m_depthFBO(0,size.x(),size.y(),false,true),
                            m_blurHorizFBO(1,size.x(),size.y(),true,false),
                            m_blurVertFBO(1,size.x(),size.y(),true,false),
                            m_cfFBO1(0,size.x(),size.y(),false,true),
                            m_cfFBO2(0,size.x(),size.y(),false,true),
                            m_thicknessFBO(1,size.x(),size.y(),true,false),
                            m_noiseFBO(1,size.x(),size.y(),true,false),
                            m_sceneFBO(1,size.x(),size.y(),true,true),
                            m_preRenderFBO(1,size.x(),size.y(),true,false),
                            m_size(size)
{
    //std::string image_location = base_directory+"images/grass.png";
    //GLuint id = SOIL_load_OGL_texture(image_location.c_str(),SOIL_LOAD_RGB,SOIL_CREATE_NEW_ID,SOIL_FLAG_MIPMAPS);
    //std::cout << id << std::endl;


    // shader programs
    const std::string vert_str = ResourceLoader::fileToString(base_directory+"shaders/particle.vert");

    const std::string frag_str = ResourceLoader::fileToString(base_directory+"shaders/particleDepth.frag");
    const std::string frag_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/particleThickness.frag");
    const std::string frag_noise_str = ResourceLoader::fileToString(base_directory+"shaders/particleNoise.frag");

    const std::string geo_str = ResourceLoader::fileToString(base_directory+"shaders/particle.geo");

    shader.buildShader("SSF", vert_str, frag_str, geo_str);

    m_thicknessShader.buildShader("SSFThickness", vert_str, frag_thickness_str, geo_str);

    m_noiseShader.buildShader("SSFNoise", vert_str, frag_noise_str, geo_str);



    const std::string vert_blur_str = ResourceLoader::fileToString(base_directory+"shaders/blur.vert");

    const std::string frag_blur_str = ResourceLoader::fileToString(base_directory+"shaders/blur.frag");

    m_blurShader = ResourceLoader::loadShadersFromText(vert_blur_str.c_str(),frag_blur_str.c_str());


    const std::string vert_cf_str = ResourceLoader::fileToString(base_directory+"shaders/blur.vert");

    const std::string frag_cf_str = ResourceLoader::fileToString(base_directory+"shaders/curvatureflow.frag");

    m_curvatureFlowShader = ResourceLoader::loadShadersFromText(vert_cf_str.c_str(),frag_cf_str.c_str());


    const std::string vert_quad_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quad.vert");

    const std::string frag_quad_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quadThickness.frag");

    m_quadThicknessShader = ResourceLoader::loadShadersFromText(vert_quad_thickness_str.c_str(),frag_quad_thickness_str.c_str());


    const std::string vert_trans_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quadtrans.vert");

    const std::string frag_trans_thickness_str = ResourceLoader::fileToString(base_directory+"shaders/quadtrans.frag");

    m_quadTransShader = ResourceLoader::loadShadersFromText(vert_trans_thickness_str.c_str(),frag_trans_thickness_str.c_str());


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


    glGenTextures(num_frames,preRender);
    for(int i=0;i<num_frames;i++){
        glBindTexture(GL_TEXTURE_2D,	preRender[i]);
        glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MAG_FILTER,	GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D,	GL_TEXTURE_MIN_FILTER,	GL_LINEAR);
        glTexImage2D(GL_TEXTURE_2D,	0,	GL_R3_G3_B2,	size.x(),	size.y(),
                     0,	GL_RGB,	 GL_UNSIGNED_BYTE_3_3_2,	0);
    }



}

void SSFRenderer::prepareToDrawScene()
{
    //vec3(.529,.808,.922);
    glClearColor(.529,.808,.922,1.f);
    glClearDepth(1.f);
    m_sceneFBO.Bind();
    glClearColor(0,0,0,0);
}

void SSFRenderer::onKeyPress(int key){
//    if(key == GLFW_KEY_Q){
//        near += .1;
//    }else if(key == GLFW_KEY_A){
//        near -= .1;
//    }else if(key == GLFW_KEY_W){
//        far += .1;
//    }else if(key == GLFW_KEY_S){
//        far -= .1;
//    }

    if(key == GLFW_KEY_ENTER && !enter_pressed){
        capped = frame_number;
        enter_pressed = true;
    }

    if(key == GLFW_KEY_TAB){
        renderType = (renderType+1)%6;
    }

    if(key == GLFW_KEY_1){
        render_settings.doNoise = !render_settings.doNoise;
        std::cout << "doNoise: " << (render_settings.doNoise ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_2){
        render_settings.doRefracted = !render_settings.doRefracted;
        std::cout << "doRefracted: " << (render_settings.doRefracted ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_3){
        render_settings.doReflected = !render_settings.doReflected;
        std::cout << "doReflected: " << (render_settings.doReflected ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_4){
        render_settings.doSpecular = !render_settings.doSpecular;
        std::cout << "doSpecular: " << (render_settings.doSpecular ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_5){
        render_settings.doRenderNormals = !render_settings.doRenderNormals;
        std::cout << "doRenderNormals: " << (render_settings.doRenderNormals ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_6){
        render_settings.doCurvatureFlow = !render_settings.doCurvatureFlow;
        std::cout << "doCurvatureFlow: " << (render_settings.doCurvatureFlow ? "on" : "off") << std::endl;
    }

    if(key == GLFW_KEY_P){
        render_settings.preRender = !render_settings.preRender;
    }
}

void SSFRenderer::onKeyReleased(int key)
{

}

void SSFRenderer::resetPreRender()
{
    frame_number = 0;
    capped = num_frames;
    enter_pressed = false;
}

bool SSFRenderer::renderingStills()
{
    return frame_number >= capped && render_settings.preRender;
}

void SSFRenderer::renderFinalToScreen(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj)
{
    if(render_settings.preRender){
        m_preRenderFBO.replaceColorAttachmentTexture(preRender[frame_number],0);
        m_preRenderFBO.Bind();
        frame_number++;
        if(frame_number%10==0){
            std::cout << "frame number " << frame_number << " out of " << num_frames << std::endl;
        }
    }else{
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }
    //glClearColor(.8,.8,.8,1);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glClearColor(0,0,0,0);


    glUseProgram(m_quadThicknessShader);
    glActiveTexture(GL_TEXTURE0);
    if(render_settings.doCurvatureFlow){
        glBindTexture(GL_TEXTURE_2D,m_cfFBO1.getDepthTexture());
    }else{
        glBindTexture(GL_TEXTURE_2D,m_depthFBO.getDepthTexture());
    }

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D,m_blurVertFBO.getColorTexture(0));

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE2);
    glBindTexture(GL_TEXTURE_2D,m_sceneFBO.getColorTexture(0));

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE3);
    glBindTexture(GL_TEXTURE_2D,m_sceneFBO.getDepthTexture());

    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_CLAMP_TO_EDGE);

    glActiveTexture(GL_TEXTURE0);

    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"tex"),0);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"thickness"),1);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"scene_color"),2);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"scene_depth"),3);


    //settings
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"doNoise"),render_settings.doNoise);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"doRefracted"),render_settings.doRefracted);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"doReflected"),render_settings.doReflected);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"doSpecular"),render_settings.doSpecular);
    glUniform1i(glGetUniformLocation(m_quadThicknessShader,"doRenderNormals"),render_settings.doRenderNormals);




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

    if(render_settings.preRender){
        m_preRenderFBO.replaceColorAttachmentTexture(0,0);
        glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    }

}

void SSFRenderer::doDepthPass(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius)
{
    m_depthFBO.Bind();
    shader.bind();
    shader.updateBindings("position", positions);
    shader.setUniform("mv", mv);
    shader.setUniform("proj", proj);
    shader.setUniform("particleRadius", particleRadius);
    glEnable(GL_DEPTH_TEST);
    shader.draw(GL_POINTS, 0, positions.cols(), 1);
    m_depthFBO.unBind();
}

void SSFRenderer::doThicknessPass(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius)
{
    m_thicknessFBO.Bind();
    m_thicknessShader.bind();
    m_thicknessShader.updateBindings("position", positions);
    m_thicknessShader.setUniform("mv", mv);
    m_thicknessShader.setUniform("proj", proj);
    m_thicknessShader.setUniform("particleRadius", particleRadius);
    glUniform2f(glGetUniformLocation(m_thicknessShader.m_program,"screensize"),m_size.x(),m_size.y());

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D,m_depthFBO.getDepthTexture());

    glUniform1i(glGetUniformLocation(m_thicknessShader.m_program,"depthTexture"),0);
    glDisable(GL_DEPTH_TEST);
    glEnable(GL_BLEND);
    glBlendFunc(GL_ONE,GL_ONE);
    m_thicknessShader.draw(GL_POINTS, 0, positions.cols(), 1);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);
    m_thicknessFBO.unBind();
}

void SSFRenderer::doBlurThickness()
{
    m_blurHorizFBO.Bind();
    glUseProgram(m_blurShader);
    glBindTexture(GL_TEXTURE_2D,m_thicknessFBO.getColorTexture(0));
    glUniform1i(glGetUniformLocation(m_blurShader,"horiz"),1);
    m_quad->draw();

    m_blurVertFBO.Bind();
    glBindTexture(GL_TEXTURE_2D,m_blurHorizFBO.getColorTexture(0));
    glUniform1i(glGetUniformLocation(m_blurShader,"horiz"),0);
    m_quad->draw();
}

void SSFRenderer::doCurvatureFlow(const Eigen::Matrix4f &proj)
{
    glUseProgram(m_curvatureFlowShader);
    glUniform2f(glGetUniformLocation(m_curvatureFlowShader,"screen_size"),m_size.x(),m_size.y());
    glUniformMatrix4fv(glGetUniformLocation(m_curvatureFlowShader,"p"),1,GL_FALSE,proj.data());

    m_cfFBO1.Bind();
    glBindTexture(GL_TEXTURE_2D,m_depthFBO.getDepthTexture());
    m_quad->draw();

    bool use_first = false;

    for(int i=0;i<60;i++){
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
}
void SSFRenderer::draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius) {
    //depthpass
    if(frame_number >= capped && render_settings.preRender){
        drawPreRendered();
        return;
    }
    doDepthPass(mv,proj,positions,particleRadius);
    doThicknessPass(mv,proj,positions,particleRadius);
    doBlurThickness();
    doCurvatureFlow(proj);
//m_depthFBO,m_blurVertFBO,m_blurHorizFBO,m_cfFBO1,m_cfFBO2,m_thicknessFBO,m_noiseFBO,m_sceneFBO
    //m_cfFBO1.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);
    if(renderType==0){
        renderFinalToScreen(mv,proj);
    }else if(renderType==1){
        m_depthFBO.renderTextureToFullScreen(0,true,1,proj,mv,m_quad.get(),near,far);



    }else if(renderType==2){
        m_thicknessFBO.renderTextureToFullScreen(0,false,2,proj,mv,m_quad.get(),near,far);


    }else if(renderType==3){
        m_thicknessFBO.renderTextureToFullScreen(0,false,3,proj,mv,m_quad.get(),near,far);

    }else if(renderType==4){
        m_blurVertFBO.renderTextureToFullScreen(0,false,2,proj,mv,m_quad.get(),near,far);

    }else if(renderType==5){
        m_cfFBO1.renderTextureToFullScreen(0,true,1,proj,mv,m_quad.get(),near,far);
    }else if(renderType==6){
       // m_depthFBO.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);
    }else if(renderType==7){
       // m_depthFBO.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);
    }else if(renderType==8){
       // m_depthFBO.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);
    }else if(renderType==9){
       // m_depthFBO.renderTextureToFullScreen(0,true,proj,mv,m_quad.get(),near,far);
    }





    //m_blurVertFBO.renderTextureToFullScreen(0,false,proj,mv,m_quad.get(),near,far);


    //m_thicknessFBO.renderTextureToFullScreen(0,false,proj,mv,m_quad.get(),near,far);
    //m_sceneFBO.renderTextureToFullScreen(0,false,proj,mv,m_quad.get(),near,far);

}

void SSFRenderer::drawPreRendered()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    GLuint texid = preRender[frame_number%capped];
    m_preRenderFBO.renderSpecificTextureToFullScreen(texid,m_quad.get());
    frame_number++;
}

void SSFRenderer::drawPreRenderedAsYouGo()
{
    glBindFramebuffer(GL_DRAW_FRAMEBUFFER,0);
    GLuint texid = preRender[(frame_number-1)%capped];
    m_preRenderFBO.renderSpecificTextureToFullScreen(texid,m_quad.get());
}

void SSFRenderer::drawQuad(const Eigen::Matrix4f &v,const Eigen::Matrix4f &p, const Box3f &box)
{

    Eigen::AngleAxis<float> aa(1.5707,Eigen::Vector3f(1,0,0));
    Eigen::Affine3f rot = Eigen::Affine3f(aa);

    float rangex = box.max.x() - box.min.x();
    float rangez = box.max.z() - box.min.z();
    float centerx = (box.max.x() + box.min.x())/2.f;
    float centerz = (box.max.z() + box.min.z())/2.f;

    Eigen::Affine3f scale(Eigen::Scaling(rangex/2.f,1.f,rangez/2.f));
    Eigen::Affine3f trans(Eigen::Translation3f(centerx,0,centerz));

    Eigen::Matrix4f m = (trans*scale*rot).matrix();

    glUseProgram(m_quadTransShader);
    glUniformMatrix4fv(glGetUniformLocation(m_quadTransShader,"p"),1,GL_FALSE,p.data());
    glUniformMatrix4fv(glGetUniformLocation(m_quadTransShader,"v"),1,GL_FALSE,v.data());
    glUniformMatrix4fv(glGetUniformLocation(m_quadTransShader,"m"),1,GL_FALSE,m.data());
    m_quad->draw();


}

}




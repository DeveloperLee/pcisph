#pragma once

#include <utils/Def.h>

#include "PCIShader.h"
#include "FBO.h"
#include "utils/ResourceLoader.h"

class Shape;

namespace cs224 {

struct RenderSettings {
    bool doNoise = true;
    bool doRefracted = true;
    bool doReflected = true;
    bool doSpecular = true;
    bool doRenderNormals = false;
    bool doCurvatureFlow = true;
};

struct SSFRenderer {
    SSFRenderer(const Vector2i &size);
    void prepareToDrawScene();
    void draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius = 0.03f);

    void onKeyPress(int key);

    void onKeyReleased(int key);

private:

    void renderFinalToScreen(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj);
    void doDepthPass(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius);
    void doThicknessPass(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, float particleRadius);
    void doBlurThickness();
    void doCurvatureFlow(const Eigen::Matrix4f &proj);


    FBO m_depthFBO,m_blurVertFBO,m_blurHorizFBO,m_cfFBO1,m_cfFBO2,m_thicknessFBO,m_noiseFBO,m_sceneFBO;
    PCIShader shader,m_thicknessShader, m_noiseShader;
    GLuint m_blurShader,m_curvatureFlowShader,m_quadThicknessShader;
    float near = .05;
    float far = 1.2f;
    int renderType=0;
    std::unique_ptr<Shape> m_quad;
    Eigen::Vector2i m_size;
    RenderSettings render_settings;

};

} // namespace cs224

#pragma once

#include <utils/Def.h>

#include "PCIShader.h"
#include "FBO.h"
#include "utils/ResourceLoader.h"

class Shape;

namespace cs224 {

struct SSFRenderer {
    SSFRenderer();
    void draw(const Eigen::Matrix4f &mv, const Eigen::Matrix4f &proj, const MatrixXf &positions, const Eigen::Vector4f &color, float particleRadius = 0.03f);

    void onKeyPress(int key);

    void onKeyReleased(int key);

private:
    FBO m_depthFBO,m_blurVertFBO,m_blurHorizFBO,m_cfFBO1,m_cfFBO2,m_thicknessFBO;
    PCIShader shader,m_thicknessShader;
    GLuint m_blurShader,m_curvatureFlowShader,m_quadThicknessShader;
    float near = .05;
    float far = 1.2f;
    std::unique_ptr<Shape> m_quad;
};

} // namespace cs224

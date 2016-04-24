#pragma once

#include "GL/glew.h"

#include <utils/STD.h>
#include <visualization/scene/Scene.h>
#include <algorithm/SPH.h>
#include <QGLWidget>
#include <QTimer>

#include "json11/json11.h"


#ifdef __APPLE__
#include <glu.h>
#else
#include <GL/glu.h>
#endif


//=========================================================
// Forward declarations
//=========================================================

namespace cs224 {

class DomainShader;
class MeshShader;
class ParticleShader;
class ParticleSphereShader;
class ParticleNormalShader;

namespace CameraUtils {
class Arcball;
} // namespace CameraUtils

}


//=========================================================
// class GLWidget --- Engine of Algorithm
// @ Camera
// @ UI Interaction
// @ File IO
// @ SPH simulation
//=========================================================

class GLWidget : public QGLWidget {

    Q_OBJECT

public:
    GLWidget(QGLFormat format, QWidget *parent = 0);
    ~GLWidget();

    // scene
    void loadScene(const std::string &path, const json11::Json &settings = json11::Json());

    // camera mvp
    void buildMVP();
    Eigen::Matrix4f viewMatrix() const;
    Eigen::Matrix4f projectionMatrix() const;

protected:
    void initializeGL();
    void paintGL();
    void resizeGL(int w, int h);
    void mousePressEvent(QMouseEvent *e);
    void mouseMoveEvent(QMouseEvent *e);
    void wheelEvent(QWheelEvent *e);

private:
    void refresh();

    // camera parameters
    Eigen::Vector2i m_viewPort;
    Eigen::Vector3f m_position;
    Eigen::Vector3f m_target;
    Eigen::Vector3f m_up;
    Eigen::Matrix4f m_view, m_proj;

    float m_fov;
    float m_near;
    float m_far;

    // scene
    cs224::Scene m_scene;

    // simulation
    std::unique_ptr<cs224::SPH> m_sph;
    QTimer m_timer;

    // shader programs
    std::unique_ptr<cs224::DomainShader> m_domainShader;
    std::unique_ptr<cs224::ParticleSphereShader> m_particleShader;
    std::unique_ptr<cs224::ParticleNormalShader> m_particleNormalShader;
    std::vector<std::unique_ptr<cs224::MeshShader>> m_boundaryMeshShaders;

    // For mouse interaction
    float m_angleX, m_angleY, m_zoom;
    cs224::CameraUtils::Arcball *m_arcball;

    bool m_leftButton = false;
};










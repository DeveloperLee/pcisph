#include "glwidget.h"

#include <QMouseEvent>
#include <QWheelEvent>
#include <iostream>

#include <utils/Math.h>
#include <utils/Settings.h>
#include <utils/CameraUtils.h>

#include "shading/DomainShader.h"
#include "shading/MeshShader.h"
#include "shading/ParticleShader.h"
#include "shading/ParticleSphereShader.h"
#include "shading/ParticleNormalShader.h"


//=========================================================
// constructor & destructor
//=========================================================

GLWidget::GLWidget(QGLFormat format, QWidget *parent) :

    QGLWidget(format, parent),
    m_angleX(0),
    m_angleY(0.5f),
    m_zoom(10.f) {

    m_viewPort = Eigen::Vector2i(1280, 720);

    // start timer
    connect(&m_timer, SIGNAL(timeout()), this, SLOT(refresh()));
    m_timer.start(1000.f / 10.f);   // fps: 10
}

GLWidget::~GLWidget() {

    delete m_arcball;
}


//=========================================================
// public
//=========================================================

void GLWidget::buildMVP() {

    float height = std::tan(m_fov / 360.f * PI) * m_near;
    float width = height * static_cast<float>(m_viewPort.x()) / static_cast<float>(m_viewPort.y());

    m_view = cs224::CameraUtils::lookAt(m_position, m_target, m_up);
    m_proj = cs224::CameraUtils::frustum(-width, width, -height, height, m_near, m_far);
}

void GLWidget::loadScene(const std::string &path, const json11::Json &settings) {

    m_scene = cs224::Scene::load(path, settings);

    m_position = m_scene.camera.position;
    m_target   = m_scene.camera.target;
    m_up       = m_scene.camera.up;
    m_fov      = m_scene.camera.fov;
    m_near     = m_scene.camera.near;
    m_far      = m_scene.camera.far;

    buildMVP();

    m_sph.reset(new cs224::SPH(m_scene));

    m_boundaryMeshShaders.clear();
    for (const auto &mesh : m_sph->getBoundaryMeshes()) {
        m_boundaryMeshShaders.emplace_back(new cs224::MeshShader(mesh));
    }
}



//=========================================================
// private
//=========================================================

void GLWidget::initializeGL() {

    // init glew
    glewExperimental = GL_TRUE;
    GLenum err = glewInit();
    if (GLEW_OK != err) assert(0 && "Failed to init GLEW");

    // set view port
    resizeGL(width(), height());

    glEnable(GL_DEPTH_TEST | GL_CULL_FACE);

    // Set the color to set the screen when the color buffer is cleared.
    glClearColor(0.f, 0.f, 0.f, 0.f);

    // init shaders
    m_domainShader.reset(new cs224::DomainShader());
    m_particleShader.reset(new cs224::ParticleSphereShader());
    m_particleNormalShader.reset(new cs224::ParticleNormalShader());
    m_arcball = new cs224::CameraUtils::Arcball();

    // load scene
    loadScene("/Users/LOVEME/Documents/Course-Projects/cs224/projects/pcisph/scenes/test.json");

    // kick off the simulation thread

}

void GLWidget::paintGL() {

    // clear opengl buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    if (!m_sph) return;

    Eigen::Matrix4f mvp = m_proj * m_view;

    //============ draw domain ================//
    if (cs224::ui_settings.renderDomain) {
        m_domainShader->draw(mvp, m_sph->getBounds());
    }

    //======== draw fluid particles ===========//
    if (cs224::ui_settings.renderParticleSphere) {
        float radius = m_sph->getParticleParams().radius * 2.f;
        m_particleShader->draw(m_view, m_proj, cs224::toMatrix(m_sph->getFluidPositions()), Eigen::Vector4f(0.25f, 0.54f, 0.87f, 1.f), radius);
    }

//    //======== draw boundary particles ========//
//    if (cs224::ui_settings.renderBoundarySphere) {
//        float radius = m_sph->getParticleParams().radius * 2.f;
//        m_particleShader->draw(m_view, m_proj, cs224::toMatrix(m_sph->getBoundaryPositions()), Eigen::Vector4f(0.5f, 0.32f, 0.01f, 1.f), radius);
//    }

//    //======== draw boundary meshes ===========//
//    if (cs224::ui_settings.renderBoundaryMesh) {
//        for (const auto &shader : m_boundaryMeshShaders) {
//            shader->draw(mvp, Eigen::Vector4f(0.5f, 0.32f, 0.01f, 1.f));
//        }
//    }
}

void GLWidget::resizeGL(int w, int h) {

    glViewport(0, 0, w, h);
}

void GLWidget::mousePressEvent(QMouseEvent *event) {

    Eigen::Vector2i p = Eigen::Vector2i(event->pos().x(), event->pos().y());

    if (event->button() == Qt::LeftButton) {
        if (event->type() == QEvent::MouseButtonPress) m_leftButton = true;
        else m_leftButton = false;

        m_arcball->setSize(m_viewPort);
        m_arcball->setState(Eigen::Quaternionf(m_view.block<3, 3>(0, 0)));
        m_arcball->button(p, m_leftButton);
    }

    update();
}

void GLWidget::mouseMoveEvent(QMouseEvent *event) {

    Eigen::Vector2i p = Eigen::Vector2i(event->pos().x(), event->pos().y());

    m_arcball->motion(p);

    if (m_leftButton) {
        Eigen::Matrix4f view = m_arcball->matrix();
        float distance = (m_position - m_target).norm();
        Eigen::Vector3f direction = view.block<3, 3>(0, 0).transpose() * Eigen::Vector3f(0.f, 0.f, 1.f);
        m_position = m_target + direction * distance;
        m_up = view.block<1, 3>(1, 0);
        buildMVP();
    }

    update();
}

void GLWidget::wheelEvent(QWheelEvent *event) {

    float distance = (m_position - m_target).norm();
    distance = cs224::clamp(distance - event->delta() * distance * 0.001f, 0.01f, 10000.f);
    m_position = m_target + (m_position - m_target).normalized() * distance;
    buildMVP();

    update();
}

void GLWidget::refresh() {
//    if (m_sph) m_sph->simulate();
}

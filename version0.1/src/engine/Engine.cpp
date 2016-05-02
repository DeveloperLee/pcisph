#include "Engine.h"
#include "visualization/scene/Scene.h"


namespace cs224 {

static inline MatrixXf toMatrix(const std::vector<Vector3f> &data) {
    MatrixXf result;
    result.resize(3, data.size());
    for (size_t i = 0; i < data.size(); ++i) {
        result.col(i) = data[i];
    }
    return std::move(result);
}

Engine::Engine()  {

}

Engine::Engine(const Vector2i &size, const Vector2i &renderSize) :
    m_size(size),
    m_renderSize(renderSize) {

    initShaders();
}

void Engine::setResolution(const Vector2i &size, const Vector2i &renderSize) {

    m_size = size;
    m_renderSize = renderSize;
}

void Engine::initShaders() {

    m_domainShader.reset(new DomainShader());
    m_particleShader.reset(new ParticleShader());
    m_fluidMeshShader.reset(new MeshShader());
    m_SSFRenderer.reset(new SSFRenderer(m_size));

}

void Engine::loadScene(const std::string &path, const json11::Json &settings) {
    //PRINT("Loading scene from '%s' ...", path.str());
    m_scene = Scene::load(path, settings);
    //PRINT("%s", m_scene.toString());

    m_camera.setResolution(m_size);
    m_camera.setPosition(m_scene.camera.position);
    m_camera.setTarget(m_scene.camera.target);
    m_camera.setUpVector(m_scene.camera.up);
    m_camera.setFov(m_scene.camera.fov);
    m_camera.setNear(m_scene.camera.near);
    m_camera.setFar(m_scene.camera.far);

    m_sph.reset(new SPH(m_scene));

    m_boundaryMeshShader.clear();
    for (const auto &mesh : m_sph->getBoundaryMeshes()) {
        m_boundaryMeshShader.emplace_back(new MeshShader(mesh));
    }
}

void Engine::onKeyPress(int key)
{
    if (key == GLFW_KEY_SPACE) {
        simulate = !simulate;
    }
    if (key == GLFW_KEY_0) {
        renderMode = !renderMode;
    }

    m_keys.insert(key);
    m_SSFRenderer->onKeyPress(key);
}

void Engine::onKeyReleased(int key)
{
    m_keys.erase(key);
    m_SSFRenderer->onKeyReleased(key);
}

void Engine::onMouseMove(double deltaX, double deltaY)
{
    Eigen::Vector3f look = m_camera.target()-m_camera.position();
    Eigen::Vector3f perp(look.z(),0,-look.x());

    Eigen::AngleAxis<float> aa(-deltaX/1000.0,Eigen::Vector3f(0,1,0));
    Eigen::Affine3f rot = Eigen::Affine3f(aa);
    look = rot * look;
    perp.normalize();
    Eigen::AngleAxis<float> aay(deltaY/1000.0,perp);
    Eigen::Affine3f roty = Eigen::Affine3f(aay);
    look = roty * look;

    m_camera.setTarget(look + m_camera.position());
}

void Engine::updateStep() {
    if(simulate){
        m_sph->simulate();
    }

    //handle input
    Eigen::Vector3f look = m_camera.target()-m_camera.position();
    look.normalize();
    Eigen::Vector3f perp(look.z(),0,-look.x());
    Eigen::Vector3f up(0,1,0);
    perp *= .01;
    look *= .01;
    up *= .01;

    if(m_keys.find(GLFW_KEY_A) != m_keys.end()){
        m_camera.setPosition(m_camera.position()+perp);
        m_camera.setTarget(m_camera.target()+perp);
    }
    if(m_keys.find(GLFW_KEY_D) != m_keys.end()){
        m_camera.setPosition(m_camera.position()-perp);
        m_camera.setTarget(m_camera.target()-perp);
    }
    if(m_keys.find(GLFW_KEY_W) != m_keys.end()){
        m_camera.setPosition(m_camera.position()+look);
        m_camera.setTarget(m_camera.target()+look);
    }
    if(m_keys.find(GLFW_KEY_S) != m_keys.end()){
        m_camera.setPosition(m_camera.position()-look);
        m_camera.setTarget(m_camera.target()-look);
    }

    if(m_keys.find(GLFW_KEY_Q) != m_keys.end()){
        m_camera.setPosition(m_camera.position()+up);
        m_camera.setTarget(m_camera.target()+up);
    }

    if(m_keys.find(GLFW_KEY_E) != m_keys.end()){
        m_camera.setPosition(m_camera.position()-up);
        m_camera.setTarget(m_camera.target()-up);
    }

}

void Engine::render() {

    if (!m_sph) return;

    Eigen::Matrix4f view = m_camera.viewMatrix();
    Eigen::Matrix4f proj = m_camera.projectionMatrix();
    float particleRadius = m_sph->getParticleParams().radius;

    if(renderMode){
        Eigen::Matrix4f mv = view;
        Eigen::Matrix4f mvp = proj * view;



        // Draw world bounding box
        m_domainShader->draw(mvp, m_sph->getBounds());

        // Draw particle spheres
        m_particleShader->draw(view, proj, cs224::toMatrix(m_sph->getFluidPositions()),Eigen::Vector4f(0.8f, 0.54f, 0.54f, 1.f), particleRadius * 2);

        // Draw boundary meshes
        for (const auto &shader : m_boundaryMeshShader) {
            shader->draw(mvp, Eigen::Vector4f(0.32f, 0.32f, 0.81f, 1.f));
        }
    }else{
        Eigen::Matrix4f mv = view;
        Eigen::Matrix4f mvp = proj * view;


        m_SSFRenderer->prepareToDrawScene();
        // Draw world bounding box
        m_domainShader->draw(mvp, m_sph->getBounds());

        // Draw boundary meshes
        for (const auto &shader : m_boundaryMeshShader) {
            shader->draw(mvp, Eigen::Vector4f(0.32f, 0.32f, 0.81f, 1.f));
        }

        m_SSFRenderer->draw(view, proj, cs224::toMatrix(m_sph->getFluidPositions()),particleRadius * 2);
    }
}

} // namespace cs224

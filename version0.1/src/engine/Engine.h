#pragma once

#include "algorithm/SPH.h"
#include "utils/Def.h"
#include "./camera/Camera.h"
#include "shading/DomainShader.h"
#include "shading/ParticleShader.h"
#include "shading/MeshShader.h"

#include <json11.h>
#include <memory>

namespace cs224 {

class Engine {
public:
    struct ViewOptions {
        bool showDomain = true;
        bool showFluidParticles = true;
        bool showFluidMesh = false;
        bool showBoundaryParticles = false;
        bool showBoundaryMeshes = true;
    };

    Engine();

    Engine(const Vector2i &size, const Vector2i &renderSize);
    Camera &camera()       { return m_camera; }
    ViewOptions &viewOptions()       { return m_viewOptions; }

    const Scene &scene() const { return m_scene; }
    const SPH &sph() const { return *m_sph; }
    
    void setResolution(const Vector2i &size, const Vector2i &renderSize);
    void loadScene(const std::string &path, const json11::Json &settings = json11::Json());

    void updateStep();

    void render();
    void initShaders();
private:

    Vector2i m_size;
    Vector2i m_renderSize;

    Camera m_camera;
    Scene m_scene;
    std::unique_ptr<SPH> m_sph;

    ViewOptions m_viewOptions;

    std::unique_ptr<DomainShader> m_domainShader;
    std::unique_ptr<ParticleShader> m_particleShader;
    std::unique_ptr<MeshShader> m_fluidMeshShader;
    std::vector<std::unique_ptr<MeshShader>> m_boundaryMeshShader;
};

} // namespace cs224

#include "Viewer.h"

#include "basics/Common.h"
#include "basics/Timer.h"
#include "basics/DebugMonitor.h"
#include "geo/ObjReader.h"
#include "geo/ParticleMesher.h"
#include "render/Camera.h"
#include "render/Painter.h"
#include "algorithm/SPH.h"

#include "Config.h"

#include <nanogui/checkbox.h>
#include <nanogui/combobox.h>
#include <nanogui/glutil.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>
#include <nanogui/window.h>

#include <filesystem/path.h>

#include <memory>
#include <chrono>

namespace cs224 {

Viewer::Viewer(const ViewerSettings &settings) :
    Screen(nanogui::Vector2i(1280, 720), "Fluid Simulator"),
    _engine(mNVGContext, mSize, mSize)
{
    initializeGUI();
    refreshGUI();

    loadScene(settings.filename);
}

Viewer::~Viewer() {
}

bool Viewer::mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) {
    if (!Screen::mouseMotionEvent(p, rel, button, modifiers)) {
        _engine.camera().mouseMotionEvent(p, rel, button, modifiers);
    }
    return true;
}

bool Viewer::mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers) {
    if (!Screen::mouseButtonEvent(p, button, down, modifiers)) {
        _engine.camera().mouseButtonEvent(p, button, down, modifiers);
    }
    return true;
}

bool Viewer::scrollEvent(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) {
    if (!Screen::scrollEvent(p, rel)) {
        _engine.camera().scrollEvent(p, rel);
    }
    return true;
}

bool Viewer::keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (!Screen::keyboardEvent(key, scancode, action, modifiers)) {
        if (action == GLFW_PRESS) {
            switch (key) {
            case GLFW_KEY_ESCAPE:
                setVisible(false);
                break;
            case GLFW_KEY_TAB:
                _window->setVisible(!_window->visible());
                break;
            case GLFW_KEY_HOME:
                //_sph->reset();
                break;
            case GLFW_KEY_SPACE:
                _isRunning = !_isRunning;
                break;
            case GLFW_KEY_G:
                _engine.viewOptions().showDomain = !_engine.viewOptions().showDomain;
                break;
            case GLFW_KEY_F:
                if (modifiers & GLFW_MOD_SHIFT) {
                    _engine.viewOptions().showFluidMesh = !_engine.viewOptions().showFluidMesh;
                } else {
                    _engine.viewOptions().showFluidParticles = !_engine.viewOptions().showFluidParticles;
                }
                break;
            case GLFW_KEY_B:
                if (modifiers & GLFW_MOD_SHIFT) {
                    _engine.viewOptions().showBoundaryMeshes = !_engine.viewOptions().showBoundaryMeshes;
                } else {
                    _engine.viewOptions().showBoundaryParticles = !_engine.viewOptions().showBoundaryParticles;
                }
                break;
            case GLFW_KEY_D:
                _engine.viewOptions().showDebug = !_engine.viewOptions().showDebug;
                break;
            }
        }
        refreshGUI();
    }
    return true;
}

void Viewer::drawContents() {
    if (_isRunning) {
        _engine.updateStep();
        glfwPostEmptyEvent();
    }

    _engine.render();
}

void Viewer::initializeGUI() {
    _window = new nanogui::Window(this, "Settings");
    _window->setPosition(Vector2i(15, 15));
    _window->setLayout(new nanogui::GroupLayout());

    new nanogui::Label(_window, "Actions", "sans-bold");
    nanogui::Button *createMeshButton = new nanogui::Button(_window, "Create Mesh");
    createMeshButton->setCallback([&] () { createMesh(); refreshGUI(); });
    nanogui::Button *clearMeshButton = new nanogui::Button(_window, "Clear Mesh");
    clearMeshButton->setCallback([&] () { clearMesh(); refreshGUI(); });
    nanogui::Button *cameraJsonButton = new nanogui::Button(_window, "Camera JSON");
    cameraJsonButton->setCallback([&] () { createCameraJson(); refreshGUI(); });

    new nanogui::Label(_window, "Display", "sans-bold");
    _showDomainCheckBox = new nanogui::CheckBox(_window, "Show Domain (G)");
    _showDomainCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showDomain = b; refreshGUI(); });
    _showFluidParticlesCheckBox = new nanogui::CheckBox(_window, "Show Fluid Particles (F)");
    _showFluidParticlesCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showFluidParticles = b; refreshGUI(); });
    _showFluidMeshCheckBox = new nanogui::CheckBox(_window, "Show Fluid Mesh (Shift+F)");
    _showFluidMeshCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showFluidMesh = b; refreshGUI(); });
    _showBoundaryParticlesCheckBox = new nanogui::CheckBox(_window, "Show Boundary Particles (B)");
    _showBoundaryParticlesCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showBoundaryParticles = b; refreshGUI(); });
    _showBoundaryMeshesCheckBox = new nanogui::CheckBox(_window, "Show Boundary Meshes (Shift+B)");
    _showBoundaryMeshesCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showBoundaryMeshes = b; refreshGUI(); });
    _showDebugCheckBox = new nanogui::CheckBox(_window, "Show Debug (D)");
    _showDebugCheckBox->setCallback([&] (bool b) { _engine.viewOptions().showDebug = b; refreshGUI(); });

    performLayout(mNVGContext);
    setVisible(true);
}

void Viewer::refreshGUI() {
    _showDomainCheckBox->setChecked(_engine.viewOptions().showDomain);
    _showFluidParticlesCheckBox->setChecked(_engine.viewOptions().showFluidParticles);
    _showFluidMeshCheckBox->setChecked(_engine.viewOptions().showFluidMesh);
    _showBoundaryParticlesCheckBox->setChecked(_engine.viewOptions().showBoundaryParticles);
    _showBoundaryMeshesCheckBox->setChecked(_engine.viewOptions().showBoundaryMeshes);
    _showDebugCheckBox->setChecked(_engine.viewOptions().showDebug);
}

void Viewer::createMesh() {
    _engine.createFluidMesh();
}

void Viewer::clearMesh() {
    _engine.clearFluidMesh();
}

void Viewer::createCameraJson() {
    auto vector3toJson = [&] (const Vector3f &v) {
        return json11::Json(json11::Json::array({
            json11::Json(v.x()),
            json11::Json(v.y()),
            json11::Json(v.z()) })
        );
    };

    const auto &camera = _engine.camera();

    json11::Json::object cameraJson;
    cameraJson["position"] = vector3toJson(camera.position());
    cameraJson["target"] = vector3toJson(camera.target());
    cameraJson["up"] = vector3toJson(camera.up());
    cameraJson["fov"] = json11::Json(camera.fov());
    cameraJson["near"] = json11::Json(camera.near());
    cameraJson["far"] = json11::Json(camera.far());

}

void Viewer::loadScene(const filesystem::path &path) {
    _engine.loadScene(path);
}

}

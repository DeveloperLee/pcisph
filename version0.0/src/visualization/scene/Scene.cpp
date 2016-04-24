#include "Scene.h"

#include "utils/STD.h"
#include "utils/Vector.h"
#include "utils/Box.h"
#include "utils/StringUtils.h"

#include "json11/json11.h"

#include <fstream>
#include <sstream>

using namespace json11;

namespace cs224 {

Scene::Camera::Camera(const Settings &props) {
    position = props.getVector3("position", position);
    target = props.getVector3("target", target);
    up = props.getVector3("up", up);
    fov = props.getFloat("fov", fov);
    near = props.getFloat("near", near);
    far = props.getFloat("far", far);
    frame = props.getInteger("frame", frame);
}


Scene::World::World(const Settings &props) {
    bounds = props.getBox3("bounds", bounds);
}


Scene::Shape::Shape(const Settings &props) {
    type = typeFromString(props.getString("type", "fluid"));
}

Scene::Box::Box(const Settings &props) : Shape(props) {
    bounds = props.getBox3("bounds");
}


Scene::Sphere::Sphere(const Settings &props) : Shape(props) {
    position = props.getVector3("position");
    radius = props.getFloat("radius");
}


Scene::Mesh::Mesh(const Settings &props) : Shape(props) {
    filename = props.getString("filename");
}

Scene Scene::load(const std::string &filename, const json11::Json &settings) {
    
    std::ifstream is(filename);
    std::stringstream ss;
    ss << is.rdbuf();
    
    std::string err;
    Json jsonRoot = Json::parse(ss.str(), err);
    if (jsonRoot.is_null()) {
        std::cout<<"Failed to load the scene" <<std::endl;
    }
    Scene scene;
    // Patch settings
    auto settingsValues = jsonRoot["settings"].object_items();
    for (auto kv : settings.object_items()) {
        settingsValues[kv.first] = kv.second;
    }
    scene.settings = Settings(json11::Json(settingsValues));

    // Parse scene objects
    Json jsonScene = jsonRoot["scene"];
    if (jsonScene.is_object()) {
        auto jsonCamera = jsonScene["camera"];
        if (jsonCamera.is_object()) {
            Settings props(jsonCamera);
            scene.camera = Camera(jsonCamera);
        }
        auto jsonWorld = jsonScene["world"];
        if (jsonWorld.is_object()) {
            Settings props(jsonWorld);
            scene.world = World(jsonWorld);
        }
        for (auto jsonBox : jsonScene["boxes"].array_items()) {
            scene.boxes.emplace_back(Box(Settings(jsonBox)));
        }
        for (auto jsonSphere : jsonScene["spheres"].array_items()) {
            scene.spheres.emplace_back(Sphere(Settings(jsonSphere)));
        }
        for (auto jsonMesh : jsonScene["meshes"].array_items()) {
            scene.meshes.emplace_back(Mesh(Settings(jsonMesh)));
        }
        for (auto jsonCameraKeyframe : jsonScene["cameraKeyframes"].array_items()) {
            scene.cameraKeyframes.emplace_back(Camera(Settings(jsonCameraKeyframe)));
        }
        // Set default camera
        if (!jsonCamera.is_object()) {
            Vector3f center = scene.world.bounds.center();
            scene.camera.position += center;
            scene.camera.target += center;
        }
    }

    return scene;
}

template<typename T>
static std::string vectorToString(const std::vector<T> &items) {
    std::string result = "[";
    for (const auto &item : items) {
        result += "\n  " + item.toString();
    }
    result += items.empty() ? "]" : "\n]";
    return result;
}

Scene::Type Scene::typeFromString(const std::string &name) {
    if (name == "fluid") {
        return Fluid;
    } else if (name == "boundary") {
        return Boundary;
    } else {
       std::cout<<"Unknown type name"<<std::endl;
    }
}

std::string Scene::typeToString(Type type) {
    switch (type) {
    case Fluid: return "fluid";
    case Boundary: return "boundary";
    }
    return "unknown";
}

} 

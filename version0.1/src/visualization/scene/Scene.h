#pragma once

#include "utils/Def.h"
#include "utils/Settings.h"
#include "utils/StringUtils.h"

#include <string>
#include <map>
#include <vector>

namespace cs224 {

class Scene {
public:
    enum Type {
        Fluid,
        Boundary,
    };

    struct Camera {
        Vector3f position = Vector3f(0.f, 0.f, 5.f);
        Vector3f target = Vector3f(0.f);
        Vector3f up = Vector3f(0.f, 1.f, 0.f);
        float fov = 20.f;
        float near = 0.1f;
        float far = 100.f;
        int frame = 0;
        Camera() = default;
        Camera(const Settings &props);
        std::string toString() const;
    };

    struct World {
        Box3f bounds = Box3f(Vector3f(-1.f), Vector3f(1.f));
        World() = default;
        World(const Settings &props);
        std::string toString() const;
    };

    struct Shape {
        Type type;
        Shape(const Settings &props);
    };

    struct Box : public Shape {
        Box3f bounds;
        Box(const Settings &props);
        std::string toString() const;
    };

    struct Sphere : public Shape {
        Vector3f position;
        float radius;
        Sphere(const Settings &props);
        std::string toString() const;
    };

    struct Mesh : public Shape {
        std::string filename;
        Mesh(const Settings &props);
        std::string toString() const;
    };

    Settings settings;

    Camera camera;
    World world;
    std::vector<Box> boxes;
    std::vector<Sphere> spheres;
    std::vector<Mesh> meshes;
    std::vector<Camera> cameraKeyframes;

    static Scene load(const std::string &filename, const json11::Json &settings = json11::Json());

    std::string toString() const;

private:
    static Type typeFromString(const std::string &name);
    static std::string typeToString(Type type);
};


} 

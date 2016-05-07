#pragma once

#include "utils/Def.h"

namespace cs224 {
namespace SceneWidget{

static enum Type {
    Fluid,
    Boundary,
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

struct Camera {
    Vector3f position = Vector3f(0.f, 0.f, 5.f);
    Vector3f target = Vector3f(0.f);
    Vector3f up = Vector3f(0.f, 1.f, 0.f);
    float fov = 30.f;
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

} // namespace SceneWidget
} // namespace cs224
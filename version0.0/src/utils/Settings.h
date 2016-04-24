#pragma once

#include "STD.h"

#include "json11/json11.h"

#include <string>

namespace cs224 {
    
typedef std::string XMLString;

class Settings {
public:
    Settings() = default;
    Settings(const json11::Json &json);

    const json11::Json &json() const { return _json; }

    bool hasObject(const XMLString &name) const;
    Settings getObject(const XMLString &name) const;

    bool hasString(const XMLString &name) const;
    XMLString getString(const XMLString &name) const;
    XMLString getString(const XMLString &name, const XMLString &def) const;

    bool hasBool(const XMLString &name) const;
    bool getBool(const XMLString &name) const;
    bool getBool(const XMLString &name, bool def) const;

    bool hasFloat(const XMLString &name) const;
    float getFloat(const XMLString &name) const;
    float getFloat(const XMLString &name, float def) const;

    bool hasInteger(const XMLString &name) const;
    int getInteger(const XMLString &name) const;
    int getInteger(const XMLString &name, int def) const;

    bool hasVector2(const XMLString &name) const;
    Vector2f getVector2(const XMLString &name) const;
    Vector2f getVector2(const XMLString &name, const Vector2f &def) const;

    bool hasVector3(const XMLString &name) const;
    Vector3f getVector3(const XMLString &name) const;
    Vector3f getVector3(const XMLString &name, const Vector3f &def) const;

    bool hasBox3(const XMLString &name) const;
    Box3f getBox3(const XMLString &name) const;
    Box3f getBox3(const XMLString &name, const Box3f &def) const;

private:
    json11::Json _json;
};

struct UISettings{

    bool renderDomain = true;
    bool renderParticleSphere = true;
    bool renderParticleMesh = false;
    bool renderBoundarySphere = false;
    bool renderBoundaryMesh = true;
};

extern UISettings ui_settings;

} 

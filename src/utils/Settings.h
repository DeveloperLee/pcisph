// This class is responsible for retrieving data from a parsed xml file.

#pragma once

#include  "STD.h"
#include  <sting>

namespace cs224 {

typedef std::string XMLString;

class Settings {

public: 

	Settings();
	//Settings(XML...);
    float getFloat(const XMLString &key, float default) const;
    int   getInt(const XMLString &key, int default) const;
    bool  getBool(const XMLString &key, bool default) const;
    Vector2f getVector2f(const XMLString &key, Vector2f default) const;
    Vector3f getVector3f(const XMLString &key, Vector3f default) const;
    Box2f getBox2f(const XMLString &key, Box2f default) const;
    Box3f getBox3f(const XMLString &key, Box3f default) const;
    XMLString getString(const XMLString &key, XMLString default) const;

};
}
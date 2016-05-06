#include "Settings.h"
#include "Math.h"


using namespace json11;

namespace cs224 {

static bool isVector2(const Json &json) {
    return json.is_array() && json.array_items().size() == 2 && json[0].is_number() && json[1].is_number();
}

static bool isVector3(const Json &json) {
    return json.is_array() && json.array_items().size() == 3 && json[0].is_number() && json[1].is_number() && json[2].is_number();
}

static bool isBox2(const Json &json) {
    return json.is_array() && json.array_items().size() == 2 && isVector2(json[0]) && isVector2(json[1]);
}

static bool isBox3(const Json &json) {
    return json.is_array() && json.array_items().size() == 2 && isVector3(json[0]) && isVector3(json[1]);
}

static Vector2f parseVector2(const Json &json) {
    return Vector2f(json[0].number_value(), json[1].number_value());
}

static Vector3f parseVector3(const Json &json) {
    return Vector3f(json[0].number_value(), json[1].number_value(), json[2].number_value());
}


static Box3f parseBox3(const Json &json) {
    return Box3f(parseVector3(json[0]), parseVector3(json[1]));
}

Settings::Settings(const Json &json) :
    _json(json)
{}

bool Settings::hasObject(const XMLString &name) const {
    return _json[name].is_object();
}

Settings Settings::getObject(const XMLString &name) const {
    return Settings(_json[name]);
}

bool Settings::hasString(const XMLString &name) const {
    return _json[name].is_string();
}

XMLString Settings::getString(const XMLString &name) const {
  
    return _json[name].string_value();
}

XMLString Settings::getString(const XMLString &name, const XMLString &def) const {
    return hasString(name) ? _json[name].string_value() : def;
}

bool Settings::hasBool(const XMLString &name) const {
    return _json[name].is_bool();
}

bool Settings::getBool(const XMLString &name) const {
    
    return _json[name].bool_value();
}

bool Settings::getBool(const XMLString &name, bool def) const {
    return hasBool(name) ? _json[name].bool_value() : def;
}

bool Settings::hasFloat(const XMLString &name) const {
    return _json[name].is_number();
}

float Settings::getFloat(const XMLString &name) const {
   
    return _json[name].number_value();
}

float Settings::getFloat(const XMLString &name, float def) const {
    return hasFloat(name) ? _json[name].number_value() : def;
}

bool Settings::hasInteger(const XMLString &name) const {
    return _json[name].is_number();
}

int Settings::getInteger(const XMLString &name) const {
   
    return _json[name].int_value();
}

int Settings::getInteger(const XMLString &name, int def) const {
    return hasInteger(name) ? _json[name].int_value() : def;
}

bool Settings::hasVector2(const XMLString &name) const {
    return isVector2(_json[name]);
}

Vector2f Settings::getVector2(const XMLString &name) const {
   
    return parseVector2(_json[name]);
}

Vector2f Settings::getVector2(const XMLString &name, const Vector2f &def) const {
    return hasVector2(name) ? parseVector2(_json[name]) : def;
}

bool Settings::hasVector3(const XMLString &name) const {
    return isVector3(_json[name]);
}

Vector3f Settings::getVector3(const XMLString &name) const {
  
    return parseVector3(_json[name]);
}

Vector3f Settings::getVector3(const XMLString &name, const Vector3f &def) const {
    return hasVector3(name) ? parseVector3(_json[name]) : def;
}


bool Settings::hasBox3(const XMLString &name) const {
    return isBox3(_json[name]);
}

Box3f Settings::getBox3(const XMLString &name) const {
    return parseBox3(_json[name]);
}

Box3f Settings::getBox3(const XMLString &name, const Box3f &def) const {
    return hasBox3(name) ? parseBox3(_json[name]) : def;
}

} 

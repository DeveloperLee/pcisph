#pragma once

#include "utils/Def.h"
#include <Eigen/Geometry>

namespace cs224 {

static Eigen::Matrix4f lookat(const Eigen::Vector3f &eye, const Eigen::Vector3f &target, const Eigen::Vector3f &up) {

    Eigen::Vector3f f = (target - eye).normalized();
    Eigen::Vector3f s = f.cross(up).normalized();
    Eigen::Vector3f u = s.cross(f);

    Eigen::Matrix4f ret = Eigen::Matrix4f::Identity();

    ret(0, 0) = s(0);
    ret(0, 1) = s(1);
    ret(0, 2) = s(2);
    ret(1, 0) = u(0);
    ret(1, 1) = u(1);
    ret(1, 2) = u(2);
    ret(2, 0) = -f(0);
    ret(2, 1) = -f(1);
    ret(2, 2) = -f(2);
    ret(0, 3) = -s.transpose() * eye;
    ret(1, 3) = -u.transpose() * eye;
    ret(2, 3) = f.transpose() * eye;

    return ret;
}

static Eigen::Matrix4f frus(const float left, const float right, const float bottom,
                        const float top, const float nearVal, const float farVal) {

    Eigen::Matrix4f ret = Eigen::Matrix4f::Zero();

    ret(0, 0) = (2.0f * nearVal) / (right - left);
    ret(1, 1) = (2.0f * nearVal) / (top - bottom);
    ret(0, 2) = (right + left) / (right - left);
    ret(1, 2) = (top + bottom) / (top - bottom);
    ret(2, 2) = -(farVal + nearVal) / (farVal - nearVal);
    ret(3, 2) = -1.0f;
    ret(2, 3) = -(2.0f * farVal * nearVal) / (farVal - nearVal);

    return ret;
}

static Eigen::Matrix4f scale(const Eigen::Matrix4f &m, const Eigen::Vector3f &v) {

    Eigen::Matrix4f ret;

    ret.col(0) = m.col(0).array() * v(0);
    ret.col(1) = m.col(1).array() * v(1);
    ret.col(2) = m.col(2).array() * v(2);
    ret.col(3) = m.col(3);

    return ret;
}

static Eigen::Matrix4f translate(const Eigen::Matrix4f &m, const Eigen::Vector3f &v) {

    Eigen::Matrix4f ret = m;

    ret.col(3) = m.col(0).array() * v(0) + m.col(1).array() * v(1) +
                 m.col(2).array() * v(2) + m.col(3).array();

    return ret;
}

class Camball {
public:
    Camball(float speedFactor = 2.0f)
        : mActive(false), mLastPos(Eigen::Vector2i::Zero()), mSize(Eigen::Vector2i::Zero()),
          mQuat(Eigen::Quaternionf::Identity()),
          mIncr(Eigen::Quaternionf::Identity()),
          mSpeedFactor(speedFactor) { }

    Camball(const Eigen::Quaternionf &quat)
        : mActive(false), mLastPos(Eigen::Vector2i::Zero()), mSize(Eigen::Vector2i::Zero()),
          mQuat(quat),
          mIncr(Eigen::Quaternionf::Identity()),
          mSpeedFactor(2.0f) { }

    Eigen::Quaternionf &state() { return mQuat; }

    void setState(const Eigen::Quaternionf &state) {
        mActive = false;
        mLastPos = Eigen::Vector2i::Zero();
        mQuat = state;
        mIncr = Eigen::Quaternionf::Identity();
    }

    void setSize(Eigen::Vector2i size) { mSize = size; }
    const Eigen::Vector2i &size() const { return mSize; }
    void setSpeedFactor(float speedFactor) { mSpeedFactor = speedFactor; }
    float speedFactor() const { return mSpeedFactor; }
    bool active() const { return mActive; }

    void button(Eigen::Vector2i pos, bool pressed) {
        mActive = pressed;
        mLastPos = pos;
        if (!mActive) mQuat = (mIncr * mQuat).normalized();
        mIncr = Eigen::Quaternionf::Identity();
    }

    bool motion(Eigen::Vector2i pos) {
        if (!mActive) return false;

        float invMinDim = 1.0f / mSize.minCoeff();
        float w = (float) mSize.x(), h = (float) mSize.y();

        float ox = (mSpeedFactor * (2 * mLastPos.x() - w) + w) - w - 1.0f;
        float tx = (mSpeedFactor * (2 * pos.x()      - w) + w) - w - 1.0f;
        float oy = (mSpeedFactor * (h - 2 * mLastPos.y()) + h) - h - 1.0f;
        float ty = (mSpeedFactor * (h - 2 * pos.y())      + h) - h - 1.0f;

        ox *= invMinDim; oy *= invMinDim;
        tx *= invMinDim; ty *= invMinDim;

        Eigen::Vector3f v0(ox, oy, 1.0f), v1(tx, ty, 1.0f);
        if (v0.squaredNorm() > 1e-4f && v1.squaredNorm() > 1e-4f) {
            v0.normalize(); v1.normalize();
            Eigen::Vector3f axis = v0.cross(v1);
            float sa = std::sqrt(axis.dot(axis)),
                  ca = v0.dot(v1),
                  angle = std::atan2(sa, ca);
            if (tx*tx + ty*ty > 1.0f) angle *= 1.0f + 0.2f * (std::sqrt(tx*tx + ty*ty) - 1.0f);
            mIncr = Eigen::AngleAxisf(angle, axis.normalized());
            if (!std::isfinite(mIncr.norm())) mIncr = Eigen::Quaternionf::Identity();
        }
        return true;
    }

    Eigen::Matrix4f matrix() const {
        Eigen::Matrix4f result2 = Eigen::Matrix4f::Identity();
        result2.block<3,3>(0, 0) = (mIncr * mQuat).toRotationMatrix();
        return result2;
    }

protected:
    bool mActive;
    Eigen::Vector2i mLastPos;
    Eigen::Vector2i mSize;
    Eigen::Quaternionf mQuat, mIncr;
    float mSpeedFactor;
};
}
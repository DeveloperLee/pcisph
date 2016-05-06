#pragma once

#include "utils/Def.h"
#include "Camball.h"

namespace cs224 {


class Camera {
public:
	Camera();
	~Camera();

	void setResolution(const Eigen::Vector2i &resolution);
	void setPosition(const Eigen::Vector3f &position);
	void setTarget(const Eigen::Vector3f &target);
	void setUpVector(const Eigen::Vector3f &up);
	void setFov(float fov);
	void setNear(float near);
	void setFar(float far);

	const Eigen::Vector2i &resolution() const;
	const Eigen::Vector3f &position() const;
	const Eigen::Vector3f &target() const;
	const Eigen::Vector3f &up() const;

	float fov() const;
	float near() const;
	float far() const;

	Eigen::Matrix4f viewMatrix() const;
	Eigen::Matrix4f projectionMatrix() const;

	bool mouseMotionEvent(const Eigen::Vector2i &p, const Eigen::Vector2i &rel, int button, int modifiers);
	bool mouseButtonEvent(const Eigen::Vector2i &p, int button, bool down, int modifiers);
	bool scrollEvent(const Eigen::Vector2i &p, const Eigen::Vector2f &rel);

private:
	Eigen::Vector2i m_resolution;
	Eigen::Vector3f m_position;
	Eigen::Vector3f m_target;
	Eigen::Vector3f m_up;
	float m_fov;
	float m_near;
	float m_far;

	std::unique_ptr<Camball> m_camball;
	bool m_leftButton = false;
};



} // namespace cs224






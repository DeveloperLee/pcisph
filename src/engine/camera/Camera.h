#pragma once

#include <utils/STD.h>
#include <nanogui/glutil.h>


namespace cs224 {

class Camera {
public:
	Camera();
	~Camera();

	void setResolution(const nanogui::Vector2i &resolution);
	void setPosition(const nanogui::Vector3f &position);
	void setTarget(const nanogui::Vector3f &target);
	void setUpVector(const nanogui::Vector3f &up);
	void setFov(float fov);
	void setNear(float near);
	void setFar(float far);

	const nanogui::Vector2i &resolution() const;
	const nanogui::Vector3f &position() const;
	const nanogui::Vector3f &target() const;
	const nanogui::Vector3f &up() const;

	float fov() const;
	float near() const;
	float far() const;

	nanogui::Matrix4f viewMatrix() const;
	nanogui::Matrix4f projectionMatrix() const;

	bool mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers);
	bool mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers);
	bool scrollEvent(const nanogui::Vector2i &p, const nanogui::Vector2f &rel);

private:
	nanogui::Vector2i m_resolution;
	nanogui::Vector3f m_position;
	nanogui::Vector3f m_target;
	nanogui::Vector3f m_up;
	float m_fov;
	float m_near;
	float m_far;

	nanogui::Arcball m_arcball;
	bool m_leftButton = false;
};

} // namespace cs224






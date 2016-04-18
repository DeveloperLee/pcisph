
// Camera class
// Apr 17th, 2016

#include "Camera.h"
#include <utils/Math.h>


//============================================================
// public
//============================================================

namespace cs224 {

Camera::Camera() {

	// initialize camera parameters
	// Vector2i m_resolution;
	// Vector3f m_position;
	// Vector3f m_target;
	// Vector3f m_up;
	// float m_fov;
	// float m_near;
	// float m_far;

	// m_resolution = nanogui::Vector2i(1440, 900);
	// m_position = nanogui::Vector3f(1.5f, 1.f, 4.5f);
	// m_target = nanogui::Vector3f(0.f, 1.f, 0.f);
	// m_up = nanogui::Vector3f();
}

Camera::~Camera() {

}

void Camera::setResolution(const nanogui::Vector2i &resolution) {

	m_resolution = resolution;
}

void Camera::setPosition(const nanogui::Vector3f &position) {

	m_position = position;
}

void Camera::setTarget(const nanogui::Vector3f &target) {

	m_target = target;
}

void Camera::setUpVector(const nanogui::Vector3f &up) {

	m_up = up;
}

void Camera::setFov(float fov) {

	m_fov = fov;
}

void Camera::setNear(float near) {

	m_near = near;
}

void Camera::setFar(float far) {

	m_far = far;
}

const nanogui::Vector2i &Camera::resolution() const {

	return m_resolution;
}

const nanogui::Vector3f &Camera::position() const {

	return m_position;
}

const nanogui::Vector3f &Camera::target() const {

	return m_target;
}

const nanogui::Vector3f &Camera::up() const {

	return m_up;
}

float Camera::fov() const {

	return m_fov;
}

float Camera::near() const {

	return m_near;
}

float Camera::far() const {

	return m_far;
}

nanogui::Matrix4f Camera::viewMatrix() const {

	return nanogui::lookAt(m_position, m_target, m_up);
}

nanogui::Matrix4f Camera::projectionMatrix() const {
	
	float height = std::tan(m_fov / 360.f * PI) * m_near;
	float width = height * static_cast<float>(m_resolution.x()) / static_cast<float>(m_resolution.y());

	return nanogui::frustum(-width, width, -height, height, m_near, m_far);
}

bool Camera::mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) {

	if (modifiers == 0) {
		m_arcball.motion(p);
		if (m_leftButton) {
			nanogui::Matrix4f view = m_arcball.matrix();
			float distance = (m_position - m_target).norm();
			nanogui::Vector3f direction = view.block<3, 3>(0, 0).transpose() * nanogui::Vector3f(0.f, 0.f, 1.f);
			m_position = m_target + direction * distance;
			m_up = view.block<1, 3>(1, 0);
		}

	} else if (m_leftButton && (modifiers & GLFW_MOD_CONTROL)) {
		float distance = (m_position - m_target).norm();
		distance = clamp(distance + 0.25f * rel.y() * (distance * 0.01f), 0.01f, 10000.f);
		m_position = m_target + (m_position - m_target).normalized() * distance;

	} else if (m_leftButton && (modifiers & GLFW_MOD_SHIFT)) {
		float distance = (m_position - m_target).norm();
		nanogui::Matrix4f view = viewMatrix();
		nanogui::Vector3f left = view.block<1, 3>(0, 0);
		nanogui::Vector3f up = view.block<1, 3>(1, 0);
		nanogui::Vector3f offset = -left * rel.x() * (distance * 0.001f) + up * rel.y() * (distance * 0.001f);
		m_position += offset;
		m_target += offset;
	}

	return true;
}

bool Camera::mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers) {

	if (button == GLFW_MOUSE_BUTTON_1) {
		m_leftButton = down;
	}

	if (button == GLFW_MOUSE_BUTTON_1 && modifiers == 0) {
		m_arcball.setSize(m_resolution);
		m_arcball.setState(nanogui::Quaternionf(viewMatrix().block<3, 3>(0, 0)));
		m_arcball.button(p, down);
	}

	return true;
}

bool Camera::scrollEvent(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) {

	float distance = (m_position - m_target).norm();
	distance = clamp(distance - rel.y() * (distance * 0.01f), 0.01f, 10000.f);
	m_position = m_target + (m_position - m_target).normalized() * distance;

	return true;
}

} // namespace cs224
















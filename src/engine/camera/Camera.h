#ifndef __DX_PCISPH_FLUID_SIMULATOR_STD__
#define __DX_PCISPH_FLUID_SIMULATOR_STD__

#include <utils/STD.h>
#include <nanogui/glutil.h>

class Camera {
public:
	Camera();
	~Camera();

	void setResolution();
	void setPosition();
	void setTarget();
	void setUpVector();
	void setFov();
	void setNear();
	void setFar();

	const Vector2i &resolution() const;
	const Vector3f &position() const;
	const Vector3f &target() const;
	const Vector3f &up() const;

	float fov() const;
	float near() const;
	float far() const;

	Matrix4f viewMatrix() const;
	Matrix4f projectionMatrix() const;

	bool mouseMotionEvent(const Vector2i &p, const Vector2i &rel, int button, int modifiers);
	bool mouseButtonEvent(const Vector2i &p, int button, bool down, int modifiers);
	bool scrollEvent(const Vector2i &p, const Vector2f &rel);

private:
	Vector2i m_resolution;
	Vector3f m_position;
	Vector3f m_target;
	Vector3f m_up;
	float m_fov;
	float m_near;
	float m_far;
};

#endif







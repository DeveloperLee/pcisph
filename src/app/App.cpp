
#include "App.h"

#include <utils/STD.h>
#include <memory>



//============================================================
// public
//============================================================

App::App() : 
	Screen(nanogui::Vector2i(1280, 720), "PCISPH Slover") {

	initialize();
	refresh();
}

App::~App() {

}

bool App::mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) {

	if (!Screen::mouseMotionEvent(p, rel, button, modifiers)) {
		// TODO camera view point movement
	}

	return true;
}

bool App::mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers) {

	if (!Screen::mouseButtonEvent(p, button, down, modifiers)) {
		// TODO
	}

	return true;
}

bool App::scrollEvent(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) {

	if (!Screen::scrollEvent(p, rel)) {
		// TODO
	}

	return true;
}

bool App::keyboardEvent(int key, int scancode, int action, int modifiers) {

	if (!Screen::keyboardEvent(key, scancode, action, modifiers)) {
		if (action == GLFW_PRESS) {
			switch (key) {
			case GLFW_KEY_ESCAPE:
				break;
			case GLFW_KEY_TAB:
				break;
			case GLFW_KEY_SPACE:
				m_running = !m_running;
				break;
			case GLFW_KEY_S:
				break;
			}
		}
		refresh();
	}

	return true;
}

void App::drawContents() {

	if (m_running) {
		// update step
	}

	// render
}



//============================================================
// private
//============================================================

void App::initialize() {

	// settings window
	m_window = new nanogui::Window(this, "Settings");
	m_window->setPosition(Vector2i(15, 15));
	m_window->setLayout(new nanogui::GroupLayout);

	setVisible(true);
}

void App::refresh() {

}

void createMesh() {

}

void clearMesh() {

}





















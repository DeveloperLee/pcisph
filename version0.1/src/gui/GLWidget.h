#pragma once

#include "utils/Def.h"
#include "engine/Engine.h"

namespace cs224 {

// Set to true for the time being.
static bool running = true;

static void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods) {

	if (key == GLFW_KEY_SPACE && action == GLFW_PRESS) {
		running = !running;
	} else if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
		//glfw
	}
}
 
class GLWidget{

public:

	GLWidget();
	~GLWidget();
    
    // Get the context window object
	GLFWwindow *glfwWindow() { return m_window; }

	bool visible() { return m_visible; }

	void setVisible(const bool visible);
    
    // Draw on the glfw screen.
	void draw();

	// Update the simulation.
	void refresh();
    
    // Set up the callback functions for handling the interactions.
	void setupCallbacks();
    
    // glfw window event handlers.
    void keyboardEvent(int key, int scancode, int action, int modifiers);

private:
    
	GLFWwindow* m_window;
	Engine m_engine;
	Eigen::Vector2i m_size;
	Eigen::Vector2i m_windowSize;
	bool m_visible;
};
}

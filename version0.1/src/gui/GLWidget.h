#pragma once

#include "utils/Def.h"
#include "engine/Engine.h"

namespace cs224 {

// Set to true for the time being.
static bool running = true;


 
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
    static Engine m_engine;
private:
    
	GLFWwindow* m_window;

	Eigen::Vector2i m_size;
	Eigen::Vector2i m_windowSize;
	bool m_visible;
};

static void key_callback(GLFWwindow *window, int key, int scancode, int action , int mods) {

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }

    if (key == GLFW_KEY_Z && action == GLFW_PRESS) {
        GLWidget::m_engine.loadScene(base_directory + "scenes/test.json");
    }

    if (key == GLFW_KEY_X && action == GLFW_PRESS) {
        GLWidget::m_engine.loadScene(base_directory + "scenes/test2.json");
    }

    if (key == GLFW_KEY_C && action == GLFW_PRESS) {
        GLWidget::m_engine.loadScene(base_directory + "scenes/test3.json");
    }

    if(action == GLFW_PRESS){
        cs224::GLWidget::m_engine.onKeyPress(key);
    }else if(action == GLFW_RELEASE){
        cs224::GLWidget::m_engine.onKeyReleased(key);
    }
}

static void mouse_move_callback(GLFWwindow *window, double x, double y) {
    int width;
    int height;
    glfwGetWindowSize(window,&width,&height);
    double deltaX = x - (width/2.0);
    double deltaY = y - (height/2.0);
    if(deltaX > 100 || deltaY > 100){
        glfwSetCursorPos(window,width/2.0,height/2.0);
        return;
    }
    cs224::GLWidget::m_engine.onMouseMove(deltaX,deltaY);
    glfwSetCursorPos(window,width/2.0,height/2.0);
}

}

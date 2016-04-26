#include "GLWidget.h"

namespace cs224 {
GLWidget::GLWidget(){

	/* Request a forward compatible OpenGL 3.3 core profile context */
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Request a RGBA8 buffer without MSAA */
    glfwWindowHint(GLFW_SAMPLES, 16);
    glfwWindowHint(GLFW_RED_BITS, 8);
    glfwWindowHint(GLFW_GREEN_BITS, 8);
    glfwWindowHint(GLFW_BLUE_BITS, 8);
    glfwWindowHint(GLFW_ALPHA_BITS, 8);
    glfwWindowHint(GLFW_STENCIL_BITS, 8);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);
    glfwWindowHint(GLFW_VISIBLE, GL_FALSE);
    glfwWindowHint(GLFW_RESIZABLE, GL_TRUE);

    m_window = glfwCreateWindow(1300,700, "CS224 Final Project", NULL, NULL);

    if (!m_window) {
        throw std::runtime_error("Could not create an OpenGL context!");
    }

    glfwMakeContextCurrent(m_window);
    glfwShowWindow(m_window);

    glfwGetFramebufferSize(m_window, &m_size[0], &m_size[1]);
    glViewport(0, 0, m_size[0], m_size[1]);
    glClearColor(0.4f, 0.4f, 0.4f, 0.4f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    glfwSwapInterval(0);
    glfwSwapBuffers(m_window);

	glfwPollEvents();

	glfwGetWindowSize(m_window, &m_windowSize[0], &m_windowSize[1]);

    m_visible = glfwGetWindowAttrib(m_window, GLFW_VISIBLE) != 0;
    
    m_engine.initShaders();
	m_engine.setResolution(m_windowSize, m_windowSize);
	m_engine.loadScene("/Users/LOVEME/Desktop/pbs/scenes/test.json");

	setupCallbacks();
}

GLWidget::~GLWidget() {

}

void GLWidget::setupCallbacks() {

    glfwSetKeyCallback(m_window, key_callback);
}

void GLWidget::setVisible(const bool visible) {

    m_visible = visible;
    if (visible) {
        glfwShowWindow(m_window);
    } else {
        glfwHideWindow(m_window);
    }
}

void GLWidget::draw() {
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
    refresh();
    glfwSwapBuffers(m_window);
}

void GLWidget::refresh() {

	if (running) {
	  m_engine.updateStep();
	  glfwPostEmptyEvent();
	}

    m_engine.render();
}

void GLWidget::keyboardEvent(int key, int scancode, int action, int modifiers) {
    if (action == GLFW_PRESS) {
        switch (key) {
        case GLFW_KEY_ESCAPE:
            glfwHideWindow(m_window);
            break;
        case GLFW_KEY_SPACE:
            running = !running;
            break;
        case GLFW_KEY_F:
            if (modifiers & GLFW_MOD_SHIFT) {
                m_engine.viewOptions().showFluidMesh = !m_engine.viewOptions().showFluidMesh;
            } else {
                m_engine.viewOptions().showFluidParticles = !m_engine.viewOptions().showFluidParticles;
            }
            break;
        }
    }
}

} // namespace cs224
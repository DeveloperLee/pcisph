#include "gui/Window.h"
#include "gui/GLWidget.h"

#define FPS 50

int main(int argc, char *argv[]) {

    try {
        cs224::Window::initglfw();
        std::unique_ptr<cs224::GLWidget> screen(new cs224::GLWidget());
        cs224::Window::startGuiLoop(1000/FPS, screen.get());
        cs224::Window::terminate(screen->glfwWindow());
    } catch (const std::exception &e) {
        std::cerr << "Runtime error: " << e.what() << std::endl;
        return -1;
    }
    return 0;
}

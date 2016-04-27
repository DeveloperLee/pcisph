#pragma once

#include "GLWidget.h"
#include <GLFW/glfw3.h>

#include <chrono>
#include <thread>
#include <memory>

namespace cs224 {

namespace Window{

static bool is_running = false;

// Initialize GLFW
void initglfw() {

    glfwSetErrorCallback(
	    [](int error, const char *desc) {
	        std::cerr << "GLFW error " << error << ": " << desc << std::endl;
	    }
    );

    if (!glfwInit()) {
     throw std::runtime_error("Could not initialize GLFW!");
    }

    glfwSetTime(0);
}

static bool main_active = false;

// Start the GLFW Event loop
void startGuiLoop(int refresh, GLWidget *screen) {
    std::cout << "main loop started" << std::endl;
    if (main_active) {
        throw std::runtime_error("Main loop already started!");
    }

    main_active = true;

    std::thread refresh_thread;
    if (refresh > 0) {
        /* If there are no mouse/keyboard events, try to refresh the
           view roughly every 50 ms (default); this is to support animations
           such as progress bars while keeping the system load
           reasonably low */
        refresh_thread = std::thread(
            [&]() {
                std::chrono::milliseconds time(refresh);
                while (main_active) {
                    std::this_thread::sleep_for(time);
                    glfwPostEmptyEvent();
                }
            }
        );
    }

    try {
        while (main_active) {
             if (glfwWindowShouldClose(screen->glfwWindow())) {
                 screen->setVisible(false);
                 main_active = false;
                 break;
             } /*else if (!screen->visible()) {
                 main_active = false;
                 break;
             }*/
            
            screen->draw();

            /* Wait for mouse/keyboard or empty refresh events */
            glfwWaitEvents();
        }
    } catch (const std::exception &e) {
        std::cerr << "Caught exception in main loop: " << e.what() << std::endl;
        abort();
    }

    if (refresh > 0)
        refresh_thread.join();
}

// Terminate the GLFW
void terminate(GLFWwindow *window) {
    glfwDestroyWindow(window);
	glfwTerminate();
}
}

}

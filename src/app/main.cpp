#include "App.h"

int main(int argc, char *argv[]) {

	try {
		nanogui::init();
		std::unique_ptr<App> screen(new App());
		nanogui::mainloop();
		nanogui::shutdown();
	} catch (const std::exception &e) {
		std::cerr << "Runtime error: " << e.what() << std::endl;
		return -1;
	}

	return 0;
}
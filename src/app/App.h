#ifndef __DX_PCISPH_FLUID_SIMULATOR_APP__
#define __DX_PCISPH_FLUID_SIMULATOR_APP__


#include <nanogui/button.h>
#include <nanogui/checkbox.h>
#include <nanogui/glutil.h>
#include <nanogui/label.h>
#include <nanogui/layout.h>
#include <nanogui/screen.h>
#include <nanogui/slider.h>
#include <nanogui/textbox.h>
#include <nanogui/window.h>

#include <utils/STD.h>



class App : public nanogui::Screen {
public:
	App();
	~App();

	// UI events handlers
	bool mouseMotionEvent(const nanogui::Vector2i &p, const nanogui::Vector2i &rel, int button, int modifiers) override;
	bool mouseButtonEvent(const nanogui::Vector2i &p, int button, bool down, int modifiers) override;
	bool scrollEvent(const nanogui::Vector2i &p, const nanogui::Vector2f &rel) override;
	bool keyboardEvent(int key, int scancode, int action, int modifiers) override;

	void drawContents() override;

private:
	void initialize();
	void refresh();

	void createMesh();
	void clearMesh();

	nanogui::Window *m_window;
	nanogui::CheckBox *m_showBoundaryParticlesCheckBox;

	bool m_running = false;

};	// class App


#endif













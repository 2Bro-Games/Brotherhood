#include "Window.h"

#include "glfw_window/GLFWWindow.h"

namespace Brotherhood {
	Window* Window::Init(const WindowStruct& windowStruct) {
		return new GLFWWindow(windowStruct);
	}

	void Window::Destroy(const Window* pWindow) {
		delete pWindow;
	}
} // Brotherhood

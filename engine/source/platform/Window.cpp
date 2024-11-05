#include "Window.h"

#include "Custom/GLFWWindow.h"

Brotherhood::Window* Brotherhood::Window::Create(
	const Brotherhood::Window::WindowStruct& windowStruct) {
	return new GLFWWindow(windowStruct);
}

void Brotherhood::Window::Destroy(Window* pWindow) {
	delete pWindow;
}

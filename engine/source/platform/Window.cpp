#include "Window.h"

#include "Custom/CustomWindow.h"

Brotherhood::Window* Brotherhood::Window::Create(const Brotherhood::Window::WindowStruct& windowStruct) {
	return new CustomWindow(windowStruct);
}

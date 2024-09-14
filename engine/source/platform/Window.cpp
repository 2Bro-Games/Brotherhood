#include "Window.h"

#include "platform/custom/CustomWindow.h"

namespace Brotherhood {
	Window* Window::Create(const Window::WindowStruct& windowStruct) {
		return new CustomWindow(windowStruct);
	}
}
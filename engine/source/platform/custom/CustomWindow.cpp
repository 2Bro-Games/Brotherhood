#include "CustomWindow.h"
#include <format>

namespace Brotherhood {
	static void GLFWErrorCallback(int error, const char* description) {
		printf(description);
	}

	CustomWindow::CustomWindow(const WindowStruct& windowStruct) {
		Init(windowStruct);
	}

	CustomWindow::~CustomWindow() {
		Shutdown();
	}

	void CustomWindow::Init(const WindowStruct& windowStruct) {
		m_WindowStruct.m_sTitle = windowStruct.m_sTitle;
		m_WindowStruct.m_nWidth = windowStruct.m_nWidth;
		m_WindowStruct.m_nHeight = windowStruct.m_nHeight;

		if (!glfwInit()) {
			throw std::runtime_error("GLFW Window is not inited");
		}
		glfwSetErrorCallback(GLFWErrorCallback);

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_pWindow = glfwCreateWindow(static_cast<int>(windowStruct.m_nWidth),
			static_cast<int>(windowStruct.m_nHeight), m_WindowStruct.m_sTitle.c_str(),
			nullptr, nullptr);

		glfwSetWindowUserPointer(m_pWindow, &m_WindowStruct);
		SetVSync(false);
	}

	void CustomWindow::Shutdown() {
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	void CustomWindow::OnUpdate() {
		glfwPollEvents();
	}

	bool CustomWindow::ShouldClose() {
		return glfwWindowShouldClose(m_pWindow);
	}

	void CustomWindow::SetVSync(bool enabled) {
		m_WindowStruct.m_bVSync = enabled;
	}

	bool CustomWindow::IsVSync() const {
		return m_WindowStruct.m_bVSync;
	}
}
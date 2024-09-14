#include "CustomWindow.h"

#include <format>

#include "core/event/KeyEvent.h"
#include "core/event/MouseEvent.h"
#include "core/event/ApplicationEvent.h"

static void GLFWErrorCallback(int error, const char* description) {
	BROTHER_CORE_ERROR(std::format("GLFW Error ({}): {}", error, description));
}

Brotherhood::CustomWindow::CustomWindow(const WindowStruct& windowStruct) {
	Init(windowStruct);
}

Brotherhood::CustomWindow::~CustomWindow() {
	Shutdown();
}

void Brotherhood::CustomWindow::Init(const WindowStruct& windowStruct) {
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

	glfwSetWindowSizeCallback(m_pWindow, [](GLFWwindow* window, int width, int height) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		windowStruct->m_nWidth = width;
		windowStruct->m_nHeight = height;

		WindowResizeEvent event(static_cast<unsigned int>(width), static_cast<unsigned int>(height));
		windowStruct->m_EventCallback(event);
		});

	glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		WindowCloseEvent event;
		windowStruct->m_EventCallback(event);
		});

	glfwSetKeyCallback(m_pWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		switch (action) {
		case GLFW_PRESS: {
			KeyPressedEvent event(key, false);
			windowStruct->m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE: {
			KeyReleasedEvent event(key);
			windowStruct->m_EventCallback(event);
			break;
		}
		case GLFW_REPEAT: {
			KeyPressedEvent event(key, true);
			windowStruct->m_EventCallback(event);
			break;
		}
		}
		});

	glfwSetMouseButtonCallback(m_pWindow, [](GLFWwindow* window, int button, int action, int mods) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		switch (action) {
		case GLFW_PRESS: {
			MouseButtonPressedEvent event(button);
			windowStruct->m_EventCallback(event);
			break;
		}
		case GLFW_RELEASE: {
			MouseButtonReleasedEvent event(button);
			windowStruct->m_EventCallback(event);
			break;
		}
		}
		});

	glfwSetScrollCallback(m_pWindow, [](GLFWwindow* window, double xOffset, double yOffset) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		MouseScrolledEvent event(static_cast<float>(xOffset), static_cast<float>(yOffset));
		windowStruct->m_EventCallback(event);
		});

	glfwSetCursorPosCallback(m_pWindow, [](GLFWwindow* window, double xPos, double yPos) {
		auto windowStruct = static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
		MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
		windowStruct->m_EventCallback(event);
		});
}

void Brotherhood::CustomWindow::Shutdown() {
	glfwDestroyWindow(m_pWindow);
	glfwTerminate();
}

void Brotherhood::CustomWindow::OnUpdate() {
	glfwPollEvents();
}

void Brotherhood::CustomWindow::SetVSync(bool enabled) {
	m_WindowStruct.m_bVSync = enabled;
}

bool Brotherhood::CustomWindow::IsVSync() const {
	return m_WindowStruct.m_bVSync;
}
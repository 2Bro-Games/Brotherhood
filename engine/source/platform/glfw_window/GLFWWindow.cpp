#include "GLFWWindow.h"

#include <format>

#include "core/event/ApplicationEvent.h"
#include "core/event/KeyEvent.h"
#include "core/event/MouseEvent.h"

namespace Brotherhood {
	GLFWWindow::GLFWWindow(const WindowStruct& inWindowStruct)
	{
		m_WindowStruct.m_sTitle = inWindowStruct.m_sTitle;
		m_WindowStruct.m_nWidth = inWindowStruct.m_nWidth;
		m_WindowStruct.m_nHeight = inWindowStruct.m_nHeight;

		if (!glfwInit()) {
			throw std::runtime_error("GLFW Window is not inited");
		}
		glfwSetErrorCallback([](int error, const char* description) {
			BROTHER_CORE_ERROR(std::format("GLFW Error ({}): {}", error, description));
		});

		glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);

		m_pWindow = glfwCreateWindow(
			static_cast<int>(inWindowStruct.m_nWidth),
			static_cast<int>(inWindowStruct.m_nHeight),
			m_WindowStruct.m_sTitle.c_str(),
			nullptr,
			nullptr);

		glfwSetWindowUserPointer(m_pWindow, &m_WindowStruct);
		SetVSync(false);

		glfwSetWindowSizeCallback(
			m_pWindow,
			[](GLFWwindow* window, int width, int height) {
				const auto windowStruct =
					static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
				windowStruct->m_nWidth = width;
				windowStruct->m_nHeight = height;

				WindowResizeEvent event(
					static_cast<unsigned int>(width),
					static_cast<unsigned int>(height));
				windowStruct->m_EventCallback(event);
			});

		glfwSetWindowCloseCallback(m_pWindow, [](GLFWwindow* window) {
			const auto windowStruct =
				static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
			WindowCloseEvent event;
			windowStruct->m_EventCallback(event);
		});

		glfwSetKeyCallback(
			m_pWindow,
			[](GLFWwindow* window, int key, int scancode, int action, int mods) {
				const auto windowStruct =
					static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
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
					default: {
						break;
					};
				}
			});

		glfwSetMouseButtonCallback(
			m_pWindow,
			[](GLFWwindow* window, int button, int action, int mods) {
				const auto windowStruct =
					static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
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
					default: {
						break;
					};
				}
			});

		glfwSetScrollCallback(
			m_pWindow,
			[](GLFWwindow* window, double xOffset, double yOffset) {
				const auto windowStruct =
					static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
				MouseScrolledEvent event(
					static_cast<float>(xOffset),
					static_cast<float>(yOffset));
				windowStruct->m_EventCallback(event);
			});

		glfwSetCursorPosCallback(
			m_pWindow,
			[](GLFWwindow* window, double xPos, double yPos) {
				const auto windowStruct =
					static_cast<WindowStruct*>(glfwGetWindowUserPointer(window));
				MouseMovedEvent event(static_cast<float>(xPos), static_cast<float>(yPos));
				windowStruct->m_EventCallback(event);
			});
	}

	GLFWWindow::~GLFWWindow() {
		glfwDestroyWindow(m_pWindow);
		glfwTerminate();
	}

	void GLFWWindow::OnUpdate() {
		glfwPollEvents();
	}
} // Brotherhood

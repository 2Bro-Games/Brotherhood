#include "Application.h"
#include "core/event/ApplicationEvent.h"
#include "platform/Window.h"
#include "renderer/RendererAPI.h"

#include <iostream>

namespace Brotherhood {
	Application::Application() {
		m_pWindow = Window::Create();
		m_pWindow->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));

		Renderer::Create(RendererAPI::VULKAN);
	}

	void Application::OnEvent(Event& e) {
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
		dispatcher.Dispatch<WindowResizeEvent>(
			BIND_EVENT_FN(Application::OnWindowResize));

		for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend(); ++it) {
			(*it)->OnEvent(e);
			if (e.Handled) {
				break;
			}
		}
	}

	Application::~Application() {
		Renderer::Destroy();
		Window::Destroy(m_pWindow);
	}

	void Application::Run() {
		while (m_IsRunning) {
			m_pWindow->OnUpdate();
			if (m_IsMinimized) {
				continue;
			}
			for (const auto& layer : m_LayerStack) {
				layer->OnUpdate();
			}
		}
	}

	bool Application::OnWindowClose(WindowCloseEvent& e) {
		m_IsRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e) {
		if (e.GetWidth() == 0 || e.GetHeight() == 0) {
			m_IsMinimized = true;
			return true;
		}

		m_IsMinimized = false;
		return false;
	}

} // Brotherhood

#include "Application.h"
#include "platform/Window.h"
#include "core/event/KeyEvent.h"
#include "core/event/ApplicationEvent.h"

#include <iostream>

Brotherhood::Application::Application() {
	m_Window = Window::Create();
	m_Window->SetEventCallback(BIND_EVENT_FN(Application::OnEvent));
}

void Brotherhood::Application::OnEvent(Event& e) {
	EventDispatcher dispatcher(e);
	dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(Application::OnWindowClose));
	dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(Application::OnWindowResize));

	for (auto it = m_LayerStack.rbegin(); it != m_LayerStack.rend();) {
		(*it)->OnEvent(e);
		if (e.Handled) {
			break;
		}
		++it;
	}
}

Brotherhood::Application::~Application() {
	delete m_Window;
}

void Brotherhood::Application::Run() {
	while (m_IsRunning) {
		m_Window->OnUpdate();
		if (m_IsMinimized) {
			continue;
		}
		for (auto layer : m_LayerStack) {
			layer->OnUpdate();
		}
	}
}

bool Brotherhood::Application::OnWindowClose(WindowCloseEvent& e) {
	m_IsRunning = false;
	return true;
}

bool Brotherhood::Application::OnWindowResize(WindowResizeEvent& e) {
	if (e.GetWidth() == 0 || e.GetHeight() == 0) {
		m_IsMinimized = true;
		return false;
	}

	m_IsMinimized = false;
	return false;
}

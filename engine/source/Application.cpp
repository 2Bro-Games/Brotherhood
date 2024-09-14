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
	std::cout << e << std::endl;
}

Brotherhood::Application::~Application() {
	delete m_Window;
}

void Brotherhood::Application::Run() {
	while (m_IsRunning) {
		if (m_IsMinimized) {
			continue;
		}
		m_Window->OnUpdate();
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

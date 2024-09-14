#include "Application.h"
#include "platform/Window.h"

#include <iostream>

Brotherhood::Application::Application() {
	m_Window = Window::Create();
}

Brotherhood::Application::~Application() {
	delete m_Window;
}

void Brotherhood::Application::Run() {
	while (!m_Window->ShouldClose()) {
		m_Window->OnUpdate();
	}
}

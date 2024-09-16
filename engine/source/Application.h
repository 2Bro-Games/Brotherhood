#ifndef APPLICATION_H
#define APPLICATION_H

#include "Core/Layer/LayerStack.h"

int main(int argc, char** argv);

namespace Brotherhood {
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Window;
	class RendererAPI;

	class Application {
		friend int ::main(int argc, char** argv);

	public:
		Application();
		~Application();

		template<typename T>
		void PushLayer() {
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.PushLayer(std::make_shared<T>());
		}

		template<typename T>
		void PushOverlay() {
			static_assert(std::is_base_of<Layer, T>::value, "Pushed type is not subclass of Layer!");
			m_LayerStack.PushOverlay(std::make_shared<T>());
		}

	private:
		void Run();
		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		LayerStack m_LayerStack;

		Window* m_Window;
		bool m_IsRunning{ true };
		bool m_IsMinimized{ false };

		RendererAPI* m_Renderer;
	};

	Application* CreateApplication();
}

#endif // APPLICATION_H

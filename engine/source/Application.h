#ifndef APPLICATION_H
#define APPLICATION_H

int main(int argc, char** argv);

namespace Brotherhood {
	class Event;
	class WindowCloseEvent;
	class WindowResizeEvent;
	class Window;

	class Application {
		friend int ::main(int argc, char** argv);

	public:
		Application();
		~Application();

	private:
		void Run();
		void OnEvent(Event& e);

		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		Window* m_Window;

		bool m_IsRunning{ true };
		bool m_IsMinimized{ false };
	};

	Application* CreateApplication();
}

#endif // APPLICATION_H

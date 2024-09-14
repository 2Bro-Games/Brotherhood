#ifndef APPLICATION_H
#define APPLICATION_H

namespace Brotherhood {
	class Window;

	class Application {
	public:
		Application();
		~Application();

		void Run();

	private:
		Window* m_Window;
	};

	Application* CreateApplication();
}

#endif // APPLICATION_H

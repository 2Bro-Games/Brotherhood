#ifndef APPLICATION_H
#define APPLICATION_H

namespace Brotherhood {
	class Application {
	public:
		Application() = default;
		~Application() = default;

		void Run();
	};

	Application* CreateApplication();
}

#endif // APPLICATION_H

#include "Brotherhood.h"

class ExampleLayer : public Brotherhood::Layer {
public:
	ExampleLayer()
		: Layer("Example") {}

	void OnUpdate() override {
	}

	void OnEvent(Brotherhood::Event& event) override {
		BROTHER_TRACE(event.ToString());
	}
};

Brotherhood::Application* Brotherhood::CreateApplication() {
	Brotherhood::Application* app = new Brotherhood::Application();
	app->PushLayer<ExampleLayer>();
	return app;
}
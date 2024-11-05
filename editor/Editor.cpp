#include "Brotherhood.h"

class ExampleLayer final : public Brotherhood::Layer {
public:
	ExampleLayer() : Layer("Example") {
	}

	void OnUpdate() override {
	}

	void OnEvent(Brotherhood::Event& event) override {
		BROTHER_TRACE(event.ToString());
	}
};

Brotherhood::Application* Brotherhood::CreateApplication() {
	const auto pApplication = new Application();
	pApplication->PushLayer<ExampleLayer>();
	return pApplication;
}
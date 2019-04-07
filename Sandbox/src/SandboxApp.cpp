#include "Crystal.h"

/**
* Singleton class. Client application
*/

class ExampleLayer : public Crystal::Layer 
{
public:
	ExampleLayer() : Layer() {}

	void OnUpdate() override 
	{
		CL_CORE_LOG_INFO("ExampleLayer::Update");
	}

	void OnEvent(Crystal::Event& event) override
	{
		CL_CORE_LOG_TRACE("ExampleLayer::OnEvent : {0}", event);
	}
};

class Sandbox : public Crystal::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_shared<ExampleLayer>());
	}

	~Sandbox()
	{

	}
};

Crystal::Application* Crystal::CreateApplication()
{
	return new Sandbox();
}
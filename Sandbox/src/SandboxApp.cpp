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
	}

	void OnEvent(Crystal::Event& event) override
	{
		//Crystal key codes usage example
		if(Crystal::Input::IsKeyPressed(CL_KEY_TAB))
			CL_CORE_LOG_TRACE("Tab key is pressed!");
	}
};

class Sandbox : public Crystal::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_shared<ExampleLayer>());
		PushOverlay(std::make_shared<Crystal::ImGuiLayer>());
	}

	~Sandbox()
	{

	}
};

Crystal::Application* Crystal::CreateApplication()
{
	return new Sandbox();
}
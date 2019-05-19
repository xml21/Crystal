#include "Crystal.h"
/**
* Singleton class. Client application
*/

class ExampleLayer : public Crystal::Layer 
{
public:
	ExampleLayer() : Layer() {}

	//void OnUpdate() override 
	//{
	//}
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
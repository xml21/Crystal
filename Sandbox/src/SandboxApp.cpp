#include "Crystal.h"

/**
* Singleton class. Client application
*/

class Sandbox : public Crystal::Application
{
public:
	Sandbox()
	{

	}

	~Sandbox()
	{

	}
};

Crystal::Application* Crystal::CreateApplication()
{
	return new Sandbox();
}
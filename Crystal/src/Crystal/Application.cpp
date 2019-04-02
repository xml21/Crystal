#include "Crpch.h"

#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"

#include "GLFW/glfw3.h"


namespace Crystal
{
	Application::Application()
	{
		mWindow = std::unique_ptr<Window>(Window::Create());
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (true) 
		{
			mWindow->OnUpdate();
		}
	}
}
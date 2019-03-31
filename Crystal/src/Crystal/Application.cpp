#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"


namespace Crystal
{
	Application::Application()
	{
	}


	Application::~Application()
	{
	}

	void Application::Run()
	{
		WindowResizeEvent e(1200, 720);

		CL_CORE_LOG_TRACE(e);

		while (true);
	}
}
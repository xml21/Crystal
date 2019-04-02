#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

class Event;

/**
 * Base application class to be defined in client
 */

namespace Crystal
{
	class CRYSTAL_API Application
	{
	public:
		Application();
		virtual ~Application();

		void Run();

	private:
		std::unique_ptr<Window> mWindow;
		bool mRunning = true;
	};

	//Meant to be defined in client
	Application* CreateApplication();

}


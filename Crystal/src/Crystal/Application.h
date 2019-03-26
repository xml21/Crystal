#pragma once

#include "Core.h"

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
	};

	//Meant to be defined in client
	Application* CreateApplication();

}


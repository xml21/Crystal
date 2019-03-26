#pragma once

#ifdef CL_PLATFORM_WINDOWS

/**
 * Main entry point class containing engine loop.
 * We externally launch CreateApplication() from SandboxApp that returns pointer to our app.
 * Then we can call Run() containing main loop.
 */

extern Crystal::Application* Crystal::CreateApplication();

int main(int argc, char** argv)
{
	auto app = Crystal::CreateApplication();
	app->Run();
	delete app;
}

#endif
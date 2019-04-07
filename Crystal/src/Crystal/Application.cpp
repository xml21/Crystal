#include "Crpch.h"

#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"

#include "GLFW/glfw3.h"

#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

namespace Crystal
{
	Application::Application()
	{
		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));
	}

	Application::~Application()
	{
	}

	void Application::Run()
	{
		while (mRunning)
		{
			for(std::shared_ptr<Layer> layer : mLayerStack)
				layer->OnUpdate();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = mLayerStack.end(); it != mLayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.Handled)
				break;
		}
	}

	void Application::PushLayer(std::shared_ptr<Layer> layer)
	{
		mLayerStack.PushLayer(layer);
	}

	void Application::PushOverlay(std::shared_ptr<Layer> overlay)
	{
		mLayerStack.PushOverlay(overlay);
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

}
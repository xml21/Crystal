#include "Crpch.h"

#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"

#include "Crystal/Logging/Log.h"

#include "glad/glad.h"

#include "Input/Input.h"
#include "Input/KeyCodes.h"

#include "ImGui/ImGuiLayer.h"
#include "ImGui/imgui.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Buffers/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

#include "Time/Time.h"

namespace Crystal
{
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		CL_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		mWindow = Scope<Window>(Window::Create());
		
		mTimer = Ref<Time>(Time::Create());

		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		mImGuiLayer = std::make_shared<ImGuiLayer>();
		PushOverlay(mImGuiLayer);
	}

	void Application::Run()
	{
		while (mRunning)
		{
			mTimer->OnUpdate();

			//--------------- Delta Time calculation ------------------
			float Time = mTimer->GetSeconds(); //Current time (in seconds)
			float DeltaTime = Time - mLastFrameTime; //Delta time
			mLastFrameTime = Time; //Previous time
			//--------------- Delta Time calculation ------------------

			for (Ref<Layer> layer : mLayerStack)
				layer->OnUpdate(DeltaTime);

			mImGuiLayer->Begin();
			for (Ref<Layer> layer : mLayerStack)
				layer->OnImGuiRender();
			mImGuiLayer->End();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<Crystal::WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = mLayerStack.end(); it != mLayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.bHandled)
				break;
		}
	}

	void Application::PushLayer(Ref<Layer> layer)
	{
		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(Ref<Layer> overlay)
	{
		mLayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}
}
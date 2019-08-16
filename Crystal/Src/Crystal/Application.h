#pragma once

#include "Window.h"

#include "Core/Core.h"

#include "Events/Event.h"
#include "Events/KeyEvent.h"
#include "Events/ApplicationEvent.h"

#include "Crystal/Layers/Layer.h"
#include "Crystal/Layers/LayerStack.h"

#include "Crystal/ImGui/ImGuiLayer.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Cameras/OrthographicCamera.h"

#include "Time/Time.h"

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
		virtual ~Application() = default;

		void Run();

		void OnEvent(Event& e);

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> overlay);

		inline Window& GetWindow() { return *mWindow; }

		inline static Application& Get() { return *sInstance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnKeyPressed(KeyPressedEvent& e);

		std::unique_ptr<Window> mWindow;
		std::shared_ptr<Time> mTimer;
		std::shared_ptr<ImGuiLayer> mImGuiLayer;

		bool mRunning = true;

		LayerStack mLayerStack;

		float mLastFrameTime = 0.0f;

		static Application* sInstance;
	};

	//Meant to be defined in client
	Application* CreateApplication();

}


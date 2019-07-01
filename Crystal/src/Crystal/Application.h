#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

#include "Crystal/Layer.h"
#include "Crystal/ImGui/ImGuiLayer.h"
#include "Crystal/LayerStack.h"
#include "Crystal/Events/ApplicationEvent.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Renderer/Shader.h"

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

		void OnEvent(Event& e);

		void PushLayer(std::shared_ptr<Layer> layer);
		void PushOverlay(std::shared_ptr<Layer> overlay);

		inline Window& GetWindow() { return *mWindow; }

		inline static Application& Get() { return *sInstance; }
	private:
		bool OnWindowClose(WindowCloseEvent& e);

		std::unique_ptr<Window> mWindow;
		std::shared_ptr<ImGuiLayer> mImGuiLayer;

		bool mRunning = true;

		LayerStack mLayerStack;

		unsigned int VertexArray, IndexBuffer, VertexBuffer;
		std::unique_ptr<Shader> mShader;

		static Application* sInstance;

	};

	//Meant to be defined in client
	Application* CreateApplication();

}


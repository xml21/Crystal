#pragma once

#include "Core.h"
#include "Events/Event.h"
#include "Window.h"

#include "Crystal/Layer.h"
#include "Crystal/ImGui/ImGuiLayer.h"
#include "Crystal/LayerStack.h"

#include "Crystal/Events/ApplicationEvent.h"

#include "Renderer/Shader.h"

#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Buffers/VertexBuffer.h"

#include "Renderer/VertexArray.h"

#include "Renderer/Cameras/OrthographicCamera.h"

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

		std::unique_ptr<Window> mWindow;
		std::shared_ptr<ImGuiLayer> mImGuiLayer;

		bool mRunning = true;

		LayerStack mLayerStack;

		std::shared_ptr<Shader> mShader;
		std::shared_ptr<VertexBuffer> mVertexBuffer;
		std::shared_ptr<IndexBuffer> mIndexBuffer;
		std::shared_ptr<VertexArray> mVertexArray;
		OrthographicCamera mCamera;

		static Application* sInstance;

	};

	//Meant to be defined in client
	Application* CreateApplication();

}


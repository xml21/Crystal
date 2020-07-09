#pragma once

#include "Window.h"

#include "Core.h"

#include "Crystal/Layers/Layer.h"
#include "Crystal/Layers/LayerStack.h"

#include "Crystal/ImGui/ImGuiLayer.h"

#include "Crystal/Time/Time.h"

class Event;
class KeyEvent;
class ApplicationEvent;

class Shader;
class VertexArray;
class IndexBuffer;
class VertexBuffer;
class OrthographicCamera;

class Time;

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

		virtual void OnInit() {}
		virtual void OnShutdown() {}
		virtual void OnUpdate() {}

		virtual void OnEvent(Event& e);

		void PushLayer(Ref<Layer> layer);
		void PushOverlay(Ref<Layer> overlay);

		void RenderImGui();

		std::string OpenFile(const std::string& Filter) const;

		inline Window& GetWindow() { return *mWindow; }

		inline static Application& Get() { return *sInstance; }

	private:
		bool OnWindowClose(WindowCloseEvent& e);
		bool OnWindowResize(WindowResizeEvent& e);

		std::unique_ptr<Window> mWindow;
		std::shared_ptr<Time> mTimer;
		std::shared_ptr<ImGuiLayer> mImGuiLayer;

		bool mRunning = true, mMinimized = false;

		LayerStack mLayerStack;

		float mLastFrameTime = 0.0f, mDeltaTime = 0.0f, mDeltaTimeMilliseconds = 0.0f, mLastFrameTimeMilliseconds = 0.0f; //Delta in milliseconds for better accuracy in fps counting

		static Application* sInstance;
	};

	//Meant to be defined in client
	Application* CreateApplication();

}


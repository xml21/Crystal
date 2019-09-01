#include "Crpch.h"

#include "Application.h"

#include "Crystal/ImGui/ImGuiLayer.h"

#include "ImGui/imgui.h"

#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Renderer/Framebuffer.h"

#define GLFW_EXPOSE_NATIVE_WIN32
#include <GLFW/glfw3native.h>
#include <GLFW/glfw3.h>

#include <Windows.h>

#include "Crystal/Time/Time.h"
#include "glad/glad.h"
#include <commdlg.h>

namespace Crystal
{
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		CL_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		mWindow = Ref<Window>(Window::Create());
		
		mTimer = Ref<Time>(Time::Create());

		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		mImGuiLayer = std::make_shared<ImGuiLayer>();
		PushOverlay(mImGuiLayer);

		Renderer::Init();
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

			if (!mMinimized)
			{
				for (Ref<Layer> layer : mLayerStack)
					layer->OnUpdate();
			}

			Application* app = this;
			CL_RENDER_1(app, { app->RenderImGui(); });

			Renderer::Get().WaitAndRender();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowResizeEvent>(BIND_EVENT_FN(OnWindowResize));
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

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

	void Application::RenderImGui()
	{
		mImGuiLayer->Begin();

		ImGui::Begin("Renderer");
		auto& caps = RendererAPI::GetCapabilities();
		ImGui::Text("Vendor: %s", caps.Vendor.c_str());
		ImGui::Text("Renderer: %s", caps.Renderer.c_str());
		ImGui::Text("Version: %s", caps.Version.c_str());
		ImGui::End();

		for (Ref<Layer> layer : mLayerStack)
			layer->OnImGuiRender();

		mImGuiLayer->End();
	}

	std::string Application::OpenFile(const std::string& Filter) const
	{
		OPENFILENAMEA Ofn;       // common dialog box structure
		CHAR szFile[260] = { 0 };       // if using TCHAR macros

		// Initialize OPENFILENAME
		ZeroMemory(&Ofn, sizeof(OPENFILENAME));
		Ofn.lStructSize = sizeof(OPENFILENAME);
		Ofn.hwndOwner = glfwGetWin32Window((GLFWwindow*)mWindow->GetNativeWindow());
		Ofn.lpstrFile = szFile;
		Ofn.nMaxFile = sizeof(szFile);
		Ofn.lpstrFilter = "All\0*.*\0";
		Ofn.nFilterIndex = 1;
		Ofn.lpstrFileTitle = NULL;
		Ofn.nMaxFileTitle = 0;
		Ofn.lpstrInitialDir = NULL;
		Ofn.Flags = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

		if (GetOpenFileNameA(&Ofn) == TRUE)
		{
			return Ofn.lpstrFile;
		}
		return std::string();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

	bool Application::OnWindowResize(WindowResizeEvent& e)
	{
		int Width = e.GetWidth(), Height = e.GetHeight();

		if (Width == 0 || Height == 0)
		{
			mMinimized = true;
			return false;
		}
		mMinimized = false;

		CL_RENDER_2(Width, Height, { glViewport(0, 0, Width, Height); });

		auto& Fbs = FramebufferPool::GetGlobal()->GetAll();
		for (auto& Fb : Fbs)
		{
			if(Fb != nullptr)
				Fb->Resize(Width, Height);
		}
			
		return false;
	}
}
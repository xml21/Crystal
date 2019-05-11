#include "Crpch.h"

#include "ImGui/imgui.h"
#include "Platform/OpenGL/ImGuiOpenGLRenderer.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "ImGuiLayer.h"
#include "Crystal/Application.h"

namespace Crystal
{
	ImGuiLayer::ImGuiLayer()
	{
	}

	ImGuiLayer::~ImGuiLayer()
	{
	}

	void ImGuiLayer::OnAttach()
	{
		IMGUI_CHECKVERSION();
		ImGui::CreateContext();

		ImGuiIO& IO = ImGui::GetIO(); (void)IO;
		IO.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;       // Enable Keyboard Controls
		IO.ConfigFlags |= ImGuiConfigFlags_DockingEnable;           // Enable Docking
		IO.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;         // Enable Multi-Viewport / Platform Windows

		ImGui::StyleColorsDark();

		// When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
		ImGuiStyle& style = ImGui::GetStyle();
		if (IO.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
		{
			style.WindowRounding = 0.0f;
			style.Colors[ImGuiCol_WindowBg].w = 1.0f;
		}

		Application& app = Application::Get();
		GLFWwindow* window = static_cast<GLFWwindow*>(app.GetWindow().GetNativeWindow());

		// TEMPORARY: should eventually use Crystal key codes
		IO.KeyMap[ImGuiKey_Tab] = GLFW_KEY_TAB;
		IO.KeyMap[ImGuiKey_LeftArrow] = GLFW_KEY_LEFT;
		IO.KeyMap[ImGuiKey_RightArrow] = GLFW_KEY_RIGHT;
		IO.KeyMap[ImGuiKey_UpArrow] = GLFW_KEY_UP;
		IO.KeyMap[ImGuiKey_DownArrow] = GLFW_KEY_DOWN;
		IO.KeyMap[ImGuiKey_PageUp] = GLFW_KEY_PAGE_UP;
		IO.KeyMap[ImGuiKey_PageDown] = GLFW_KEY_PAGE_DOWN;
		IO.KeyMap[ImGuiKey_Home] = GLFW_KEY_HOME;
		IO.KeyMap[ImGuiKey_End] = GLFW_KEY_END;
		IO.KeyMap[ImGuiKey_Insert] = GLFW_KEY_INSERT;
		IO.KeyMap[ImGuiKey_Delete] = GLFW_KEY_DELETE;
		IO.KeyMap[ImGuiKey_Backspace] = GLFW_KEY_BACKSPACE;
		IO.KeyMap[ImGuiKey_Space] = GLFW_KEY_SPACE;
		IO.KeyMap[ImGuiKey_Enter] = GLFW_KEY_ENTER;
		IO.KeyMap[ImGuiKey_Escape] = GLFW_KEY_ESCAPE;
		IO.KeyMap[ImGuiKey_A] = GLFW_KEY_A;
		IO.KeyMap[ImGuiKey_C] = GLFW_KEY_C;
		IO.KeyMap[ImGuiKey_V] = GLFW_KEY_V;
		IO.KeyMap[ImGuiKey_X] = GLFW_KEY_X;
		IO.KeyMap[ImGuiKey_Y] = GLFW_KEY_Y;
		IO.KeyMap[ImGuiKey_Z] = GLFW_KEY_Z;

		// Setup Platform/Renderer bindings
		ImGui_ImplOpenGL3_Init("#version 410");
	}

	void ImGuiLayer::OnDetach()
	{

	}

	void ImGuiLayer::OnUpdate()
	{
		ImGuiIO& IO = ImGui::GetIO();
		Application& app = Application::Get();
		IO.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		float time = (float)glfwGetTime();
		IO.DeltaTime = mTime > 0.0f ? (time - mTime) : (1.0f / 60.0f);
		mTime = time;

		ImGui_ImplOpenGL3_NewFrame();
		ImGui::NewFrame();

		static bool show = true;
		ImGui::ShowDemoWindow(&show);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
	}

	void ImGuiLayer::OnEvent(Event& event)
	{
		EventDispatcher Dispatcher(event);

		Dispatcher.Dispatch<KeyPressedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonPressedEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnMouseButtonReleasedEvent));
		Dispatcher.Dispatch<MouseMovedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnMouseMovedEvent));
		Dispatcher.Dispatch<MouseScrolledEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnMouseScrolledEvent));
		Dispatcher.Dispatch<KeyPressedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnKeyPressedEvent));
		Dispatcher.Dispatch<KeyTypedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnKeyTypedEvent));
		Dispatcher.Dispatch<KeyReleasedEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnKeyReleasedEvent));
		Dispatcher.Dispatch<WindowResizeEvent>(CL_BIND_EVENT_FN(ImGuiLayer::OnWindowResizeEvent));
	}

	bool ImGuiLayer::OnMouseButtonPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		
		if(e.GetKeyCode() == 0)
			IO.MouseDown[0] = true;

		return false;
	}

	bool ImGuiLayer::OnMouseButtonReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();

		if (e.GetKeyCode() == 0)
			IO.MouseDown[0] = false;

		return false;
	}

	bool ImGuiLayer::OnMouseMovedEvent(MouseMovedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		IO.MousePos = ImVec2(e.GetX(), e.GetY());

		return false;
	}

	bool ImGuiLayer::OnMouseScrolledEvent(MouseScrolledEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		IO.MouseWheelH += e.GetXOffset();
		IO.MouseWheel += e.GetYOffset();

		return false;
	}

	bool ImGuiLayer::OnKeyPressedEvent(KeyPressedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		IO.KeysDown[e.GetKeyCode()] = true;

		IO.KeyCtrl = IO.KeysDown[GLFW_KEY_RIGHT_CONTROL || GLFW_KEY_LEFT_CONTROL];
		IO.KeyShift = IO.KeysDown[GLFW_KEY_RIGHT_SHIFT || GLFW_KEY_LEFT_SHIFT];
		IO.KeyAlt = IO.KeysDown[GLFW_KEY_RIGHT_ALT || GLFW_KEY_LEFT_ALT];
		IO.KeySuper = IO.KeysDown[GLFW_KEY_RIGHT_SUPER || GLFW_KEY_LEFT_SUPER];
		
		return false;
	}

	bool ImGuiLayer::OnKeyReleasedEvent(KeyReleasedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		IO.KeysDown[e.GetKeyCode()] = false;

		return false;
	}

	bool ImGuiLayer::OnKeyTypedEvent(KeyTypedEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();

		int Keycode = e.GetKeyCode();

		if(Keycode > 0 && Keycode < 0x10000)
			IO.AddInputCharacter(unsigned short(Keycode));

		return false;
	}

	bool ImGuiLayer::OnWindowResizeEvent(WindowResizeEvent& e)
	{
		ImGuiIO& IO = ImGui::GetIO();
		Application& app = Application::Get();
		IO.DisplaySize = ImVec2(app.GetWindow().GetWidth(), app.GetWindow().GetHeight());

		IO.DisplayFramebufferScale = ImVec2(1.0f, 1.0f);

		glViewport(0, 0, e.GetWidth(), e.GetHeight());

		return false;
	}
}




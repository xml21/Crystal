#include "Crpch.h"

#include "Memory.h"
#include "WindowsWindow.h"

namespace Crystal
{
	/* One static variable to rule them all (I mean all open windows) */
	static bool sGLFWInitialized = false;

	Window* Window::Create(const WindowProps& props /*= WindowProps()*/)
	{
		return new WindowsWindow(props);
	}


	Crystal::WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	Crystal::WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void Crystal::WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		glfwSwapBuffers(mWindow);
	}

	void Crystal::WindowsWindow::Init(const WindowProps & props)
	{
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;

		if (!sGLFWInitialized)
		{
			int bSuccess = glfwInit();
			CL_CORE_ASSERT(bSuccess, "Could not initialize GLFW!")

			sGLFWInitialized = bSuccess;
		}

		mWindow = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height),
									mData.Title.c_str(), nullptr, nullptr);
		glfwMakeContextCurrent(mWindow);
		glfwSetWindowUserPointer(mWindow, &mData);
	}

	void Crystal::WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
	}
}
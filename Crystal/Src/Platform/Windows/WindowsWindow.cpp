#include "Crpch.h"

#include "Memory.h"
#include "WindowsWindow.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Events/MouseEvent.h"
#include "Crystal/Events/KeyEvent.h"

#include "Platform/OpenGL/OpenGLContext.h"
#include "GLFW/glfw3.h"

namespace Crystal
{
	/* One static variable to rule all open windows) */
	static bool sGLFWInitialized = false;

	static void GLFWErrorCallback(int error, const char* description)
	{
		CL_CORE_LOG_ERROR("GLFW Error ({0}) : {1}", error, description);
	}

	Window* Window::Create(const WindowProps& props /*= WindowProps()*/)
	{
		return new WindowsWindow(props);
	}


	WindowsWindow::WindowsWindow(const WindowProps& props)
	{
		Init(props);
	}

	WindowsWindow::~WindowsWindow()
	{
		Shutdown();
	}

	void WindowsWindow::OnUpdate()
	{
		glfwPollEvents();
		mContext->SwapBuffers();
	}

	void WindowsWindow::Init(const WindowProps & props)
	{
		mData.Title = props.Title;
		mData.Width = props.Width;
		mData.Height = props.Height;

		if (!sGLFWInitialized)
		{
			int bSuccess = glfwInit();
			CL_CORE_ASSERT(bSuccess, "Could not initialize GLFW!");
			glfwSetErrorCallback(GLFWErrorCallback);

			sGLFWInitialized = bSuccess;
		}

		mWindow = glfwCreateWindow(static_cast<int>(props.Width), static_cast<int>(props.Height),
									mData.Title.c_str(), nullptr, nullptr);

		mContext = new OpenGLContext(mWindow);
		mContext->Init();

		glfwSetWindowUserPointer(mWindow, &mData);

		//GLFW event callbacks
		glfwSetWindowSizeCallback(mWindow, [](GLFWwindow* window, int Width, int Height)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			data.Width = Width;
			data.Height = Height;

			WindowResizeEvent event(Width, Height);
			data.EventCallback(event);
		});

		glfwSetWindowCloseCallback(mWindow, [](GLFWwindow* window)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			WindowCloseEvent event;
			data.EventCallback(event);
		});

		glfwSetKeyCallback(mWindow, [](GLFWwindow* window, int key, int scancode, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(key, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(key);
					data.EventCallback(event);
					break;
				}
				case GLFW_REPEAT:
				{
					KeyPressedEvent event(key, 1);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetCharCallback(mWindow, [](GLFWwindow* window, unsigned int keycode)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			KeyTypedEvent event(keycode);
			data.EventCallback(event);
		});

		glfwSetMouseButtonCallback(mWindow, [](GLFWwindow* window, int button, int action, int mods)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			switch (action)
			{
				case GLFW_PRESS:
				{
					KeyPressedEvent event(button, 0);
					data.EventCallback(event);
					break;
				}
				case GLFW_RELEASE:
				{
					KeyReleasedEvent event(button);
					data.EventCallback(event);
					break;
				}
			}
		});

		glfwSetScrollCallback(mWindow, [](GLFWwindow* window, double xOffset, double yOffset)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseScrolledEvent event((float)xOffset, (float)yOffset);
			data.EventCallback(event);
		});

		glfwSetCursorPosCallback(mWindow, [](GLFWwindow* window, double xPos, double yPos)
		{
			WindowData& data = *(WindowData*)glfwGetWindowUserPointer(window);

			MouseMovedEvent event((float)xPos, (float)yPos);
			data.EventCallback(event);
		});
	}

	void WindowsWindow::Shutdown()
	{
		glfwDestroyWindow(mWindow);
	}
}
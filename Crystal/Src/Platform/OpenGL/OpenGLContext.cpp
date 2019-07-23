#include "Crpch.h"
#include "OpenGLContext.h"

#include "GLFW/glfw3.h"
#include "glad/glad.h"
#include "GL/GL.h"

namespace Crystal 
{
	OpenGLContext::OpenGLContext(GLFWwindow* windowHandle)
		: mWindowHandle(windowHandle)
	{
		CL_CORE_ASSERT(windowHandle, "Window handle is null!")
	}

	void OpenGLContext::Init()
	{
		glfwMakeContextCurrent(mWindowHandle);
		int status = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
		CL_CORE_ASSERT(status, "Failed to initialize Glad!");

		CL_CORE_LOG_INFO("OpenGL Info:");
		CL_CORE_LOG_INFO("  Vendor: {0}", glGetString(GL_VENDOR));
		CL_CORE_LOG_INFO("  Renderer: {0}", glGetString(GL_RENDERER));
		CL_CORE_LOG_INFO("  Version: {0}", glGetString(GL_VERSION));
	}

	void OpenGLContext::SwapBuffers()
	{
		glfwSwapBuffers(mWindowHandle);
	}
}

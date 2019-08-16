#include "Crpch.h"
#include "WindowsTime.h"
#include "GLFW/glfw3.h"

namespace Crystal
{
	void WindowsTime::OnUpdate()
	{
		mTime = (float)glfwGetTime();
	}
}
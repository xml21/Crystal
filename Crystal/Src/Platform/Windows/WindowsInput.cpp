#include "Crpch.h"
#include "WindowsInput.h"
#include "Crystal/Application.h"

#include <GLFW/glfw3.h>


namespace Crystal
{
	Input* Input::sInstance = new WindowsInput();

	bool WindowsInput::IsKeyPressedImpl(int KeyCode)
	{
		auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetKey(Window, KeyCode);

		return state == GLFW_PRESS || state == GLFW_REPEAT;
	}

	bool WindowsInput::IsMouseButtonPressedImpl(int Button)
	{
		auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		auto state = glfwGetMouseButton(Window, Button);

		return state == GLFW_PRESS;
	}

	std::pair<float, float> WindowsInput::GetMousePositionImpl()
	{
		auto Window = static_cast<GLFWwindow*>(Application::Get().GetWindow().GetNativeWindow());
		double xPos, yPos;
		glfwGetCursorPos(Window, &xPos, &yPos);

		return {(float)xPos, (float)yPos};
	}
}


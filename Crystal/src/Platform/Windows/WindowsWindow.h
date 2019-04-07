#pragma once

#include "Crystal/Window.h"

struct GLFWwindow;

namespace Crystal
{
	class WindowsWindow : public Window
	{
	public:
		WindowsWindow(const WindowProps& props);
		virtual ~WindowsWindow();

		void OnUpdate() override;

		inline unsigned int GetWidth() const override { return mData.Width; };
		inline unsigned int GetHeight() const override { return mData.Height; };

		inline void SetEventCallback(const EventCallbackFunction& callback) override { mData.EventCallback = callback; };

		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

	private:
		GLFWwindow* mWindow;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool VSync;

			EventCallbackFunction EventCallback;
		};

		WindowData mData;
	};
}
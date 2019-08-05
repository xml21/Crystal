#pragma once

#include "Crystal/Window.h"
#include "Crystal/Renderer/GraphicsContext.h"

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

		inline void* GetNativeWindow() const { return mWindow; }

		void SetEventCallback(const EventCallbackFunction& callback) override { mData.EventCallback = callback; };

	private:
		virtual void Init(const WindowProps& props);
		virtual void Shutdown();

		GLFWwindow* mWindow;
		GraphicsContext* mContext;

		struct WindowData
		{
			std::string Title;
			unsigned int Width, Height;
			bool bVSync;

			EventCallbackFunction EventCallback;
		};

		WindowData mData;
	};
}
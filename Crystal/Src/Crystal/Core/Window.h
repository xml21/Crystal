#pragma once

#include "Crpch.h"

#include "Crystal/Core/Core.h"
#include "Crystal/Events/Event.h"

namespace Crystal
{
	struct WindowProps
	{
		std::string Title;
		unsigned int Width, Height;

		WindowProps(const std::string& title = "Crystal Engine",
					unsigned int width = 1280,
					unsigned int height = 720) 
					: Title(title), Width(width), Height(height) {}
	};

	/*	Interface representing desktop system based Window.
	*	Platform independant, abstract implementation of Window. */
	class CRYSTAL_API Window
	{
	public:
		using EventCallbackFunction = std::function<void(Event&)>;

		virtual ~Window() = default;

		virtual void OnUpdate() = 0;

		virtual unsigned int GetWidth() const = 0;
		virtual unsigned int GetHeight() const = 0;
		virtual std::pair<float, float> GetWindowPos() const = 0;

		virtual void SetEventCallback(const EventCallbackFunction& callback) = 0;
		virtual void SetVSync(bool bEnabled) = 0;

		virtual void* GetNativeWindow() const = 0;

		static Window* Create(const WindowProps& props = WindowProps());
	};
}
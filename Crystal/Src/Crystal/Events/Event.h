#pragma once

#include "Crpch.h"

#include "Crystal/Core/Core.h"
#include "Crystal/Logging/Log.h"

namespace Crystal
{
	enum class EventType
	{
		NONE = 0,
		WindowClose, WindowResize, WindowFocus, WindowLostFocus, WindowMoved,	//Window related event types
		AppTick, AppUpdate, AppRender,											//Application related event types
		KeyPressed, KeyReleased, KeyTyped,										//Key related event types
		MouseButtonPressed, MouseButtonReleased, MouseMoved, MouseScrolled		//Mouse related event typesS
	};

	/* Bitmask created to describe multiple categories of an Event */
	enum EventCategory
	{
		NONE = 0,
		EventCategoryApplication =	BIT(0),
		EventCategoryInput =		BIT(1),
		EventCategoryKeyboard =		BIT(2),
		EventCategoryMouse =		BIT(3),
		EventCategoryMouseButton =	BIT(4),
	};

	/* Macro defines meant to be used in classes deriving from Event. Created to avoid code redundancy */
	#define EVENT_CLASS_TYPE(type)	static EventType GetStaticType() { return EventType::##type; }\
									virtual EventType GetEventType() const override { return GetStaticType(); }\
									virtual const char* GetName() const override { return #type; }

	#define EVENT_CLASS_CATEGORY(category)	virtual int GetCategoryFlags() const override { return category; }

	class CRYSTAL_API Event
	{
		friend class EventDispatcher;

	public:
		bool bHandled = false;

		/* virtual getter functions that must be implemented in subclasses */
		virtual EventType GetEventType() const = 0;
		virtual const char* GetName() const = 0;
		virtual int GetCategoryFlags() const = 0;

		/* virtual getter functions that can be optionally implemented in subclasses */
		virtual std::string ToString() const { return GetName(); }

		inline bool IsInCategory(EventCategory Category) { return GetCategoryFlags() & Category; }
	};

	class EventDispatcher
	{
		/* Generic template function taking T arg and returning bool.
		Meant to be used as an argument by generic Dispatch() function.
		*/
		template<typename T>
		using EventFn = std::function<bool(T&)>;

	public:
		EventDispatcher(Event& e) : mEvent(e) {}

		template<typename T>
		bool Dispatch(EventFn<T> func) 
		{
			if (mEvent.GetEventType() == T::GetStaticType())
			{
				mEvent.bHandled = func(*(T*)&mEvent);
				//CL_CORE_LOG_TRACE("{0}", mEvent.ToString());
				return true;
			}
			//CL_CORE_LOG_ERROR("{0}", mEvent.ToString());
			return false;
		}

	private:
		Event& mEvent;

};

inline std::ostream& operator<<(std::ostream& os, const Event& e)
{
	return os << e.ToString();
}

}
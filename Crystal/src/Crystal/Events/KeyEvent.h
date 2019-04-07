#pragma once

#include "Event.h"

namespace Crystal
{
	class CRYSTAL_API KeyEvent : public Event
	{
	public:
		inline int GetKeyCode () const { return mKeyCode; }

		EVENT_CLASS_CATEGORY(EventCategoryKeyboard | EventCategoryInput)
	protected:
		KeyEvent(int KeyCode) : mKeyCode(KeyCode) {}

		int mKeyCode;
	};

	class CRYSTAL_API KeyPressedEvent : public KeyEvent
	{
	public:
		KeyPressedEvent(int KeyCode, int RepeatCount)
			: KeyEvent(KeyCode), mRepeatCount(RepeatCount) {}

		inline int GetRepeatCount() const { return mRepeatCount; }

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyPressedEvent: " << mKeyCode << " (" << mRepeatCount << " repeats)";
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyPressed)
	private:
		int mRepeatCount;
	};

	class CRYSTAL_API KeyReleasedEvent : public KeyEvent
	{

	public:
		KeyReleasedEvent(int Keycode) : KeyEvent(mKeyCode) {}

		std::string ToString() const override
		{
			std::stringstream ss;
			ss << "KeyReleasedEvent: " << mKeyCode;
			return ss.str();
		}

		EVENT_CLASS_TYPE(KeyReleased)
	};
}

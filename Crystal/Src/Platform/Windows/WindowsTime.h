#pragma once
#include "Crystal/Time/Time.h"
#include "GLFW/glfw3.h"

namespace Crystal
{
	/* Struct holding current delta time in seconds */

	class WindowsTime : public Time
	{
	public:
		virtual float GetSeconds() const override { return mTime; }
		virtual float GetMilliSeconds() const override { return mTime * 1000.0f; }

		virtual void OnUpdate() override;

	private:
		float mTime;
	};
}
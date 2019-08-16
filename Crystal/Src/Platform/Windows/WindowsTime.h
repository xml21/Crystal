#pragma once
#include "Crystal/Time/Time.h"
#include "GLFW/glfw3.h"

namespace Crystal
{
	/* Class holding current time in seconds. */
	class WindowsTime : public Time
	{
	public:
		virtual float GetSeconds() const override { return mTime; }
		virtual float GetMilliSeconds() const override { return mTime * 1000.0f; }

		virtual void OnUpdate() override;

		operator float() const { return mTime; }

	private:
		float mTime;
	};
}
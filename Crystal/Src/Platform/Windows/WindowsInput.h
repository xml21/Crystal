#pragma once
#include "Crystal/Input.h"

namespace Crystal
{
	class WindowsInput : public Input
	{
	protected:
		virtual bool IsKeyPressedImpl(int KeyCode) override;
		virtual bool IsMouseButtonPressedImpl(int Button) override;
		virtual std::pair<float, float> GetMousePositionImpl() override;

	};
}



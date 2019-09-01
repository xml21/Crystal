#pragma once

#include "Crystal/Core/Core.h"

namespace Crystal
{
	class CRYSTAL_API Input
	{
		public:
			inline static bool IsKeyPressed(int KeyCode) { return sInstance->IsKeyPressedImpl(KeyCode); }
			inline static bool IsMouseButtonPressed(int Button) { return sInstance->IsMouseButtonPressedImpl(Button); }
			inline static std::pair<float, float> GetMousePosition() { return sInstance->GetMousePositionImpl(); }
			inline static float GetMouseX() { return sInstance->GetMouseXImpl(); }
			inline static float GetMouseY() { return sInstance->GetMouseYImpl(); }

		protected:
			virtual bool IsKeyPressedImpl(int KeyCode) = 0;
			virtual bool IsMouseButtonPressedImpl(int Button) = 0;

			virtual float GetMouseXImpl() = 0;
			virtual float GetMouseYImpl() = 0;

			virtual std::pair<float, float> GetMousePositionImpl() = 0;

		private:
			static Input* sInstance;
	};
}
#pragma once

#include "Crystal/Core/Core.h"
#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	enum class TextureWrap
	{
		None = 0,
		Clamp = 1,
		Repeat = 2
	};

	enum class TextureFormat
	{
		None = 0,
		RGB = 1,
		RGBA = 2,
	};

	class Texture
	{
	public:
		virtual ~Texture() = default;

		virtual uint32_t GetWidth() const = 0;
		virtual uint32_t GetHeight() const = 0;

		virtual RendererID GetRendererID() const = 0;

		virtual void Bind(uint32_t Slot = 0) const = 0;
	};
}

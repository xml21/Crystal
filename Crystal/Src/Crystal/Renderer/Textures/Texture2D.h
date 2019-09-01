#pragma once

#include "Crystal/Core/Core.h"
#include "string.h"

#include "Texture.h"

namespace Crystal
{
	class Texture2D : public Texture
	{
	public:
		static Ref<Texture2D> Create(TextureFormat Format, unsigned int Width, unsigned int Height);
		static Ref<Texture2D> Create(const std::string& Path, bool sRGB = false);

		virtual const std::string& GetPath() const = 0;
	};
}
#pragma once

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Core/Core.h"

#include "Texture.h"

namespace Crystal
{
	class TextureCube : public Texture
	{
	public:
		static Ref<TextureCube> Create(const std::string& Path);

		virtual TextureFormat GetFormat() const = 0;
		virtual const std::string& GetPath() const = 0;
	};
}

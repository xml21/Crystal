#include "Crpch.h"
#include "Texture.h"

#include "Crystal/Renderer/RendererAPI.h"
#include "Platform/OpenGL/Textures/OpenGLTexture.h"
#include "Platform/OpenGL/Textures/OpenGLTexture2D.h"
#include "Platform/OpenGL/Textures/OpenGLTextureCube.h"


namespace Crystal {

	uint32_t Texture::GetBPP(TextureFormat Format)
	{
		switch (Format)
		{
		case TextureFormat::RGB:    return 3;
		case TextureFormat::RGBA:   return 4;
		}
		return 0;
	}

}
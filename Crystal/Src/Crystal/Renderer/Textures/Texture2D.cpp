#include "Crpch.h"
#include "Texture2D.h"

#include "Crystal/Renderer/Renderer.h"

#include "Platform/OpenGL/Textures/OpenGLTexture2D.h"

namespace Crystal
{
	Ref<Texture2D> Texture2D::Create(TextureFormat Format, unsigned int Width, unsigned int Height)
	{
		switch (Renderer::GetAPI())
		{
		case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case API::OpenGL:	return std::make_shared<OpenGLTexture2D>(Format, Width, Height);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}

	Ref<Texture2D> Texture2D::Create(const std::string& Path, bool sRGB)
	{
		switch (Renderer::GetAPI())
		{
			case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
			case API::OpenGL:	return std::make_shared<OpenGLTexture2D>(Path, sRGB);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}
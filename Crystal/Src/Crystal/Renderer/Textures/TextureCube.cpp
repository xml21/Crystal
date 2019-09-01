#include "Crpch.h"

#include "TextureCube.h"

#include "Crystal/Renderer/Renderer.h"
#include "Crystal/Renderer/RendererAPI.h"

#include "Platform/OpenGL/Textures/OpenGLTextureCube.h"

namespace Crystal
{
	Ref<TextureCube> TextureCube::Create(const std::string& Path)
	{
		switch (RendererAPI::GetAPI())
		{
		case API::NONE: return nullptr;
		case API::OpenGL: return std::make_shared<OpenGLTextureCube>(Path);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");

		return nullptr;
	}
}
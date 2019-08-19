#include "Crpch.h"

#include "Shader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Crystal
{
	Ref<Shader> Shader::Create(const std::string& VertexSrc, const std::string& FragmentSrc)
	{
		switch (Renderer::GetAPI())
		{
		case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case API::OpenGL:	return std::make_shared<OpenGLShader>(VertexSrc, FragmentSrc);
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

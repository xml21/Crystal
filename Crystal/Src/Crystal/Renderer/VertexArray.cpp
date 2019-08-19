#include "Crpch.h"

#include "VertexArray.h"
#include "Renderer.h"
#include "Platform/OpenGL/OpenGLVertexArray.h"

namespace Crystal
{
	Ref<VertexArray> VertexArray::Create()
	{
		switch (Renderer::GetAPI())
		{
		case API::NONE:		CL_CORE_ASSERT(false, "RendererAPI::None is currently not supported"); return nullptr;
		case API::OpenGL:	return std::make_shared<OpenGLVertexArray>();
		}

		CL_CORE_ASSERT(false, "Unknown RendererAPI!");
		return nullptr;
	}
}

#include "Crpch.h"
#include "RenderCommand.h"
#include "Platform/OpenGL/OpenGLRendererAPI.h"

namespace Crystal
{
	std::shared_ptr<Crystal::RendererAPI> RenderCommand::sRendererAPI = std::make_shared<OpenGLRendererAPI>();

}
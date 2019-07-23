#include "Crpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

class VertexArray;

namespace Crystal
{
	void Renderer::BeginScene()
	{

	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<VertexArray>& vertexArray)
	{
		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}


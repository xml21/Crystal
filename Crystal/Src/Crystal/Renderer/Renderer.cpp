#include "Crpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

class VertexArray;

namespace Crystal
{
	std::shared_ptr<SceneData> Renderer::mSceneData = std::make_shared<SceneData>();

	void Renderer::BeginScene(OrthographicCamera& Camera)
	{
		mSceneData->SetViewProjectionMatrix(Camera.GetViewProjectionMatrix());
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexArray>& vertexArray)
	{
		shader->Bind();
		shader->UploadUniformMat4("u_ViewProjection", mSceneData->GetViewProjectionMatrix());

		vertexArray->Bind();
		RenderCommand::DrawIndexed(vertexArray);
	}

}


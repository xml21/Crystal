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

	void Renderer::Submit(const std::shared_ptr<Shader>& Shader, const std::shared_ptr<VertexArray>& VertexArray, const glm::mat4& Transform)
	{
		Shader->Bind();
		Shader->UploadUniformMat4("uViewProjection", mSceneData->GetViewProjectionMatrix());
		Shader->UploadUniformMat4("uTransform", Transform);

		VertexArray->Bind();
		RenderCommand::DrawIndexed(VertexArray);
	}

}


#include "Crpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLShader.h"


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

		//------------------- TODO: Prepare proper abstraction of Shader.cpp class -------------------
		std::shared_ptr<OpenGLShader> MyOpenGLShader = std::dynamic_pointer_cast<OpenGLShader>(Shader);

		MyOpenGLShader->UploadUniformMat4("uViewProjection", mSceneData->GetViewProjectionMatrix());
		MyOpenGLShader->UploadUniformMat4("uTransform", Transform);
		//--------------------------------------------------------------------------------------------

		VertexArray->Bind();
		RenderCommand::DrawIndexed(VertexArray);
	}

}


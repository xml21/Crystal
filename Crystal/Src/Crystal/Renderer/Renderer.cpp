#include "Crpch.h"
#include "Renderer.h"
#include "RenderCommand.h"

#include "Platform/OpenGL/OpenGLShader.h"


class VertexArray;

namespace Crystal
{
	Ref<SceneData> Renderer::mSceneData = std::make_shared<SceneData>();

	void Renderer::BeginScene(OrthographicCamera& Camera)
	{
		mSceneData->SetViewProjectionMatrix(Camera.GetViewProjectionMatrix());
	}

	void Renderer::EndScene()
	{

	}

	void Renderer::Submit(const Ref<Shader>& Shader, const Ref<VertexArray>& VertexArray, const glm::mat4& Transform)
	{
		Shader->Bind();

		//------------------- TODO: Prepare proper abstraction of Shader.cpp class -------------------
		Ref<OpenGLShader> MyOpenGLShader = std::dynamic_pointer_cast<OpenGLShader>(Shader);

		MyOpenGLShader->UploadUniformMat4("uViewProjection", mSceneData->GetViewProjectionMatrix());
		MyOpenGLShader->UploadUniformMat4("uTransform", Transform);
		//--------------------------------------------------------------------------------------------

		VertexArray->Bind();
		RenderCommand::DrawIndexed(VertexArray);
	}

}


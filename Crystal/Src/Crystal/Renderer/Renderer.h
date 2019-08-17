#pragma once

#include "RendererAPI.h"

#include "Cameras/OrthographicCamera.h"
#include "Shader.h"

class RenderCommand;
class VertexArray;

namespace Crystal
{
	struct SceneData
	{
		inline glm::mat4 GetViewProjectionMatrix() const { return ViewProjectionMatrix; }
		void SetViewProjectionMatrix(glm::mat4 val) { ViewProjectionMatrix = val; }

	private:
		glm::mat4 ViewProjectionMatrix;
	};

	class Renderer
	{
	public:
		static void BeginScene(OrthographicCamera& Camera);
		static void EndScene();

		static void Submit(const std::shared_ptr<Shader>& Shader, const std::shared_ptr<VertexArray>& VertexArray, const glm::mat4& Transform = glm::mat4(1.0f));

		inline static API GetAPI() { return RendererAPI::GetAPI(); };

		static std::shared_ptr<SceneData> mSceneData;
	};
}

#include "Crpch.h"
#include "Renderer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Crystal
{
	Ref<SceneData> Renderer::mSceneData = std::make_shared<SceneData>();
	Renderer* Renderer::sInstance = new Renderer();

	API RendererAPI::sAPI = API::OpenGL;

	void Renderer::Init()
	{
		CL_RENDER({ RendererAPI::Init(); });
	}

	void Renderer::Clear()
	{
		CL_RENDER({
			RendererAPI::Clear(glm::vec4(0.0f, 0.0f, 0.0f, 1.0f));
		});
	}

	void Renderer::Clear(float r, float g, float b, float a /*= 1.0f*/)
	{
		CL_RENDER_4(r, g, b, a, {
			RendererAPI::Clear(glm::vec4(r, g, b, a));
		});
	}

	void Renderer::SetClearColor(float r, float g, float b, float a)
	{

	}

	void Renderer::DrawIndexed(unsigned int Count, bool DepthTest /*= true*/)
	{
		CL_RENDER_2(Count, DepthTest, {
			RendererAPI::DrawIndexed(Count, DepthTest);
		});
	}

	void Renderer::WaitAndRender()
	{
		sInstance->mCommandQueue.Execute();
	}

}


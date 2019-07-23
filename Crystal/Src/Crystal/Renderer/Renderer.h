#pragma once
#include "RendererAPI.h"

class RenderCommand;
class VertexArray;

namespace Crystal
{
	class Renderer
	{
	public:

		/* TODO: add parameters */
		static void BeginScene();
		static void EndScene();
		/* TODO: add parameters */

		static void Submit(const std::shared_ptr<VertexArray>& vertexarray);

		inline static API GetAPI() { return RendererAPI::GetAPI(); };
	};
}

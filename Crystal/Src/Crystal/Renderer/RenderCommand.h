#pragma once

#include "RendererAPI.h"

namespace Crystal
{
	class RenderCommand
	{
	public:
		static inline void SetClearColor(const glm::vec4& color) { sRendererAPI->SetClearColor(color); };
		static inline void Clear() { sRendererAPI->Clear(); }

		inline static void DrawIndexed(const Ref<VertexArray>& vertexArray)
		{
			sRendererAPI->DrawIndexed(vertexArray);
		}
	private:
		static std::shared_ptr<RendererAPI> sRendererAPI;
	};
}
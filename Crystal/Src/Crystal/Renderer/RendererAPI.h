#pragma once

#include "glm/glm.hpp"

#include "VertexArray.h"

namespace Crystal
{
	enum class API
	{
		NONE = 0, OpenGL = 1
	};

	class RendererAPI
	{
	public:
		virtual void SetClearColor(const glm::vec4& color) = 0;
		virtual void Clear() = 0;

		/* Draw call request supplied by vertex array */
		virtual void DrawIndexed(const Ref<VertexArray>& vertexarray) = 0;

		inline static API GetAPI() { return sAPI; }

	private:
		static API sAPI;
	};
}
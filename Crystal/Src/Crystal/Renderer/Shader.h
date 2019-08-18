#pragma once

#include <string>
#include "glm/glm.hpp"

namespace Crystal
{
	class Shader
	{
	public:
		virtual ~Shader() = default;

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		static std::shared_ptr<Shader> Create(const std::string& VertexSrc, const std::string& FragmentSrc);
	};
}

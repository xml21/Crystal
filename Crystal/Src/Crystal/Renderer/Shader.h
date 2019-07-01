#pragma once

#include <string>

namespace Crystal
{
	class Shader
	{
	public:
		Shader(const std::string& VertexSrc, const std::string& FragmentSrc);
		~Shader();

		void Bind() const;
		void UnBind() const;

	private:
		uint32_t mRendererID;
	};
}

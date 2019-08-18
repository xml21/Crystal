#pragma once

#include "Crystal/Renderer/Shader.h"

namespace Crystal
{
	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& VertexSrc, const std::string& FragmentSrc);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		void CalculateUniformLocation(const std::string& Name);

		void UploadUniformInt(const std::string& Name, int Value);

		void UploadUniformFloat(const std::string& Name, float Value);
		void UploadUniformFloat2(const std::string& Name, const glm::vec2& Value);
		void UploadUniformFloat3(const std::string& Name, const glm::vec3& Value);
		void UploadUniformFloat4(const std::string& Name, const glm::vec4& Value);

		void UploadUniformMat3(const std::string& Name, const glm::mat3& Value);
		void UploadUniformMat4(const std::string& Name, const glm::mat4& Value);

	private:
		uint32_t mRendererID;
	};
}

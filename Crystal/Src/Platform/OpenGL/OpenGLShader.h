#pragma once

#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/RendererAPI.h"
#include "glad/glad.h"

namespace Crystal
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& Filepath);
		virtual ~OpenGLShader();

		virtual void Bind() const override;
		virtual void UnBind() const override;

		virtual void Reload() override;

		virtual void UploadUniformBuffer(const UniformBufferBase& uniformBuffer) override;

		virtual void SetFloat(const std::string& Name, float Value) override;
		virtual void SetMat4(const std::string& Name, const glm::mat4& Value) override;

		virtual const std::string& GetName() const override { return mName; }

	private:
		void UploadUniformInt(const std::string& Name, int Value);

		void UploadUniformFloat(const std::string& Name, float Value);
		void UploadUniformFloat2(const std::string& Name, const glm::vec2& Value);
		void UploadUniformFloat3(const std::string& Name, const glm::vec3& Value);
		void UploadUniformFloat4(const std::string& Name, const glm::vec4& Value);

		void UploadUniformMat3(const std::string& Name, const glm::mat3& Value);
		void UploadUniformMat4(const std::string& Name, const glm::mat4& Value);

		void ReadShaderFromFile(const std::string& Filepath);

		void CompileAndUploadShader();
		static GLenum ShaderTypeFromString(const std::string& type);

		int32_t GetUniformLocation(const std::string& Name) const;

	private:
		RendererID mRendererID;

		std::string mName, mAssetPath, mShaderSource;

		mutable std::unordered_map<std::string, int32_t> UniformLocations;
	};
}

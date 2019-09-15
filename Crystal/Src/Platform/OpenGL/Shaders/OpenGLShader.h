#pragma once

#include "Crystal/Renderer/Shader.h"

#include "OpenGLShaderUniform.h"

#include "glad/glad.h"

namespace Crystal
{

	class OpenGLShader : public Shader
	{
	public:
		OpenGLShader(const std::string& Filepath);

		virtual void Bind() override;
		virtual void UnBind() override;

		virtual void Reload() override;

		virtual void AddShaderReloadedCallback(const ShaderReloadedCallback& Callback) override;

		virtual void UploadUniformBuffer(const UniformBufferBase& NewUniformBuffer) override;

		virtual void SetVSMaterialUniformBuffer(Buffer InBuffer) override;
		virtual void SetPSMaterialUniformBuffer(Buffer InBuffer) override;

		virtual void SetFloat(const std::string& Name, float Value) override;
		virtual void SetMat4(const std::string& Name, const glm::mat4& Value) override;

		virtual void SetMat4FromRenderThread(const std::string& Name, const glm::mat4& Value) override;

		virtual const std::string& GetName() const override { return mName; }

	private:
		std::string ReadShaderFromFile(const std::string& Filepath) const;

		std::unordered_map<GLenum, std::string> PreProcess(const std::string& Source);
		void Parse();
		void ParseUniform(const std::string& Statement, ShaderDomain Domain);
		void ParseUniformStruct(const std::string& Block, ShaderDomain Domain);
		ShaderStruct* FindStruct(const std::string& Name);

		void ResolveUniforms();
		void ValidateUniforms();

		void CompileAndUploadShader();
		static GLenum ShaderTypeFromString(const std::string& Type);

		int32_t GetUniformLocation(const std::string& Name) const;

		void ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBufferDeclaration>& Decl, Buffer InBuffer);
		void ResolveAndSetUniform(OpenGLShaderUniformDeclaration* InUniform, Buffer InBuffer);
		void ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* InUniform, Buffer InBuffer);
		void ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& Field, byte* Data, int32_t Offset);

		void UploadUniformInt(uint32_t Location, int32_t Value);
		void UploadUniformIntArray(uint32_t Location, int32_t* Values, int32_t Count);
		void UploadUniformFloat(uint32_t Location, float Value);
		void UploadUniformFloat2(uint32_t Location, const glm::vec2& Value);
		void UploadUniformFloat3(uint32_t Location, const glm::vec3& Value);
		void UploadUniformFloat4(uint32_t Location, const glm::vec4& Value);
		void UploadUniformMat3(uint32_t Location, const glm::mat3& Value);
		void UploadUniformMat4(uint32_t Location, const glm::mat4& Value);
		void UploadUniformMat4Array(uint32_t Location, const glm::mat4& Value, uint32_t Count);
		void UploadUniformStruct(OpenGLShaderUniformDeclaration* Uniform, byte* InBuffer, uint32_t Offset);

		void UploadUniformInt(const std::string& Name, int32_t Value);
		void UploadUniformIntArray(const std::string& Name, int32_t* Values, int32_t Count);

		void UploadUniformFloat(const std::string& Name, float Value);
		void UploadUniformFloat2(const std::string& Name, const glm::vec2& Value);
		void UploadUniformFloat3(const std::string& Name, const glm::vec3& Value);
		void UploadUniformFloat4(const std::string& Name, const glm::vec4& Value);

		void UploadUniformMat4(const std::string& Name, const glm::mat4& Value);

		inline const ShaderUniformBufferList& GetVSRendererUniforms() const override { return mVSRendererUniformBuffers; }
		inline const ShaderUniformBufferList& GetPSRendererUniforms() const override { return mPSRendererUniformBuffers; }
		inline const ShaderUniformBufferDeclaration& GetVSMaterialUniformBuffer() const override { return *mVSMaterialUniformBuffer; }
		inline const ShaderUniformBufferDeclaration& GetPSMaterialUniformBuffer() const override { return *mPSMaterialUniformBuffer; }
		inline const ShaderResourceList& GetResources() const override { return mResources; }

	private:
		RendererID mRendererID;
		
		bool mLoaded = false;

		std::string mName, mAssetPath;

		mutable std::unordered_map<std::string, int32_t> UniformLocations;
		std::unordered_map<GLenum, std::string> mShaderSource;

		std::vector<ShaderReloadedCallback> mShaderReloadedCallbacks;

		ShaderUniformBufferList mVSRendererUniformBuffers;
		ShaderUniformBufferList mPSRendererUniformBuffers;
		Scope<OpenGLShaderUniformBufferDeclaration> mVSMaterialUniformBuffer;
		Scope<OpenGLShaderUniformBufferDeclaration> mPSMaterialUniformBuffer;
		ShaderResourceList mResources;
		ShaderStructList mStructs;
	};
}

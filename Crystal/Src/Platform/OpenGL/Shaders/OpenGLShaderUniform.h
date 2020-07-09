#pragma once

#include "Crystal/Renderer/ShaderUniform.h"

namespace Crystal {

	class OpenGLShaderResourceDeclaration : public ShaderResourceDeclaration
	{
	public:
		enum class Type
		{
			NONE, TEXTURE2D, TEXTURECUBE
		};
	private:
		friend class OpenGLShader;
	private:
		std::string mName;
		uint32_t mRegister = 0;
		uint32_t mCount;
		Type mType;
	public:
		OpenGLShaderResourceDeclaration(Type InType, const std::string& Name, uint32_t Count);

		inline const std::string& GetName() const override { return mName; }
		inline uint32_t GetRegister() const override { return mRegister; }
		inline uint32_t GetCount() const override { return mCount; }

		inline Type GetType() const { return mType; }
	public:
		static Type StringToType(const std::string& InType);
		static std::string TypeToString(Type InType);
	};

	class OpenGLShaderUniformDeclaration : public ShaderUniformDeclaration
	{
	private:
		friend class OpenGLShader;
		friend class OpenGLShaderUniformBufferDeclaration;
	public:
		enum class Type
		{
			NONE, FLOAT32, VEC2, VEC3, VEC4, MAT3, MAT4, INT32, STRUCT
		};
	private:
		std::string mName;
		uint32_t mSize;
		uint32_t mCount;
		uint32_t mOffset;
		ShaderDomain mDomain;

		Type mType;
		ShaderStruct* mStruct;
		mutable int32_t mLocation;
	public:
		OpenGLShaderUniformDeclaration(ShaderDomain Domain, Type InType, const std::string& Name, uint32_t Count = 1);
		OpenGLShaderUniformDeclaration(ShaderDomain Domain, ShaderStruct* UniformStruct, const std::string& Name, uint32_t Count = 1);

		inline const std::string& GetName() const override { return mName; }
		inline uint32_t GetSize() const override { return mSize; }
		inline uint32_t GetCount() const override { return mCount; }
		inline uint32_t GetOffset() const override { return mOffset; }
		inline uint32_t GetAbsoluteOffset() const { return mStruct ? mStruct->GetOffset() + mOffset : mOffset; }
		inline ShaderDomain GetDomain() const { return mDomain; }

		int32_t GetLocation() const { return mLocation; }
		inline Type GetType() const { return mType; }
		inline bool IsArray() const { return mCount > 1; }
		inline const ShaderStruct& GetShaderUniformStruct() const { CL_CORE_ASSERT(mStruct, ""); return *mStruct; }
	protected:
		void SetOffset(uint32_t Offset) override;
	public:
		static uint32_t SizeOfUniformType(Type InType);
		static Type StringToType(const std::string& InType);
		static std::string TypeToString(Type InType);
	};

	struct GLShaderUniformField
	{
		OpenGLShaderUniformDeclaration::Type Type;
		std::string Name;
		uint32_t Count;
		mutable uint32_t Size;
		mutable int32_t Location;
	};

	class OpenGLShaderUniformBufferDeclaration : public ShaderUniformBufferDeclaration
	{
	private:
		friend class Shader;
	private:
		std::string mName;
		ShaderUniformList mUniforms;
		uint32_t mRegister;
		uint32_t mSize;
		ShaderDomain mDomain;
	public:
		OpenGLShaderUniformBufferDeclaration(const std::string& Name, ShaderDomain Domain);

		void PushUniform(OpenGLShaderUniformDeclaration* Uniform);

		inline const std::string& GetName() const override { return mName; }
		inline uint32_t GetRegister() const override { return mRegister; }
		inline uint32_t GetSize() const override { return mSize; }
		virtual ShaderDomain GetDomain() const { return mDomain; }
		inline const ShaderUniformList& GetUniformDeclarations() const override { return mUniforms; }

		ShaderUniformDeclaration* FindUniform(const std::string& Name);
	};

}
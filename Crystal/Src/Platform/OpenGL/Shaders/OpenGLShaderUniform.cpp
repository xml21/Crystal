#include "Crpch.h"
#include "OpenGLShaderUniform.h"

namespace Crystal {

	OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain Domain, Type InType, const std::string& Name, uint32_t Count)
		: mType(InType), mStruct(nullptr), mDomain(Domain)
	{
		mName = Name;
		mCount = Count;
		mSize = SizeOfUniformType(InType) * Count;
	}

	OpenGLShaderUniformDeclaration::OpenGLShaderUniformDeclaration(ShaderDomain Domain, ShaderStruct* UniformStruct, const std::string& Name, uint32_t Count)
		: mStruct(UniformStruct), mType(OpenGLShaderUniformDeclaration::Type::STRUCT), mDomain(Domain)
	{
		mName = Name;
		mCount = Count;
		mSize = mStruct->GetSize() * Count;
	}

	void OpenGLShaderUniformDeclaration::SetOffset(uint32_t Offset)
	{
		if (mType == OpenGLShaderUniformDeclaration::Type::STRUCT)
			mStruct->SetOffset(Offset);

		mOffset = Offset;
	}

	uint32_t OpenGLShaderUniformDeclaration::SizeOfUniformType(Type InType)
	{
		switch (InType)
		{
		case OpenGLShaderUniformDeclaration::Type::INT32:      return 4;
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return 4;
		case OpenGLShaderUniformDeclaration::Type::VEC2:       return 4 * 2;
		case OpenGLShaderUniformDeclaration::Type::VEC3:       return 4 * 3;
		case OpenGLShaderUniformDeclaration::Type::VEC4:       return 4 * 4;
		case OpenGLShaderUniformDeclaration::Type::MAT3:       return 4 * 3 * 3;
		case OpenGLShaderUniformDeclaration::Type::MAT4:       return 4 * 4 * 4;
		}
		return 0;
	}

	OpenGLShaderUniformDeclaration::Type OpenGLShaderUniformDeclaration::StringToType(const std::string& InType)
	{
		if (InType == "int32")    return Type::INT32;
		if (InType == "float")    return Type::FLOAT32;
		if (InType == "vec2")     return Type::VEC2;
		if (InType == "vec3")     return Type::VEC3;
		if (InType == "vec4")     return Type::VEC4;
		if (InType == "mat3")     return Type::MAT3;
		if (InType == "mat4")     return Type::MAT4;

		return Type::NONE;
	}

	std::string OpenGLShaderUniformDeclaration::TypeToString(Type InType)
	{
		switch (InType)
		{
		case OpenGLShaderUniformDeclaration::Type::INT32:      return "int32";
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:    return "float";
		case OpenGLShaderUniformDeclaration::Type::VEC2:       return "vec2";
		case OpenGLShaderUniformDeclaration::Type::VEC3:       return "vec3";
		case OpenGLShaderUniformDeclaration::Type::VEC4:       return "vec4";
		case OpenGLShaderUniformDeclaration::Type::MAT3:       return "mat3";
		case OpenGLShaderUniformDeclaration::Type::MAT4:       return "mat4";
		}
		return "Invalid Type";
	}

	OpenGLShaderUniformBufferDeclaration::OpenGLShaderUniformBufferDeclaration(const std::string& Name, ShaderDomain Domain)
		: mName(Name), mDomain(Domain), mSize(0), mRegister(0)
	{
	}

	void OpenGLShaderUniformBufferDeclaration::PushUniform(OpenGLShaderUniformDeclaration* Uniform)
	{
		uint32_t Offset = 0;
		if (mUniforms.size())
		{
			OpenGLShaderUniformDeclaration* Previous = (OpenGLShaderUniformDeclaration*)mUniforms.back();
			Offset = Previous->mOffset + Previous->mSize;
		}
		Uniform->SetOffset(Offset);
		mSize += Uniform->GetSize();
		mUniforms.push_back(Uniform);
	}

	ShaderUniformDeclaration* OpenGLShaderUniformBufferDeclaration::FindUniform(const std::string& Name)
	{
		for (ShaderUniformDeclaration* Uniform : mUniforms)
		{
			if (Uniform->GetName() == Name)
				return Uniform;
		}
		return nullptr;
	}

	OpenGLShaderResourceDeclaration::OpenGLShaderResourceDeclaration(Type InType, const std::string& Name, uint32_t Count)
		: mType(InType), mName(Name), mCount(Count)
	{
		mName = Name;
		mCount = Count;
	}

	OpenGLShaderResourceDeclaration::Type OpenGLShaderResourceDeclaration::StringToType(const std::string& InType)
	{
		if (InType == "sampler2D")		return Type::TEXTURE2D;
		if (InType == "samplerCube")	return Type::TEXTURECUBE;

		return Type::NONE;
	}

	std::string OpenGLShaderResourceDeclaration::TypeToString(Type InType)
	{
		switch (InType)
		{
		case Type::TEXTURE2D:	return "sampler2D";
		case Type::TEXTURECUBE:		return "samplerCube";
		}
		return "Invalid Type";
	}

}
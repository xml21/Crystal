#pragma once

#include "Crystal/Core/Core.h"
#include "Crystal/Logging/Log.h"

#include <string>
#include <vector>

namespace Crystal 
{
	enum class ShaderDomain
	{
		None = 0, Vertex = 0, Pixel = 1
	};

	class ShaderUniformDeclaration
	{
	private:
		friend class Shader;
		friend class OpenGLShader;
		friend class ShaderStruct;
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual uint32_t GetCount() const = 0;
		virtual uint32_t GetOffset() const = 0;
		virtual ShaderDomain GetDomain() const = 0;
	protected:
		virtual void SetOffset(uint32_t Offset) = 0;
	};

	typedef std::vector<ShaderUniformDeclaration*> ShaderUniformList;

	class ShaderUniformBufferDeclaration
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
		virtual uint32_t GetSize() const = 0;
		virtual const ShaderUniformList& GetUniformDeclarations() const = 0;

		virtual ShaderUniformDeclaration* FindUniform(const std::string& Name) = 0;
	};

	typedef std::vector<ShaderUniformBufferDeclaration*> ShaderUniformBufferList;

	class ShaderStruct
	{
	private:
		friend class Shader;
	private:
		std::string mName;
		std::vector<ShaderUniformDeclaration*> mFields;
		uint32_t mSize;
		uint32_t mOffset;
	public:
		ShaderStruct(const std::string& Name)
			: mName(Name), mSize(0), mOffset(0)
		{
		}

		void AddField(ShaderUniformDeclaration* Field)
		{
			mSize += Field->GetSize();
			uint32_t Offset = 0;
			if (mFields.size())
			{
				ShaderUniformDeclaration* Previous = mFields.back();
				Offset = Previous->GetOffset() + Previous->GetSize();
			}
			Field->SetOffset(Offset);
			mFields.push_back(Field);
		}

		inline void SetOffset(uint32_t Offset) { mOffset = Offset; }

		inline const std::string& GetName() const { return mName; }
		inline uint32_t GetSize() const { return mSize; }
		inline uint32_t GetOffset() const { return mOffset; }
		inline const std::vector<ShaderUniformDeclaration*>& GetFields() const { return mFields; }
	};

	typedef std::vector<ShaderStruct*> ShaderStructList;

	class ShaderResourceDeclaration
	{
	public:
		virtual const std::string& GetName() const = 0;
		virtual uint32_t GetRegister() const = 0;
		virtual uint32_t GetCount() const = 0;
	};

	typedef std::vector<ShaderResourceDeclaration*> ShaderResourceList;
}
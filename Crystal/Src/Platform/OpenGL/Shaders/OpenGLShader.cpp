#include "Crpch.h"

#include "OpenGLShader.h"

#include "glad/glad.h"

#include "glm/gtc/Type_ptr.hpp"

#include "Crystal/Renderer/RendererAPI.h"

#include "Crystal/Core/Buffer.h"

#include <string>
#include <sstream>
#include <limits>

namespace Crystal
{
	// ------------------------------- Parsing helper functions -------------------------------------
	const char* FindToken(const char* Str, const std::string& Token)
	{
		const char* T = Str;
		while (T = strstr(T, Token.c_str()))
		{
			bool Left = Str == T || isspace(T[-1]);
			bool Right = !T[Token.size()] || isspace(T[Token.size()]);
			if (Left && Right)
				return T;

			T += Token.size();
		}
		return nullptr;
	}

	const char* FindToken(const std::string& Str, const std::string& Token)
	{
		return FindToken(Str.c_str(), Token);
	}

	std::vector<std::string> SplitString(const std::string& Str, const std::string& Delimiters)
	{
		size_t Start = 0;
		size_t End = Str.find_first_of(Delimiters);

		std::vector<std::string> Result;

		while (End <= std::string::npos)
		{
			std::string Token = Str.substr(Start, End - Start);
			if (!Token.empty())
				Result.push_back(Token);

			if (End == std::string::npos)
				break;

			Start = End + 1;
			End = Str.find_first_of(Delimiters, Start);
		}

		return Result;
	}

	std::vector<std::string> SplitString(const std::string& Str, const char Delimiter)
	{
		return SplitString(Str, std::string(1, Delimiter));
	}

	std::vector<std::string> Tokenize(const std::string& Str)
	{
		return SplitString(Str, " \t\n");
	}

	std::vector<std::string> GetLines(const std::string& Str)
	{
		return SplitString(Str, "\n");
	}

	std::string GetBlock(const char* Str, const char** OutPosition)
	{
		const char* End = strstr(Str, "}");
		if (!End)
			return Str;

		if (OutPosition)
			*OutPosition = End;
		__int64 Length = End - Str + 1;
		return std::string(Str, Length);
	}

	std::string GetStatement(const char* Str, const char** OutPosition)
	{
		const char* End = strstr(Str, ";");
		if (!End)
			return Str;

		if (OutPosition)
			*OutPosition = End;
		__int64 length = End - Str + 1;
		return std::string(Str, length);
	}

	bool StartsWith(const std::string& Str, const std::string& Start)
	{
		return Str.find(Start) == 0;
	}

	static bool IsTypeStringResource(const std::string& Type)
	{
		if (Type == "sampler2D")		return true;
		if (Type == "samplerCube")		return true;
		if (Type == "sampler2DShadow")	return true;
		return false;
	}
	// ------------------------------- Parsing helper functions -------------------------------------

	OpenGLShader::OpenGLShader(const std::string& FilePath)
		: mAssetPath(FilePath)
	{	
		size_t Found = FilePath.find_last_of("/\\");
		mName = Found != std::string::npos ? FilePath.substr(Found + 1) : FilePath;
		Found = mName.find_last_of(".");
		mName = Found != std::string::npos ? mName.substr(0, Found) : mName;

		Reload();
	}

	void OpenGLShader::Reload()
	{
		std::string Source = ReadShaderFromFile(mAssetPath);
		mShaderSource = PreProcess(Source);
		Parse();

		CL_RENDER_S({
			if (self->mRendererID)
				glDeleteShader(self->mRendererID);

			self->CompileAndUploadShader();
			self->ResolveUniforms();
			self->ValidateUniforms();

			if (self->mLoaded)
			{
				for (auto& callback : self->mShaderReloadedCallbacks)
					callback();
			}

			self->mLoaded = true;
		});
	}

	void OpenGLShader::AddShaderReloadedCallback(const ShaderReloadedCallback& Callback)
	{
		mShaderReloadedCallbacks.push_back(Callback);
	}

	void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& NewUniformBuffer)
	{
		for (unsigned int i = 0; i < NewUniformBuffer.GetUniformCount(); i++)
		{
			const UniformDecl& Decl = NewUniformBuffer.GetUniforms()[i];
			switch (Decl.Type)
			{
			case UniformType::Float:
			{
				const std::string& Name = Decl.Name;
				float Value = *(float*)(NewUniformBuffer.GetBuffer() + Decl.Offset);

				CL_RENDER_S2(Name, Value, {
					self->UploadUniformFloat(Name, Value);
				});
			}
			case UniformType::Float3:
			{
				const std::string& Name = Decl.Name;
				glm::vec3& Values = *(glm::vec3*)(NewUniformBuffer.GetBuffer() + Decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformFloat3(Name, Values);
				});
			}
			case UniformType::Float4:
			{
				const std::string& Name = Decl.Name;
				glm::vec4& Values = *(glm::vec4*)(NewUniformBuffer.GetBuffer() + Decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformFloat4(Name, Values);
				});
			}
			case UniformType::Matrix4x4:
			{
				const std::string& Name = Decl.Name;
				glm::mat4& Values = *(glm::mat4*)(NewUniformBuffer.GetBuffer() + Decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformMat4(Name, Values);
				});
			}
			}
		}
	}

	void OpenGLShader::SetVSMaterialUniformBuffer(Buffer InBuffer)
	{
		CL_RENDER_S1(InBuffer, {
			glUseProgram(self->mRendererID);
			self->ResolveAndSetUniforms(self->mVSMaterialUniformBuffer, InBuffer);
			});
	}

	void OpenGLShader::SetPSMaterialUniformBuffer(Buffer InBuffer)
	{
		CL_RENDER_S1(InBuffer, {
			glUseProgram(self->mRendererID);
			self->ResolveAndSetUniforms(self->mPSMaterialUniformBuffer, InBuffer);
			});
	}

	void OpenGLShader::SetFloat(const std::string& Name, float Value)
	{
		CL_RENDER_S2(Name, Value, {
			self->UploadUniformFloat(Name, Value);
		});
	}

	void OpenGLShader::SetMat4(const std::string& Name, const glm::mat4& Value)
	{
		CL_RENDER_S2(Name, Value, {
			self->UploadUniformMat4(Name, Value);
		});
	}

	void OpenGLShader::SetMat4FromRenderThread(const std::string& Name, const glm::mat4& Value)
	{
		CL_RENDER_S2(Name, Value, {
			self->UploadUniformMat4(Name, Value);
		});
	}

	void OpenGLShader::Bind()
	{
		CL_RENDER_S({
			glUseProgram(self->mRendererID);
		});
	}

	void OpenGLShader::UnBind()
	{
		CL_RENDER_S({
			glUseProgram(0);
		});
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& Name) const
	{
		int32_t result = glGetUniformLocation(mRendererID, Name.c_str());
		if (result == -1)
			CL_CORE_LOG_WARN("Could not find uniform '{0}' in shader", Name);

		return result;
	}

	void OpenGLShader::ResolveAndSetUniforms(const Scope<OpenGLShaderUniformBufferDeclaration>& Decl, Buffer InBuffer)
	{
		const ShaderUniformList& Uniforms = Decl->GetUniformDeclarations();
		for (size_t i = 0; i < Uniforms.size(); i++)
		{
			OpenGLShaderUniformDeclaration* Uniform = (OpenGLShaderUniformDeclaration*)Uniforms[i];
			if (Uniform->IsArray())
				ResolveAndSetUniformArray(Uniform, InBuffer);
			else
				ResolveAndSetUniform(Uniform, InBuffer);
		}
	}

	void OpenGLShader::ResolveAndSetUniform(OpenGLShaderUniformDeclaration* InUniform, Buffer InBuffer)
	{
		if (InUniform->GetLocation() == -1)
			return;

		CL_CORE_ASSERT(Uniform->GetLocation() != -1, "Uniform has invalid location!");

		uint32_t Offset = InUniform->GetOffset();

		switch (InUniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(InUniform->GetLocation(), *(float*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(InUniform->GetLocation(), *(int32_t*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(InUniform->GetLocation(), *(glm::vec2*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(InUniform->GetLocation(), *(glm::vec3*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(InUniform->GetLocation(), *(glm::vec4*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(InUniform->GetLocation(), *(glm::mat3*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(InUniform->GetLocation(), *(glm::mat4*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(InUniform, InBuffer.Data, Offset);
			break;
		default:
			CL_CORE_ASSERT(false, "Unknown uniform Type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformArray(OpenGLShaderUniformDeclaration* InUniform, Buffer InBuffer)
	{
		uint32_t Offset = InUniform->GetOffset();
		switch (InUniform->GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(InUniform->GetLocation(), *(float*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(InUniform->GetLocation(), *(int32_t*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(InUniform->GetLocation(), *(glm::vec2*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(InUniform->GetLocation(), *(glm::vec3*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(InUniform->GetLocation(), *(glm::vec4*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(InUniform->GetLocation(), *(glm::mat3*)&InBuffer.Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4Array(InUniform->GetLocation(), *(glm::mat4*)&InBuffer.Data[Offset], InUniform->GetCount());
			break;
		case OpenGLShaderUniformDeclaration::Type::STRUCT:
			UploadUniformStruct(InUniform, InBuffer.Data, Offset);
			break;
		default:
			CL_CORE_ASSERT(false, "Unknown uniform Type!");
		}
	}

	void OpenGLShader::ResolveAndSetUniformField(const OpenGLShaderUniformDeclaration& Field, byte* Data, int32_t Offset)
	{
		switch (Field.GetType())
		{
		case OpenGLShaderUniformDeclaration::Type::FLOAT32:
			UploadUniformFloat(Field.GetLocation(), *(float*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::INT32:
			UploadUniformInt(Field.GetLocation(), *(int32_t*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC2:
			UploadUniformFloat2(Field.GetLocation(), *(glm::vec2*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC3:
			UploadUniformFloat3(Field.GetLocation(), *(glm::vec3*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::VEC4:
			UploadUniformFloat4(Field.GetLocation(), *(glm::vec4*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT3:
			UploadUniformMat3(Field.GetLocation(), *(glm::mat3*)&Data[Offset]);
			break;
		case OpenGLShaderUniformDeclaration::Type::MAT4:
			UploadUniformMat4(Field.GetLocation(), *(glm::mat4*)&Data[Offset]);
			break;
		default:
			CL_CORE_ASSERT(false, "Unknown uniform Type!");
		}
	}

	void OpenGLShader::UploadUniformInt(uint32_t Location, int32_t Value)
	{
		glUniform1i(Location, Value);
	}

	void OpenGLShader::UploadUniformInt(const std::string& Name, int32_t Value)
	{
		int32_t Location = GetUniformLocation(Name);
		glUniform1i(Location, Value);
	}

	void OpenGLShader::UploadUniformIntArray(uint32_t Location, int32_t* Values, int32_t Count)
	{
		glUniform1iv(Location, Count, Values);
	}

	void OpenGLShader::UploadUniformIntArray(const std::string& Name, int32_t* Values, int32_t Count)
	{
		int32_t Location = GetUniformLocation(Name);
		glUniform1iv(Location, Count, Values);
	}

	void OpenGLShader::UploadUniformFloat(uint32_t Location, float Value)
	{
		glUniform1f(Location, Value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& Name, float Value)
	{
		glUseProgram(mRendererID);
		auto Location = glGetUniformLocation(mRendererID, Name.c_str());
		if (Location != -1)
			glUniform1f(Location, Value);
		else
			CL_CORE_LOG_INFO("Uniform '{0}' not found!", Name);
	}

	void OpenGLShader::UploadUniformFloat2(uint32_t Location, const glm::vec2& Value)
	{
		glUniform2f(Location, Value.x, Value.y);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& Name, const glm::vec2& Value)
	{
		glUseProgram(mRendererID);
		auto Location = glGetUniformLocation(mRendererID, Name.c_str());
		if (Location != -1)
			glUniform2f(Location, Value.x, Value.y);
		else
			CL_CORE_LOG_INFO("Uniform '{0}' not found!", Name);
	}

	void OpenGLShader::UploadUniformFloat3(uint32_t Location, const glm::vec3& Value)
	{
		glUniform3f(Location, Value.x, Value.y, Value.z);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& Name, const glm::vec3& Value)
	{
		glUseProgram(mRendererID);
		auto Location = glGetUniformLocation(mRendererID, Name.c_str());
		if (Location != -1)
			glUniform3f(Location, Value.x, Value.y, Value.z);
		else
			CL_CORE_LOG_INFO("Uniform '{0}' not found!", Name);
	}

	void OpenGLShader::UploadUniformFloat4(uint32_t Location, const glm::vec4& Value)
	{
		glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& Name, const glm::vec4& Value)
	{
		glUseProgram(mRendererID);
		auto Location = glGetUniformLocation(mRendererID, Name.c_str());
		if (Location != -1)
			glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
		else
			CL_CORE_LOG_INFO("Uniform '{0}' not found!", Name);
	}

	void OpenGLShader::UploadUniformMat3(uint32_t Location, const glm::mat3& Value)
	{
		glUniformMatrix3fv(Location, 1, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::UploadUniformMat4(uint32_t Location, const glm::mat4& Value)
	{
		glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& Name, const glm::mat4& Value)
	{
		glUseProgram(mRendererID);
		auto Location = glGetUniformLocation(mRendererID, Name.c_str());
		if (Location != -1)
			glUniformMatrix4fv(Location, 1, GL_FALSE, (const float*)&Value);
		else
			CL_CORE_LOG_INFO("Uniform '{0}' not found!", Name);
	}

	void OpenGLShader::UploadUniformMat4Array(uint32_t Location, const glm::mat4& Value, uint32_t Count)
	{
		glUniformMatrix4fv(Location, Count, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::UploadUniformStruct(OpenGLShaderUniformDeclaration* Uniform, byte* InBuffer, uint32_t Offset)
	{
		const ShaderStruct& UniformShaderStruct = Uniform->GetShaderUniformStruct();
		const auto& Fields = UniformShaderStruct.GetFields();

		for (size_t k = 0; k < Fields.size(); k++)
		{
			OpenGLShaderUniformDeclaration* Field = (OpenGLShaderUniformDeclaration*)Fields[k];
			ResolveAndSetUniformField(*Field, InBuffer, Offset);
			Offset += Field->mSize;
		}
	}

	std::string OpenGLShader::ReadShaderFromFile(const std::string& Filepath) const
	{
		std::string Result;
		std::ifstream in(Filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			Result.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&Result[0], Result.size());
			in.close();
		}
		else
		{
			CL_CORE_LOG_WARN("Could not read shader file {0}", Filepath);
		}

		return Result;
	}

	std::unordered_map<GLenum, std::string> OpenGLShader::PreProcess(const std::string& Source)
	{
		std::unordered_map<GLenum, std::string> ShaderSources;

		const char* TypeToken = "#type";
		size_t TypeTokenLength = strlen(TypeToken);
		size_t Pos = Source.find(TypeToken, 0);

		while (Pos != std::string::npos)
		{
			size_t EOL = Source.find_first_of("\r\n", Pos);
			CL_CORE_ASSERT(EOL != std::string::npos, "Syntax error");

			size_t Begin = Pos + TypeTokenLength + 1;
			std::string Type = Source.substr(Begin, EOL - Begin);
			CL_CORE_ASSERT(Type == "vertex" || Type == "fragment" || Type == "pixel", "Invalid shader Type specified");

			size_t NextLinePos = Source.find_first_not_of("\r\n", EOL);
			Pos = Source.find(TypeToken, NextLinePos);
			ShaderSources[ShaderTypeFromString(Type)] = Source.substr(NextLinePos, Pos - (NextLinePos == std::string::npos ? Source.size() - 1 : NextLinePos));
		}

		return ShaderSources;
	}

	void OpenGLShader::Parse()
	{
		const char* Token;
		const char* vStr;
		const char* fStr;

		mResources.clear();
		mStructs.clear();
		mVSMaterialUniformBuffer.reset();
		mPSMaterialUniformBuffer.reset();

		auto& VertexSource = mShaderSource[GL_VERTEX_SHADER];
		auto& FragmentSource = mShaderSource[GL_FRAGMENT_SHADER];

		// Vertex Shader
		vStr = VertexSource.c_str();
		while (Token = FindToken(vStr, "struct"))
			ParseUniformStruct(GetBlock(Token, &vStr), ShaderDomain::Vertex);

		vStr = VertexSource.c_str();
		while (Token = FindToken(vStr, "uniform"))
			ParseUniform(GetStatement(Token, &vStr), ShaderDomain::Vertex);

		// Fragment Shader
		fStr = FragmentSource.c_str();
		while (Token = FindToken(fStr, "struct"))
			ParseUniformStruct(GetBlock(Token, &fStr), ShaderDomain::Pixel);

		fStr = FragmentSource.c_str();
		while (Token = FindToken(fStr, "uniform"))
			ParseUniform(GetStatement(Token, &fStr), ShaderDomain::Pixel);
	}

	void OpenGLShader::ParseUniform(const std::string& Statement, ShaderDomain Domain)
	{
		std::vector<std::string> Tokens = Tokenize(Statement);
		uint32_t Index = 0;

		Index++; // "uniform"
		std::string TypeString = Tokens[Index++];
		std::string Name = Tokens[Index++];
		// Strip ; from name if present
		if (const char* s = strstr(Name.c_str(), ";"))
			Name = std::string(Name.c_str(), s - Name.c_str());

		std::string N(Name);
		int32_t Count = 1;
		const char* Namestr = N.c_str();
		if (const char* S = strstr(Namestr, "["))
		{
			Name = std::string(Namestr, S - Namestr);

			const char* End = strstr(Namestr, "]");
			std::string c(S + 1, End - S);
			Count = atoi(c.c_str());
		}

		if (IsTypeStringResource(TypeString))
		{
			ShaderResourceDeclaration* Declaration = new OpenGLShaderResourceDeclaration(OpenGLShaderResourceDeclaration::StringToType(TypeString), Name, Count);
			mResources.push_back(Declaration);
		}
		else
		{
			OpenGLShaderUniformDeclaration::Type T = OpenGLShaderUniformDeclaration::StringToType(TypeString);
			OpenGLShaderUniformDeclaration* Declaration = nullptr;

			if (T == OpenGLShaderUniformDeclaration::Type::NONE)
			{
				// Find struct
				ShaderStruct* S = FindStruct(TypeString);
				CL_CORE_ASSERT(S, "");
				Declaration = new OpenGLShaderUniformDeclaration(Domain, S, Name, Count);
			}
			else
			{
				Declaration = new OpenGLShaderUniformDeclaration(Domain, T, Name, Count);
			}

			if (StartsWith(Name, "r_"))
			{
				if (Domain == ShaderDomain::Vertex)
					((OpenGLShaderUniformBufferDeclaration*)mVSRendererUniformBuffers.front())->PushUniform(Declaration);
				else if (Domain == ShaderDomain::Pixel)
					((OpenGLShaderUniformBufferDeclaration*)mPSRendererUniformBuffers.front())->PushUniform(Declaration);
			}
			else
			{
				if (Domain == ShaderDomain::Vertex)
				{
					if (!mVSMaterialUniformBuffer)
						mVSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", Domain));

					mVSMaterialUniformBuffer->PushUniform(Declaration);
				}
				else if (Domain == ShaderDomain::Pixel)
				{
					if (!mPSMaterialUniformBuffer)
						mPSMaterialUniformBuffer.reset(new OpenGLShaderUniformBufferDeclaration("", Domain));

					mPSMaterialUniformBuffer->PushUniform(Declaration);
				}
			}
		}
	}

	void OpenGLShader::ParseUniformStruct(const std::string& Block, ShaderDomain Domain)
	{
		std::vector<std::string> Tokens = Tokenize(Block);

		uint32_t Index = 0;
		Index++;
		std::string Name = Tokens[Index++];
		ShaderStruct* UniformStruct = new ShaderStruct(Name);
		Index++;

		while (Index < Tokens.size())
		{
			if (Tokens[Index] == "}")
				break;

			std::string Type = Tokens[Index++];
			std::string Name = Tokens[Index++];

			// Strip ; from name if present
			if (const char* s = strstr(Name.c_str(), ";"))
				Name = std::string(Name.c_str(), s - Name.c_str());

			uint32_t Count = 1;
			const char* Namestr = Name.c_str();
			if (const char* S = strstr(Namestr, "["))
			{
				Name = std::string(Namestr, S - Namestr);

				const char* End = strstr(Namestr, "]");
				std::string c(S + 1, End - S);
				Count = atoi(c.c_str());
			}
			ShaderUniformDeclaration* Field = new OpenGLShaderUniformDeclaration(Domain, OpenGLShaderUniformDeclaration::StringToType(Type), Name, Count);
			UniformStruct->AddField(Field);
		}
		mStructs.push_back(UniformStruct);
	}

	ShaderStruct* OpenGLShader::FindStruct(const std::string& Name)
	{
		for (ShaderStruct* S : mStructs)
		{
			if (S->GetName() == Name)
				return S;
		}
		return nullptr;
	}

	void OpenGLShader::ResolveUniforms()
	{
		glUseProgram(mRendererID);

		for (size_t i = 0; i < mVSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* Decl = (OpenGLShaderUniformBufferDeclaration*)mVSRendererUniformBuffers[i];
			const ShaderUniformList& Uniforms = Decl->GetUniformDeclarations();
			for (size_t j = 0; j < Uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* Uniform = (OpenGLShaderUniformDeclaration*)Uniforms[j];
				if (Uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& s = Uniform->GetShaderUniformStruct();
					const auto& Fields = s.GetFields();
					for (size_t k = 0; k < Fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* Field = (OpenGLShaderUniformDeclaration*)Fields[k];
						Field->mLocation = GetUniformLocation(Uniform->mName + "." + Field->mName);
					}
				}
				else
				{
					Uniform->mLocation = GetUniformLocation(Uniform->mName);
				}
			}
		}

		for (size_t i = 0; i < mPSRendererUniformBuffers.size(); i++)
		{
			OpenGLShaderUniformBufferDeclaration* Decl = (OpenGLShaderUniformBufferDeclaration*)mPSRendererUniformBuffers[i];
			const ShaderUniformList& Uniforms = Decl->GetUniformDeclarations();
			for (size_t j = 0; j < Uniforms.size(); j++)
			{
				OpenGLShaderUniformDeclaration* Uniform = (OpenGLShaderUniformDeclaration*)Uniforms[j];
				if (Uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
				{
					const ShaderStruct& S = Uniform->GetShaderUniformStruct();
					const auto& Fields = S.GetFields();
					for (size_t k = 0; k < Fields.size(); k++)
					{
						OpenGLShaderUniformDeclaration* Field = (OpenGLShaderUniformDeclaration*)Fields[k];
						Field->mLocation = GetUniformLocation(Uniform->mName + "." + Field->mName);
					}
				}
				else
				{
					Uniform->mLocation = GetUniformLocation(Uniform->mName);
				}
			}
		}

		{
			const auto& Decl = mVSMaterialUniformBuffer;
			if (Decl)
			{
				const ShaderUniformList& Uniforms = Decl->GetUniformDeclarations();
				for (size_t j = 0; j < Uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* Uniform = (OpenGLShaderUniformDeclaration*)Uniforms[j];
					if (Uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& S = Uniform->GetShaderUniformStruct();
						const auto& Fields = S.GetFields();
						for (size_t k = 0; k < Fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* Field = (OpenGLShaderUniformDeclaration*)Fields[k];
							Field->mLocation = GetUniformLocation(Uniform->mName + "." + Field->mName);
						}
					}
					else
					{
						Uniform->mLocation = GetUniformLocation(Uniform->mName);
					}
				}
			}
		}

		{
			const auto& Decl = mPSMaterialUniformBuffer;
			if (Decl)
			{
				const ShaderUniformList& Uniforms = Decl->GetUniformDeclarations();
				for (size_t j = 0; j < Uniforms.size(); j++)
				{
					OpenGLShaderUniformDeclaration* Uniform = (OpenGLShaderUniformDeclaration*)Uniforms[j];
					if (Uniform->GetType() == OpenGLShaderUniformDeclaration::Type::STRUCT)
					{
						const ShaderStruct& S = Uniform->GetShaderUniformStruct();
						const auto& Fields = S.GetFields();
						for (size_t k = 0; k < Fields.size(); k++)
						{
							OpenGLShaderUniformDeclaration* Field = (OpenGLShaderUniformDeclaration*)Fields[k];
							Field->mLocation = GetUniformLocation(Uniform->mName + "." + Field->mName);
						}
					}
					else
					{
						Uniform->mLocation = GetUniformLocation(Uniform->mName);
					}
				}
			}
		}

		uint32_t Sampler = 0;
		for (size_t i = 0; i < mResources.size(); i++)
		{
			OpenGLShaderResourceDeclaration* Resource = (OpenGLShaderResourceDeclaration*)mResources[i];
			int32_t Location = GetUniformLocation(Resource->mName);

			if (Resource->GetCount() == 1)
			{
				Resource->mRegister = Sampler;

				UploadUniformInt(Location, Sampler);

				Sampler++;
			}
			else if (Resource->GetCount() > 1)
			{
				Resource->mRegister = 0;
				uint32_t Count = Resource->GetCount();
				int* Samplers = new int[Count];

				for (uint32_t s = 0; s < Count; s++)
					Samplers[s] = s;

				UploadUniformIntArray(Location, Samplers, Count);
				delete[] Samplers;
			}
		}
	}

	void OpenGLShader::ValidateUniforms()
	{
		//TODO
	}

	GLenum OpenGLShader::ShaderTypeFromString(const std::string& Type)
	{
		if (Type == "vertex")
			return GL_VERTEX_SHADER;
		if (Type == "fragment" || Type == "pixel")
			return GL_FRAGMENT_SHADER;

		return GL_NONE;
	}

	void OpenGLShader::CompileAndUploadShader()
	{
		std::vector<GLuint> ShaderRendererIDs;

		GLuint Program = glCreateProgram();
		for (auto& KV : mShaderSource)
		{
			GLenum Type = KV.first;
			std::string& Source = KV.second;

			GLuint ShaderRendererID = glCreateShader(Type);
			const GLchar *SourceCstr = (const GLchar *)Source.c_str();
			glShaderSource(ShaderRendererID, 1, &SourceCstr, 0);

			glCompileShader(ShaderRendererID);

			GLint IsCompiled = 0;
			glGetShaderiv(ShaderRendererID, GL_COMPILE_STATUS, &IsCompiled);
			if (IsCompiled == GL_FALSE)
			{
				GLint MaxLength = 0;
				glGetShaderiv(ShaderRendererID, GL_INFO_LOG_LENGTH, &MaxLength);

				// The MaxLength includes the NULL character
				std::vector<GLchar> InfoLog(MaxLength);
				glGetShaderInfoLog(ShaderRendererID, MaxLength, &MaxLength, &InfoLog[0]);

				CL_CORE_LOG_ERROR("Shader compilation failed:\n{0}", &InfoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(ShaderRendererID);

				CL_CORE_ASSERT(false, "Failed");
			}

			ShaderRendererIDs.push_back(ShaderRendererID);
			glAttachShader(Program, ShaderRendererID);
		}

		// Link our Program
		glLinkProgram(Program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(Program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint MaxLength = 0;
			glGetProgramiv(Program, GL_INFO_LOG_LENGTH, &MaxLength);

			// The MaxLength includes the NULL character
			std::vector<GLchar> InfoLog(MaxLength);
			glGetProgramInfoLog(Program, MaxLength, &MaxLength, &InfoLog[0]);
			CL_CORE_LOG_ERROR("Shader compilation failed:\n{0}", &InfoLog[0]);

			// We don't need the Program anymore.
			glDeleteProgram(Program);
			// Don't leak shaders either.
			for (auto id : ShaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : ShaderRendererIDs)
			glDetachShader(Program, id);

		mRendererID = Program;
	}
}

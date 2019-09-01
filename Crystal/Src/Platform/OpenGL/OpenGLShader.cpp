#include "Crpch.h"

#include "OpenGLShader.h"

#include "glad/glad.h"

#include "glm/gtc/type_ptr.hpp"

#include "Crystal/Renderer/RendererAPI.h"

#include <string>
#include <sstream>
#include <limits>

namespace Crystal
{
	OpenGLShader::OpenGLShader(const std::string& FilePath)
		: mAssetPath(FilePath)
	{	
		size_t Found = FilePath.find_last_of("/\\");
		mName = Found != std::string::npos ? FilePath.substr(Found + 1) : FilePath;
		Reload();
	}

	OpenGLShader::~OpenGLShader()
	{
		CL_RENDER_S({
			glDeleteProgram(self->mRendererID);
		});
	}

	void OpenGLShader::Reload()
	{
		ReadShaderFromFile(mAssetPath);

		CL_RENDER_S({
			if (self->mRendererID)
				glDeleteShader(self->mRendererID);

			self->CompileAndUploadShader();
		});
	}

	void OpenGLShader::UploadUniformBuffer(const UniformBufferBase& uniformBuffer)
	{
		for (unsigned int i = 0; i < uniformBuffer.GetUniformCount(); i++)
		{
			const UniformDecl& decl = uniformBuffer.GetUniforms()[i];
			switch (decl.Type)
			{
			case UniformType::Float:
			{
				const std::string& Name = decl.Name;
				float Value = *(float*)(uniformBuffer.GetBuffer() + decl.Offset);

				CL_RENDER_S2(Name, Value, {
					self->UploadUniformFloat(Name, Value);
				});
			}
			case UniformType::Float3:
			{
				const std::string& Name = decl.Name;
				glm::vec3& Values = *(glm::vec3*)(uniformBuffer.GetBuffer() + decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformFloat3(Name, Values);
				});
			}
			case UniformType::Float4:
			{
				const std::string& Name = decl.Name;
				glm::vec4& Values = *(glm::vec4*)(uniformBuffer.GetBuffer() + decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformFloat4(Name, Values);
				});
			}
			case UniformType::Matrix4x4:
			{
				const std::string& Name = decl.Name;
				glm::mat4& Values = *(glm::mat4*)(uniformBuffer.GetBuffer() + decl.Offset);

				CL_RENDER_S2(Name, Values, {
					self->UploadUniformMat4(Name, Values);
				});
			}
			}
		}
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

	void OpenGLShader::Bind() const
	{
		CL_RENDER_S({
			glUseProgram(self->mRendererID);
		});
	}

	void OpenGLShader::UnBind() const
	{
		CL_RENDER_S({
			glUseProgram(0);
		});
	}

	GLint OpenGLShader::GetUniformLocation(const std::string& Name) const
	{
		auto Location = UniformLocations.find(Name);

		if (Location != UniformLocations.end())
			return Location->second;
		else
		{
			GLint Location = glGetUniformLocation(mRendererID, Name.c_str());
			UniformLocations.emplace(Name, Location);

			return Location;
		}
	}

	void OpenGLShader::UploadUniformInt(const std::string& Name, int Value)
	{
		glUseProgram(mRendererID);
		glUniform1i(GetUniformLocation(Name), Value);

	}

	void OpenGLShader::UploadUniformFloat(const std::string& Name, float Value)
	{
		glUseProgram(mRendererID);
		glUniform1f(GetUniformLocation(Name), Value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& Name, const glm::vec2& Value)
	{
		glUniform2f(GetUniformLocation(Name), Value.x, Value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& Name, const glm::vec3& Value)
	{
		glUseProgram(mRendererID);
		glUniform3f(GetUniformLocation(Name), Value.x, Value.y, Value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& Name, const glm::vec4& Value)
	{
		glUseProgram(mRendererID);
		glUniform4f(GetUniformLocation(Name), Value.x, Value.y, Value.z, Value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& Name, const glm::mat3& Value)
	{
		glUniformMatrix3fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& Name, const glm::mat4& Value)
	{
		glUseProgram(mRendererID);
		glUniformMatrix4fv(GetUniformLocation(Name), 1, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::ReadShaderFromFile(const std::string& Filepath)
	{
		std::ifstream in(Filepath, std::ios::in | std::ios::binary);
		if (in)
		{
			in.seekg(0, std::ios::end);
			mShaderSource.resize(in.tellg());
			in.seekg(0, std::ios::beg);
			in.read(&mShaderSource[0], mShaderSource.size());
			in.close();
		}
		else
		{
			CL_CORE_LOG_WARN("Could not read shader file {0}", Filepath);
		}
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
		std::unordered_map<unsigned int, std::string> shaderSources;

		const char* typeToken = "#type";
		size_t typeTokenLength = strlen(typeToken);
		size_t pos = mShaderSource.find(typeToken, 0);
		while (pos != std::string::npos)
		{
			size_t eol = mShaderSource.find_first_of("\r\n", pos);
			CL_CORE_ASSERT(eol != std::string::npos, "Syntax error");
			size_t begin = pos + typeTokenLength + 1;
			std::string type = mShaderSource.substr(begin, eol - begin);
			CL_CORE_ASSERT(type == "vertex" || type == "fragment" || type == "pixel", "Invalid shader type specified");

			size_t nextLinePos = mShaderSource.find_first_not_of("\r\n", eol);
			pos = mShaderSource.find(typeToken, nextLinePos);
			shaderSources[ShaderTypeFromString(type)] = mShaderSource.substr(nextLinePos, pos - (nextLinePos == std::string::npos ? mShaderSource.size() - 1 : nextLinePos));
		}

		std::vector<GLuint> shaderRendererIDs;

		unsigned int program = glCreateProgram();
		for (auto& kv : shaderSources)
		{
			unsigned int type = kv.first;
			std::string& source = kv.second;

			GLuint shaderRendererID = glCreateShader(type);
			const GLchar *sourceCstr = (const GLchar *)source.c_str();
			glShaderSource(shaderRendererID, 1, &sourceCstr, 0);

			glCompileShader(shaderRendererID);

			GLint isCompiled = 0;
			glGetShaderiv(shaderRendererID, GL_COMPILE_STATUS, &isCompiled);
			if (isCompiled == GL_FALSE)
			{
				GLint maxLength = 0;
				glGetShaderiv(shaderRendererID, GL_INFO_LOG_LENGTH, &maxLength);

				// The maxLength includes the NULL character
				std::vector<GLchar> infoLog(maxLength);
				glGetShaderInfoLog(shaderRendererID, maxLength, &maxLength, &infoLog[0]);

				CL_CORE_LOG_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

				// We don't need the shader anymore.
				glDeleteShader(shaderRendererID);

				CL_CORE_ASSERT(false, "Failed");
			}

			shaderRendererIDs.push_back(shaderRendererID);
			glAttachShader(program, shaderRendererID);
		}

		// Link our program
		glLinkProgram(program);

		// Note the different functions here: glGetProgram* instead of glGetShader*.
		GLint isLinked = 0;
		glGetProgramiv(program, GL_LINK_STATUS, (int *)&isLinked);
		if (isLinked == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetProgramiv(program, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetProgramInfoLog(program, maxLength, &maxLength, &infoLog[0]);
			CL_CORE_LOG_ERROR("Shader compilation failed:\n{0}", &infoLog[0]);

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			for (auto id : shaderRendererIDs)
				glDeleteShader(id);
		}

		// Always detach shaders after a successful link.
		for (auto id : shaderRendererIDs)
			glDetachShader(program, id);

		mRendererID = program;

		// Bind default texture unit
		UploadUniformInt("u_Texture", 0);

		// PBR shader textures
		UploadUniformInt("u_AlbedoTexture", 1);
		UploadUniformInt("u_NormalTexture", 2);
		UploadUniformInt("u_MetalnessTexture", 3);
		UploadUniformInt("u_RoughnessTexture", 4);

		UploadUniformInt("u_EnvRadianceTex", 10);
		UploadUniformInt("u_EnvIrradianceTex", 11);

		UploadUniformInt("u_BRDFLUTTexture", 15);
	}
}

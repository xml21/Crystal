#include "Crpch.h"
#include "OpenGLShader.h"

#include "glad/glad.h"

#include "glm/gtc/type_ptr.hpp"

static GLint Location;

namespace Crystal
{
	OpenGLShader::OpenGLShader(const std::string& VertexSrc, const std::string& FragmentSrc)
	{
		// Create an empty vertex shader handle
		GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);

		// Send the vertex shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		const GLchar *source = VertexSrc.c_str();
		glShaderSource(vertexShader, 1, &source, 0);

		// Compile the vertex shader
		glCompileShader(vertexShader);

		GLint isCompiled = 0;
		glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(vertexShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(vertexShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(vertexShader);

			CL_CORE_LOG_ERROR("{0}", infoLog.data());
			CL_CORE_ASSERT(false, "Vertex shader compilation error!");
			return;
		}

		// Create an empty fragment shader handle
		GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

		// Send the fragment shader source code to GL
		// Note that std::string's .c_str is NULL character terminated.
		source = FragmentSrc.c_str();
		glShaderSource(fragmentShader, 1, &source, 0);

		// Compile the fragment shader
		glCompileShader(fragmentShader);

		glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &isCompiled);
		if (isCompiled == GL_FALSE)
		{
			GLint maxLength = 0;
			glGetShaderiv(fragmentShader, GL_INFO_LOG_LENGTH, &maxLength);

			// The maxLength includes the NULL character
			std::vector<GLchar> infoLog(maxLength);
			glGetShaderInfoLog(fragmentShader, maxLength, &maxLength, &infoLog[0]);

			// We don't need the shader anymore.
			glDeleteShader(fragmentShader);
			// Either of them. Don't leak shaders.
			glDeleteShader(vertexShader);

			CL_CORE_LOG_ERROR("{0}", infoLog.data());
			CL_CORE_ASSERT(false, "Fragment shader compilation error!");
			return;
		}

		// Vertex and fragment shaders are successfully compiled.
		// Now time to link them together into a program.
		// Get a program object.
		mRendererID = glCreateProgram();

		GLuint program = mRendererID;

		// Attach our shaders to our program
		glAttachShader(program, vertexShader);
		glAttachShader(program, fragmentShader);

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

			// We don't need the program anymore.
			glDeleteProgram(program);
			// Don't leak shaders either.
			glDeleteShader(vertexShader);
			glDeleteShader(fragmentShader);

			CL_CORE_LOG_ERROR("{0}", infoLog.data());
			CL_CORE_ASSERT(false, "Shader linking error!");
			return;
		}

		// Always detach shaders after a successful link.
		glDetachShader(program, vertexShader);
		glDetachShader(program, fragmentShader);
	}

	OpenGLShader::~OpenGLShader()
	{
		glDeleteProgram(mRendererID);
	}

	void OpenGLShader::Bind() const
	{
		glUseProgram(mRendererID);
	}

	void OpenGLShader::UnBind() const
	{
		glUseProgram(0);
	}

	void OpenGLShader::CalculateUniformLocation(const std::string& Name)
	{
		Location = glGetUniformLocation(mRendererID, Name.c_str());
	}
	//-- TODO: Rework pre-calculate location for all UploadUniform functions for better performance --
	void OpenGLShader::UploadUniformInt(const std::string& Name, int Value)
	{
		CalculateUniformLocation(Name);
		glUniform1i(Location, Value);
	}

	void OpenGLShader::UploadUniformFloat(const std::string& Name, float Value)
	{
		CalculateUniformLocation(Name);
		glUniform1f(Location, Value);
	}

	void OpenGLShader::UploadUniformFloat2(const std::string& Name, const glm::vec2& Value)
	{
		CalculateUniformLocation(Name);
		glUniform2f(Location, Value.x, Value.y);
	}

	void OpenGLShader::UploadUniformFloat3(const std::string& Name, const glm::vec3& Value)
	{
		CalculateUniformLocation(Name);
		glUniform3f(Location, Value.x, Value.y, Value.z);
	}

	void OpenGLShader::UploadUniformFloat4(const std::string& Name, const glm::vec4& Value)
	{
		CalculateUniformLocation(Name);
		glUniform4f(Location, Value.x, Value.y, Value.z, Value.w);
	}

	void OpenGLShader::UploadUniformMat3(const std::string& Name, const glm::mat3& Value)
	{
		CalculateUniformLocation(Name);
		glUniformMatrix3fv(Location, 1, GL_FALSE, glm::value_ptr(Value));
	}

	void OpenGLShader::UploadUniformMat4(const std::string& Name, const glm::mat4& Value)
	{
		CalculateUniformLocation(Name);
		glUniformMatrix4fv(Location, 1, GL_FALSE, glm::value_ptr(Value));
	}
	//------------------------------------------------------------------------------------------------
}

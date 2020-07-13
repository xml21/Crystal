#include "Crpch.h"

#include "glad/glad.h"

#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	static void OpenGLLogMessage(GLenum Source, GLenum Type, GLuint Id, GLenum Severity, GLsizei Length, const GLchar* Message, const void* UserParam)
	{
		if (Severity != GL_DEBUG_SEVERITY_NOTIFICATION)
		{
			CL_CORE_LOG_ERROR("{0}", Message);
			CL_CORE_ASSERT(false, "");
		}
		else
		{
			CL_CORE_LOG_TRACE("{0}", Message);
		}
	}

	void RendererAPI::Init()
	{
		glDebugMessageCallback(OpenGLLogMessage, nullptr);
		glEnable(GL_DEBUG_OUTPUT);
		glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);

		unsigned int vao;
		glGenVertexArrays(1, &vao);
		glBindVertexArray(vao);

		glEnable(GL_DEPTH_TEST);
		glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
		glFrontFace(GL_CCW);

		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		auto& Caps = RendererAPI::GetCapabilities();

		Caps.Vendor = (const char*)glGetString(GL_VENDOR);
		Caps.Renderer = (const char*)glGetString(GL_RENDERER);
		Caps.Version = (const char*)glGetString(GL_VERSION);

		glGetIntegerv(GL_MAX_SAMPLES, &Caps.MaxSamples);
		glGetFloatv(GL_MAX_TEXTURE_MAX_ANISOTROPY, &Caps.MaxAnisotropy);

		GLenum Error = glGetError();
		while (Error != GL_NO_ERROR)
		{
			CL_CORE_LOG_ERROR("OpenGL ERROR {0}", Error);
			Error = glGetError();
		}

		LoadRequiredAssets();
	}

	void RendererAPI::SetClearColor(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
	}

	void RendererAPI::Clear(const glm::vec4& color)
	{
		glClearColor(color.r, color.g, color.b, color.a);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	}

	void RendererAPI::DrawIndexed(unsigned int Count, bool DepthTest)
	{
		DepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);

		glDrawElements(GL_TRIANGLES, Count, GL_UNSIGNED_INT, nullptr);

		DepthTest ? glEnable(GL_DEPTH_TEST) : glDisable(GL_DEPTH_TEST);
	}

	void RendererAPI::LoadRequiredAssets()
	{
		//TODO
	}
}
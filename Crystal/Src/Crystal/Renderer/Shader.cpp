#include "Crpch.h"

#include "Shader.h"
#include "Renderer.h"

#include "Platform/OpenGL/OpenGLShader.h"

namespace Crystal
{
	std::vector<Ref<Crystal::Shader>> Shader::sAllShaders;

	Ref<Shader> Shader::Create(const std::string& Filepath)
	{
		Ref<Shader> Result = nullptr;

		switch (RendererAPI::GetAPI())
		{
		case API::NONE: return nullptr;
		case API::OpenGL: Result = std::make_shared<OpenGLShader>(Filepath);
		}

		sAllShaders.push_back(Result);
		return Result;
	}

	

}

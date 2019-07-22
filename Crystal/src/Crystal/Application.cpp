#include "Crpch.h"

#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"

#include <glad/glad.h>
#include "Input.h"
#include "ImGui/ImGuiLayer.h"
#include "ImGui/imgui.h"

#include "Renderer/Shader.h"

#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Buffers/BufferLayout.h"

namespace Crystal
{
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Crystal::ShaderDataType::Float:    return GL_FLOAT;
		case Crystal::ShaderDataType::Float2:   return GL_FLOAT;
		case Crystal::ShaderDataType::Float3:   return GL_FLOAT;
		case Crystal::ShaderDataType::Float4:   return GL_FLOAT;
		case Crystal::ShaderDataType::Mat3:     return GL_FLOAT;
		case Crystal::ShaderDataType::Mat4:     return GL_FLOAT;
		case Crystal::ShaderDataType::Int:      return GL_INT;
		case Crystal::ShaderDataType::Int2:     return GL_INT;
		case Crystal::ShaderDataType::Int3:     return GL_INT;
		case Crystal::ShaderDataType::Int4:     return GL_INT;
		case Crystal::ShaderDataType::Bool:     return GL_BOOL;
		}

		CL_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	Application::Application()
	{
		CL_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		mImGuiLayer = std::make_shared<ImGuiLayer>();
		PushOverlay(mImGuiLayer);

		//Vertex Buffer
		glGenVertexArrays(1, &VertexArray);
		glBindVertexArray(VertexArray);

		float Vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};
		//TODO: consider using make_unique/shared
		mVertexBuffer.reset(VertexBuffer::Create(Vertices, sizeof(Vertices)));

		BufferLayout Layout = {
			{ShaderDataType::Float3, "aPosition"},
			{ShaderDataType::Float4, "aColor"}
		};

		mVertexBuffer->SetLayout(Layout);

		uint32_t index = 0;
		for (const auto& Element : Layout)
		{
			glEnableVertexAttribArray(index);
			glVertexAttribPointer(index, Element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(Element.GetType()), 
									Element.GetNormalized() ? GL_TRUE : GL_FALSE, Layout.GetStride(), (const void*)Element.GetOffset());
			index++;
		}

		//Index Buffer
		uint32_t Indices[3] = { 0, 1, 2 };

		//TODO: consider using make_unique/shared
		mIndexBuffer.reset(IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t)));

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColor;

			out vec3 vPosition;
			out vec4 vColor;

			void main()
			{
				vPosition = aPosition;
				vColor = aColor;
				gl_Position = vec4(aPosition, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;
			in vec4 vColor;

			void main()
			{
				color = vColor;
			}
		)";

		mShader = std::make_unique<Shader>(vertexSrc, fragmentSrc);
	}

	void Application::Run()
	{
		while (mRunning)
		{
			glClearColor(0.1f, 0.1f, 0.1f, 1);
			glClear(GL_COLOR_BUFFER_BIT);
			
			mShader->Bind();
			glBindVertexArray(VertexArray);

			//Request draw call
			glDrawElements(GL_TRIANGLES, mIndexBuffer->GetCount(), GL_UNSIGNED_INT, nullptr);

			for (std::shared_ptr<Layer> layer : mLayerStack)
				layer->OnUpdate();

			mImGuiLayer->Begin();
			for (std::shared_ptr<Layer> layer : mLayerStack)
				layer->OnImGuiRender();
			mImGuiLayer->End();

			mWindow->OnUpdate();
		}
	}

	void Application::OnEvent(Event& e)
	{
		EventDispatcher dispatcher(e);
		dispatcher.Dispatch<WindowCloseEvent>(BIND_EVENT_FN(OnWindowClose));

		for (auto it = mLayerStack.end(); it != mLayerStack.begin(); )
		{
			(*--it)->OnEvent(e);
			if (e.bHandled)
				break;
		}
	}

	void Application::PushLayer(std::shared_ptr<Layer> layer)
	{
		mLayerStack.PushLayer(layer);
		layer->OnAttach();
	}

	void Application::PushOverlay(std::shared_ptr<Layer> overlay)
	{
		mLayerStack.PushOverlay(overlay);
		overlay->OnAttach();
	}

	bool Application::OnWindowClose(WindowCloseEvent& e)
	{
		mRunning = false;
		return true;
	}

}
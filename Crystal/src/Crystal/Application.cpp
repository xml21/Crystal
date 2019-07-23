#include "Crpch.h"

#include "Application.h"

#include "Crystal/Events/ApplicationEvent.h"
#include "Crystal/Log.h"

#include "glad/glad.h"
#include "Input.h"
#include "ImGui/ImGuiLayer.h"
#include "ImGui/imgui.h"

#include "Renderer/Shader.h"
#include "Renderer/VertexArray.h"
#include "Renderer/Buffers/VertexBuffer.h"
#include "Renderer/Buffers/IndexBuffer.h"
#include "Renderer/Buffers/BufferLayout.h"
#include "Renderer/Renderer.h"
#include "Renderer/RenderCommand.h"

namespace Crystal
{
	#define BIND_EVENT_FN(x) std::bind(&Application::x, this, std::placeholders::_1)

	Application* Application::sInstance = nullptr;

	Application::Application()
	{
		CL_CORE_ASSERT(!sInstance, "Application already exists!");
		sInstance = this;

		mWindow = std::unique_ptr<Window>(Window::Create());
		mWindow->SetEventCallback(BIND_EVENT_FN(OnEvent));

		mImGuiLayer = std::make_shared<ImGuiLayer>();
		PushOverlay(mImGuiLayer);

		mVertexArray = VertexArray::Create();

		float Vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		mVertexBuffer = VertexBuffer::Create(Vertices, sizeof(Vertices));

		BufferLayout Layout = {
			{ShaderDataType::Float3, "aPosition"},
			{ShaderDataType::Float4, "aColor"}
		};

		mVertexBuffer->SetLayout(Layout);
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		//Index Buffer
		uint32_t Indices[3] = { 0, 1, 2 };


		mIndexBuffer = IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));
		mVertexArray->SetIndexBuffer(mIndexBuffer);

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
			RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
			RenderCommand::Clear();

			Renderer::BeginScene();
			
			mShader->Bind();

			Renderer::Submit(mVertexArray); //Request draw call
			
			Renderer::EndScene();

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
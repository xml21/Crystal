#include "Crystal.h"
/**
* Singleton class. Client application
*/

class ExampleLayer : public Crystal::Layer 
{
public:
	ExampleLayer() : Layer(), mCamera(-1.6f, 1.6f, -0.9f, 0.9f), mCameraPosition(0.0f)
	{
		mVertexArray = Crystal::VertexArray::Create();

		float Vertices[3 * 7] = {
			-0.5f, -0.5f, 0.0f, 0.8f, 0.2f, 0.8f, 1.0f,
			 0.5f, -0.5f, 0.0f, 0.2f, 0.3f, 0.8f, 1.0f,
			 0.0f,  0.5f, 0.0f, 0.8f, 0.8f, 0.2f, 1.0f
		};

		mVertexBuffer = Crystal::VertexBuffer::Create(Vertices, sizeof(Vertices));

		Crystal::BufferLayout Layout = {
			{Crystal::ShaderDataType::Float3, "aPosition"},
			{Crystal::ShaderDataType::Float4, "aColor"}
		};

		mVertexBuffer->SetLayout(Layout);
		mVertexArray->AddVertexBuffer(mVertexBuffer);

		//Index Buffer
		uint32_t Indices[3] = { 0, 1, 2 };


		mIndexBuffer = Crystal::IndexBuffer::Create(Indices, sizeof(Indices) / sizeof(uint32_t));
		mVertexArray->SetIndexBuffer(mIndexBuffer);

		std::string vertexSrc = R"(
			#version 330 core
			
			layout(location = 0) in vec3 aPosition;
			layout(location = 1) in vec4 aColor;

			uniform mat4 u_ViewProjection;

			out vec3 vPosition;
			out vec4 vColor;

			void main()
			{
				vPosition = aPosition;
				vColor = aColor;
				gl_Position = u_ViewProjection * vec4(aPosition, 1.0);	
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

		mShader = std::make_unique<Crystal::Shader>(vertexSrc, fragmentSrc);
	}

	void OnUpdate() override
	{
		Crystal::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Crystal::RenderCommand::Clear();

		//---------------- Camera ----------------------
		if (Crystal::Input::IsKeyPressed(CL_KEY_A))
			mCameraPosition.x -= mCameraMoveSpeed;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_D))
			mCameraPosition.x += mCameraMoveSpeed;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_W))
			mCameraPosition.y += mCameraMoveSpeed;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_S))
			mCameraPosition.y -= mCameraMoveSpeed;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_Q))
			mCameraRotation -= mCameraRotationSpeed;

		mCamera.SetPosition(mCameraPosition);
		mCamera.SetRotation(mCameraRotation);
		//----------------------------------------------

		Crystal::Renderer::BeginScene(mCamera);
		Crystal::Renderer::Submit(mShader, mVertexArray); //Request draw call
		Crystal::Renderer::EndScene();
	}

private:
	std::shared_ptr<Crystal::Shader> mShader;
	std::shared_ptr<Crystal::VertexBuffer> mVertexBuffer;
	std::shared_ptr<Crystal::IndexBuffer> mIndexBuffer;
	std::shared_ptr<Crystal::VertexArray> mVertexArray;

	Crystal::OrthographicCamera mCamera;

	glm::vec3 mCameraPosition;

	float mCameraMoveSpeed = .1f;

	float mCameraRotation = 0.0f;
	float mCameraRotationSpeed = 2.0f;
};

class Sandbox : public Crystal::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_shared<ExampleLayer>());
	}

	~Sandbox()
	{

	}
};

Crystal::Application* Crystal::CreateApplication()
{
	return new Sandbox();
}
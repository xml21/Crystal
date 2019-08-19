#include "Crystal.h"

#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/random.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "imgui/imgui.h"

#include "Platform/OpenGL/OpenGLShader.h"

/**
* Singleton class. Client application
*/

class ExampleLayer : public Crystal::Layer 
{
public:
	ExampleLayer() : Layer(), mCamera(-1.6f, 1.6f, -0.9f, 0.9f), mCameraPosition(0.0f), mMovableObjectPosition(0.0f)
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

			uniform mat4 uViewProjection;
			uniform mat4 uTransform;

			out vec3 vPosition;
			out vec4 vColor;

			void main()
			{
				vPosition = aPosition;
				vColor = aColor;
				gl_Position = uViewProjection * uTransform * vec4(aPosition, 1.0);	
			}
		)";

		std::string fragmentSrc = R"(
			#version 330 core
			
			layout(location = 0) out vec4 color;

			in vec3 vPosition;

			uniform vec3 uColor;

			void main()
			{
				color = vec4(uColor, 1.0);
			}
		)";

		mShader = Crystal::Shader::Create(vertexSrc, fragmentSrc);

		// ---------------------------- Preparing random object transforms ----------------------------
		mObjectTransforms = new glm::mat4[mObjectsAmount];
		mObjectColors = new glm::vec3[mObjectsAmount];

		for (int i = 0; i < mObjectsAmount; i++)
		{
			glm::vec3 Position(glm::linearRand(-5.0f, 5.0f) * 0.11f, glm::linearRand(-5.0f, 5.0f) * 0.11f, 0.0f);
			glm::mat4 Transform = glm::translate(glm::mat4(1.0f), Position) * mObjectsScale;
		
			mObjectColors[i] = glm::vec4(glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), glm::linearRand(0.0f, 1.0f), 0.0f);
			mObjectTransforms[i] = glm::mat4(Transform);
		}
		// --------------------------------------------------------------------------------------------
	}

	void OnUpdate(float DeltaTime) override
	{
		Crystal::RenderCommand::SetClearColor({ 0.1f, 0.1f, 0.1f, 1 });
		Crystal::RenderCommand::Clear();

		//---------------- Camera -----------------------------------
		// Camera movement
		if (Crystal::Input::IsKeyPressed(CL_KEY_A))
			mCameraPosition.x -= mCameraMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_D))
			mCameraPosition.x += mCameraMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_W))
			mCameraPosition.y += mCameraMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_S))
			mCameraPosition.y -= mCameraMoveSpeed * DeltaTime;
		// Camera rotation
		else if (Crystal::Input::IsKeyPressed(CL_KEY_Q))
			mCameraRotation -= mCameraRotationSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_E))
			mCameraRotation += mCameraRotationSpeed * DeltaTime;
		// Object movement
		else if (Crystal::Input::IsKeyPressed(CL_KEY_F))
			mMovableObjectPosition.x -= mObjectMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_H))
			mMovableObjectPosition.x += mObjectMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_T))
			mMovableObjectPosition.y += mObjectMoveSpeed * DeltaTime;
		else if (Crystal::Input::IsKeyPressed(CL_KEY_G))
			mMovableObjectPosition.y -= mObjectMoveSpeed * DeltaTime;

		mCamera.SetPosition(mCameraPosition);
		mCamera.SetRotation(mCameraRotation);
		//-----------------------------------------------------------
		
		Crystal::Renderer::BeginScene(mCamera);

		//------------------------- TODO: Prepare proper abstraction of Shader.cpp class ---------------------
		Crystal::Ref<Crystal::OpenGLShader> OpenGLShader = std::dynamic_pointer_cast<Crystal::OpenGLShader>(mShader);
		OpenGLShader->Bind();
		//----------------------------------------------------------------------------------------------------

		//---------------------------------- Rendering batch of triangles ------------------------------------
		for (int i = 0; i < mObjectsAmount; i++)
		{		
			OpenGLShader->UploadUniformFloat3("uColor", mObjectColors[i] * mTintColor);
			Crystal::Renderer::Submit(OpenGLShader, mVertexArray, mObjectTransforms[i]); //Request draw call
		}
		//---------------------------------- Rendering batch of triangles ------------------------------------

		//------------------------------------- Unique, movable triangle -------------------------------------
		glm::mat4 ObjectTransform = glm::translate(glm::mat4(1.0f), mMovableObjectPosition) * mObjectsScale;
		Crystal::Renderer::Submit(mShader, mVertexArray, ObjectTransform); //Request draw call
		//------------------------------------- Unique, movable triangle -------------------------------------

		Crystal::Renderer::EndScene();
	}

	virtual void OnImGuiRender() override
	{
		ImGui::Begin("Color Settings");
		ImGui::ColorEdit3("Tint Color", glm::value_ptr(mTintColor));
		ImGui::End();
	}

private:
	Crystal::Ref<Crystal::Shader> mShader;
	Crystal::Ref<Crystal::VertexBuffer> mVertexBuffer;
	Crystal::Ref<Crystal::IndexBuffer> mIndexBuffer;
	Crystal::Ref<Crystal::VertexArray> mVertexArray;

	Crystal::OrthographicCamera mCamera;

	glm::vec3 mCameraPosition;

	float mCameraMoveSpeed = 1.0f;
	float mObjectMoveSpeed = 1.0f;

	float mCameraRotation = 0.0f;
	float mCameraRotationSpeed = 100.0f;

	glm::vec3 mMovableObjectPosition;
	glm::mat4* mObjectTransforms;
	glm::vec3* mObjectColors;
	glm::vec3 mTintColor = glm::vec3(1.0f, 1.0f, 1.0f);

	const glm::mat4 mObjectsScale = glm::scale(glm::mat4(1.0f), glm::vec3(0.1f));
	const int mObjectsAmount = 10;
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
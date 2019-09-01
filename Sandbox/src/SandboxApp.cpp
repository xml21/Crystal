#include "Crystal.h"

#include "Crystal/ImGui/ImGuiLayer.h"

#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/matrix_decompose.hpp>
#include <glm/gtx/quaternion.hpp>
/**
* Singleton class. Client application
*/

static void ImGuiShowHelpMarker(const char* desc)
{
	ImGui::TextDisabled("(?)");
	if (ImGui::IsItemHovered())
	{
		ImGui::BeginTooltip();
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted(desc);
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
}

class EditorLayer : public Crystal::Layer 
{
public:
	EditorLayer()
		: mScene(Scene::Spheres), mCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{
	}

	virtual ~EditorLayer()
	{
	}

	virtual void OnAttach() override
	{
		mSimplePBRShader = Crystal::Shader::Create("Assets/Shaders/simplepbr.glsl");
		mQuadShader = Crystal::Shader::Create("Assets/Shaders/quad.glsl");
		mHDRShader = Crystal::Shader::Create("Assets/Shaders/hdr.glsl");
		mMesh = std::make_shared<Crystal::Mesh>("Assets/Meshes/cerberus.fbx");
		mSphereMesh = std::make_shared<Crystal::Mesh>("Assets/Models/Sphere.fbx");

		// Editor
		mCheckerboardTex = Crystal::Texture2D::Create("Assets/Editor/Checkerboard.tga");

		// Environment
		mEnvironmentCubeMap = Crystal::TextureCube::Create("Assets/Textures/environments/Arches_E_PineTree_Radiance.tga");
		mEnvironmentIrradiance = Crystal::TextureCube::Create("Assets/Textures/environments/Arches_E_PineTree_Irradiance.tga");
		mBRDFLUT = Crystal::Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

		mFramebuffer = Crystal::Framebuffer::Create(1280, 720, Crystal::FramebufferFormat::RGBA16F);
		mFinalPresentBuffer = Crystal::Framebuffer::Create(1280, 720, Crystal::FramebufferFormat::RGBA8);

		// Create Quad
		float x = -1, y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* data = new QuadVertex[4];

		data[0].Position = glm::vec3(x, y, 0);
		data[0].TexCoord = glm::vec2(0, 0);

		data[1].Position = glm::vec3(x + width, y, 0);
		data[1].TexCoord = glm::vec2(1, 0);

		data[2].Position = glm::vec3(x + width, y + height, 0);
		data[2].TexCoord = glm::vec2(1, 1);

		data[3].Position = glm::vec3(x, y + height, 0);
		data[3].TexCoord = glm::vec2(0, 1);

		mVertexBuffer = Crystal::VertexBuffer::Create();
		mVertexBuffer->SetData(data, 4 * sizeof(QuadVertex));

		uint32_t* indices = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
		mIndexBuffer = Crystal::IndexBuffer::Create();
		mIndexBuffer->SetData(indices, 6 * sizeof(unsigned int));

		mLight.Direction = { -0.5f, -0.5f, 1.0f };
		mLight.Radiance = { 1.0f, 1.0f, 1.0f };
	}

	virtual void OnDetach() override
	{
	}

	virtual void OnUpdate() override
	{
		// THINGS TO LOOK AT:
		// - BRDF LUT
		// - Cubemap mips and filtering
		// - Tonemapping and proper HDR pipeline
		using namespace Crystal;
		using namespace glm;

		mCamera.Update();
		auto viewProjection = mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix();

		mFramebuffer->Bind();
		Renderer::Clear(mClearColor[0], mClearColor[1], mClearColor[2], mClearColor[3]);

		Crystal::UniformBufferDeclaration<sizeof(mat4), 1> quadShaderUB;
		quadShaderUB.Push("u_InverseVP", inverse(viewProjection));
		mQuadShader->UploadUniformBuffer(quadShaderUB);

		mQuadShader->Bind();
		mEnvironmentIrradiance->Bind(0);
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();
		Renderer::DrawIndexed(mIndexBuffer->GetCount(), false);

		Crystal::UniformBufferDeclaration<sizeof(mat4) * 2 + sizeof(vec3) * 4 + sizeof(float) * 8, 14> simplePbrShaderUB;
		simplePbrShaderUB.Push("u_ViewProjectionMatrix", viewProjection);
		simplePbrShaderUB.Push("u_ModelMatrix", mat4(1.0f));
		simplePbrShaderUB.Push("u_AlbedoColor", mAlbedoInput.Color);
		simplePbrShaderUB.Push("u_Metalness", mMetalnessInput.Value);
		simplePbrShaderUB.Push("u_Roughness", mRoughnessInput.Value);
		simplePbrShaderUB.Push("lights.Direction", mLight.Direction);
		simplePbrShaderUB.Push("lights.Radiance", mLight.Radiance * mLightMultiplier);
		simplePbrShaderUB.Push("u_CameraPosition", mCamera.GetPosition());
		simplePbrShaderUB.Push("u_RadiancePrefilter", mRadiancePrefilter ? 1.0f : 0.0f);
		simplePbrShaderUB.Push("u_AlbedoTexToggle", mAlbedoInput.UseTexture ? 1.0f : 0.0f);
		simplePbrShaderUB.Push("u_NormalTexToggle", mNormalInput.UseTexture ? 1.0f : 0.0f);
		simplePbrShaderUB.Push("u_MetalnessTexToggle", mMetalnessInput.UseTexture ? 1.0f : 0.0f);
		simplePbrShaderUB.Push("u_RoughnessTexToggle", mRoughnessInput.UseTexture ? 1.0f : 0.0f);
		simplePbrShaderUB.Push("u_EnvMapRotation", mEnvMapRotation);
		mSimplePBRShader->UploadUniformBuffer(simplePbrShaderUB);

		mEnvironmentCubeMap->Bind(10);
		mEnvironmentIrradiance->Bind(11);
		mBRDFLUT->Bind(15);

		mSimplePBRShader->Bind();
		if (mAlbedoInput.TextureMap)
			mAlbedoInput.TextureMap->Bind(1);
		if (mNormalInput.TextureMap)
			mNormalInput.TextureMap->Bind(2);
		if (mMetalnessInput.TextureMap)
			mMetalnessInput.TextureMap->Bind(3);
		if (mRoughnessInput.TextureMap)
			mRoughnessInput.TextureMap->Bind(4);

		if (mScene == Scene::Spheres)
		{
			// Metals
			float roughness = 0.0f;
			float x = -88.0f;
			for (int i = 0; i < 8; i++)
			{
				mSimplePBRShader->SetMat4("u_ModelMatrix", translate(mat4(1.0f), vec3(x, 0.0f, 0.0f)));
				mSimplePBRShader->SetFloat("u_Roughness", roughness);
				mSimplePBRShader->SetFloat("u_Metalness", 1.0f);
				mSphereMesh->Render();

				roughness += 0.15f;
				x += 22.0f;
			}

			// Dielectrics
			roughness = 0.0f;
			x = -88.0f;
			for (int i = 0; i < 8; i++)
			{
				mSimplePBRShader->SetMat4("u_ModelMatrix", translate(mat4(1.0f), vec3(x, 22.0f, 0.0f)));
				mSimplePBRShader->SetFloat("u_Roughness", roughness);
				mSimplePBRShader->SetFloat("u_Metalness", 0.0f);
				mSphereMesh->Render();

				roughness += 0.15f;
				x += 22.0f;
			}

		}
		else if (mScene == Scene::Model)
		{
			mMesh->Render();
		}

		mFramebuffer->Unbind();

		mFinalPresentBuffer->Bind();
		mHDRShader->Bind();
		mHDRShader->SetFloat("u_Exposure", mExposure);
		mFramebuffer->BindTexture();
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();
		Renderer::DrawIndexed(mIndexBuffer->GetCount(), false);
		mFinalPresentBuffer->Unbind();
	}

	virtual void OnImGuiRender() override
	{
		static bool p_open = true;

		static bool opt_fullscreen_persistant = true;
		static ImGuiDockNodeFlags opt_flags = ImGuiDockNodeFlags_None;
		bool opt_fullscreen = opt_fullscreen_persistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags window_flags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (opt_fullscreen)
		{
			ImGuiViewport* viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(viewport->Pos);
			ImGui::SetNextWindowSize(viewport->Size);
			ImGui::SetNextWindowViewport(viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			window_flags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode)
			window_flags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &p_open, window_flags);
		ImGui::PopStyleVar();

		if (opt_fullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID dockspace_id = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(dockspace_id, ImVec2(0.0f, 0.0f), opt_flags);
		}

		//Log
		{
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Log");
			ImGui::Text("[18:57:20] Crystal: Initialized Core Log\n"
				"[18:57:20] Application: Initialized Client Log\n"
				"[18:57:21] Crystal : OpenGL Info :\n"
				"[18:57:21] Crystal : Vendor : NVIDIA Corporation\n"
				"[18:57:21] Crystal : Renderer : GeForce GTX 1080 8GB / PCIe / SSE2\n"
				"[18:57:21] Crystal : Version : 4.6.0 NVIDIA 418.91\n"
				"[18:57:21] Crystal : Loading mesh : Assets / Meshes / Model.fbx\n"
				"[18:57:21] Crystal : Loading mesh : Assets / Models / Sphere.fbx\n");
			ImGui::End();
		}

		//Hierarchy view
		ImGui::Begin("Scene");
		if (ImGui::TreeNode("Hierarchy"))
		{
			ImGui::Text("Sphere01");
			ImGui::Text("Sphere02");
			ImGui::Text("Sphere03");
			ImGui::Text("Sphere04");
			ImGui::Text("Sphere05");
			ImGui::Text("Sphere06");
			ImGui::Text("Sphere07");
			ImGui::Text("Sphere08");
			ImGui::TreePop();
		}
		ImGui::End();
		

		// Editor Panel ------------------------------------------------------------------------------
		ImGui::Begin("Settings");
		if (ImGui::TreeNode("Shaders"))
		{
			auto& shaders = Crystal::Shader::sAllShaders;
			for (auto& shader : shaders)
			{
				if (ImGui::TreeNode(shader->GetName().c_str()))
				{
					std::string buttonName = "Reload##" + shader->GetName();
					if (ImGui::Button(buttonName.c_str()))
						shader->Reload();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::RadioButton("Spheres", (int*)&mScene, (int)Scene::Spheres);
		ImGui::SameLine();
		ImGui::RadioButton("Model", (int*)&mScene, (int)Scene::Model);

		ImGui::ColorEdit4("Clear Color", mClearColor);

		ImGui::SliderFloat3("Light Dir", glm::value_ptr(mLight.Direction), -1, 1);
		ImGui::ColorEdit3("Light Radiance", glm::value_ptr(mLight.Radiance));
		ImGui::SliderFloat("Light Multiplier", &mLightMultiplier, 0.0f, 5.0f);
		ImGui::SliderFloat("Exposure", &mExposure, 0.0f, 10.0f);
		auto cameraForward = mCamera.GetForwardDirection();
		ImGui::Text("Camera Forward: %.2f, %.2f, %.2f", cameraForward.x, cameraForward.y, cameraForward.z);

		ImGui::Separator();
		{
			ImGui::Text("Mesh");
			std::string fullpath = mMesh ? mMesh->GetFilePath() : "None";
			size_t found = fullpath.find_last_of("/\\");
			std::string path = found != std::string::npos ? fullpath.substr(found + 1) : fullpath;
			ImGui::Text(path.c_str()); ImGui::SameLine();
			if (ImGui::Button("...##Mesh"))
			{
				std::string Filename = Crystal::Application::Get().OpenFile("");
				if (Filename != "")
					mMesh = (std::make_shared<Crystal::Mesh>(Filename));
			}
		}
		ImGui::Separator();

		ImGui::Text("Shader Parameters");
		ImGui::Checkbox("Radiance Prefiltering", &mRadiancePrefilter);
		ImGui::SliderFloat("Env Map Rotation", &mEnvMapRotation, -360.0f, 360.0f);

		ImGui::Separator();

		// Textures ------------------------------------------------------------------------------
		{
			// Albedo
			if (ImGui::CollapsingHeader("Albedo", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(mAlbedoInput.TextureMap ? (void*)mAlbedoInput.TextureMap->GetRendererID() : (void*)mCheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (mAlbedoInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(mAlbedoInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)mAlbedoInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Crystal::Application::Get().OpenFile("");
						if (filename != "")
							mAlbedoInput.TextureMap = Crystal::Texture2D::Create(filename, mAlbedoInput.SRGB);
					}
				}
				ImGui::SameLine();
				ImGui::BeginGroup();
				ImGui::Checkbox("Use##AlbedoMap", &mAlbedoInput.UseTexture);
				if (ImGui::Checkbox("sRGB##AlbedoMap", &mAlbedoInput.SRGB))
				{
					if (mAlbedoInput.TextureMap)
						mAlbedoInput.TextureMap = Crystal::Texture2D::Create(mAlbedoInput.TextureMap->GetPath(), mAlbedoInput.SRGB);
				}
				ImGui::EndGroup();
				ImGui::SameLine();
				ImGui::ColorEdit3("Color##Albedo", glm::value_ptr(mAlbedoInput.Color), ImGuiColorEditFlags_NoInputs);
			}
		}
		{
			// Normals
			if (ImGui::CollapsingHeader("Normals", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(mNormalInput.TextureMap ? (void*)mNormalInput.TextureMap->GetRendererID() : (void*)mCheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (mNormalInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(mNormalInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)mNormalInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Crystal::Application::Get().OpenFile("");
						if (filename != "")
							mNormalInput.TextureMap = Crystal::Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##NormalMap", &mNormalInput.UseTexture);
			}
		}
		{
			// Metalness
			if (ImGui::CollapsingHeader("Metalness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(mMetalnessInput.TextureMap ? (void*)mMetalnessInput.TextureMap->GetRendererID() : (void*)mCheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (mMetalnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(mMetalnessInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)mMetalnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Crystal::Application::Get().OpenFile("");
						if (filename != "")
							mMetalnessInput.TextureMap = Crystal::Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##MetalnessMap", &mMetalnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##MetalnessInput", &mMetalnessInput.Value, 0.0f, 1.0f);
			}
		}
		{
			// Roughness
			if (ImGui::CollapsingHeader("Roughness", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
			{
				ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(10, 10));
				ImGui::Image(mRoughnessInput.TextureMap ? (void*)mRoughnessInput.TextureMap->GetRendererID() : (void*)mCheckerboardTex->GetRendererID(), ImVec2(64, 64));
				ImGui::PopStyleVar();
				if (ImGui::IsItemHovered())
				{
					if (mRoughnessInput.TextureMap)
					{
						ImGui::BeginTooltip();
						ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
						ImGui::TextUnformatted(mRoughnessInput.TextureMap->GetPath().c_str());
						ImGui::PopTextWrapPos();
						ImGui::Image((void*)mRoughnessInput.TextureMap->GetRendererID(), ImVec2(384, 384));
						ImGui::EndTooltip();
					}
					if (ImGui::IsItemClicked())
					{
						std::string filename = Crystal::Application::Get().OpenFile("");
						if (filename != "")
							mRoughnessInput.TextureMap = Crystal::Texture2D::Create(filename);
					}
				}
				ImGui::SameLine();
				ImGui::Checkbox("Use##RoughnessMap", &mRoughnessInput.UseTexture);
				ImGui::SameLine();
				ImGui::SliderFloat("Value##RoughnessInput", &mRoughnessInput.Value, 0.0f, 1.0f);
			}
		}

		ImGui::Separator();

		ImGui::End();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		auto viewportSize = ImGui::GetContentRegionAvail();
		mFramebuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		mFinalPresentBuffer->Resize((uint32_t)viewportSize.x, (uint32_t)viewportSize.y);
		mCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), viewportSize.x, viewportSize.y, 0.1f, 10000.0f));
		ImGui::Image((void*)mFinalPresentBuffer->GetColorAttachmentRendererID(), viewportSize, { 0, 1 }, { 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Docking"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &opt_fullscreen_persistant);

				if (ImGui::MenuItem("Flag: NoSplit", "", (opt_flags & ImGuiDockNodeFlags_NoSplit) != 0))                 opt_flags ^= ImGuiDockNodeFlags_NoSplit;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (opt_flags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  opt_flags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: NoResize", "", (opt_flags & ImGuiDockNodeFlags_NoResize) != 0))                opt_flags ^= ImGuiDockNodeFlags_NoResize;
				if (ImGui::MenuItem("Flag: PassthruDockspace", "", (opt_flags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))       opt_flags ^= ImGuiDockNodeFlags_PassthruCentralNode;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (opt_flags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          opt_flags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				if (ImGui::MenuItem("Close DockSpace", NULL, false, p_open != NULL))
					p_open = false;
				ImGui::EndMenu();
			}
			ImGuiShowHelpMarker(
				"You can _always_ dock _any_ window into another by holding the SHIFT key while moving a window. Try it now!" "\n"
				"This demo app has nothing to do with it!" "\n\n"
				"This demo app only demonstrate the use of ImGui::DockSpace() which allows you to manually create a docking node _within_ another window. This is useful so you can decorate your main application window (e.g. with a menu bar)." "\n\n"
				"ImGui::DockSpace() comes with one hard constraint: it needs to be submitted _before_ any window which may be docked into it. Therefore, if you use a dock spot as the central point of your application, you'll probably want it to be part of the very first window you are submitting to imgui every frame." "\n\n"
				"(NB: because of this constraint, the implicit \"Debug\" window can not be docked into an explicit DockSpace() node, because that window is submitted as part of the NewFrame() call. An easy workaround is that you can create your own implicit \"Debug##2\" window after calling DockSpace() and leave it in the window stack for anyone to use.)"
			);

			ImGui::EndMenuBar();
		}

		ImGui::End();
	}

	virtual void OnEvent(Crystal::Event& event) override
	{
	}

private:
	std::shared_ptr<Crystal::Shader> mSimplePBRShader;
	std::shared_ptr<Crystal::Shader> mQuadShader;
	std::shared_ptr<Crystal::Shader> mHDRShader;
	std::shared_ptr<Crystal::Mesh> mMesh;
	std::shared_ptr<Crystal::Mesh> mSphereMesh;
	std::shared_ptr<Crystal::Texture2D> mBRDFLUT;

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.550, 0.556, 0.554 };
		std::shared_ptr<Crystal::Texture2D> TextureMap;
		bool SRGB = true;
		bool UseTexture = false;
	};
	AlbedoInput mAlbedoInput;

	struct NormalInput
	{
		std::shared_ptr<Crystal::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	NormalInput mNormalInput;

	struct MetalnessInput
	{
		float Value = 1.0f;
		std::shared_ptr<Crystal::Texture2D> TextureMap;
		bool UseTexture = false;
	};
	MetalnessInput mMetalnessInput;

	struct RoughnessInput
	{
		float Value = 0.5f;
		std::shared_ptr<Crystal::Texture2D> TextureMap;
		bool UseTexture = false;
	};

	RoughnessInput mRoughnessInput;

	std::shared_ptr<Crystal::Framebuffer> mFramebuffer, mFinalPresentBuffer;

	std::shared_ptr<Crystal::VertexBuffer> mVertexBuffer;
	std::shared_ptr<Crystal::IndexBuffer> mIndexBuffer;
	std::shared_ptr<Crystal::TextureCube> mEnvironmentCubeMap, mEnvironmentIrradiance;

	Crystal::Camera mCamera;

	float mClearColor[4];

	struct Light
	{
		glm::vec3 Direction;
		glm::vec3 Radiance;
	};

	Light mLight;
	float mLightMultiplier = 0.3f;

	// PBR params
	float mExposure = 1.0f;

	bool mRadiancePrefilter = false;

	float mEnvMapRotation = 0.0f;

	enum class Scene : uint32_t
	{
		Spheres = 0, Model = 1
	};
	Scene mScene;

	// Editor resources
	std::shared_ptr<Crystal::Texture2D> mCheckerboardTex;
};

class Sandbox : public Crystal::Application
{
public:
	Sandbox()
	{
		PushLayer(std::make_shared<EditorLayer>());
	}

	~Sandbox()
	{

	}
};

Crystal::Application* Crystal::CreateApplication()
{
	return new Sandbox();
}
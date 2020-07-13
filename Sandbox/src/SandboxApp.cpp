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
		: mScene(Scene::Model), mCamera(glm::perspectiveFov(glm::radians(45.0f), 1280.0f, 720.0f, 0.1f, 10000.0f))
	{
	}

	virtual ~EditorLayer()
	{
	}

	virtual void OnAttach() override
	{
		using namespace glm;

		mSimplePBRShader = Crystal::Shader::Create("Assets/Shaders/simplepbr.glsl");
		mQuadShader = Crystal::Shader::Create("Assets/Shaders/quad.glsl");
		mHDRShader = Crystal::Shader::Create("Assets/Shaders/hdr.glsl");
		mGridShader = Crystal::Shader::Create("Assets/Shaders/Grid.glsl");
		mMesh = std::make_shared<Crystal::Mesh>("Assets/Models/m1911/m1911.fbx");

		mSphereMesh = std::make_shared<Crystal::Mesh>("Assets/Models/Sphere1m.fbx");
		mPlaneMesh = std::make_shared<Crystal::Mesh>("Assets/Models/Plane1m.obj");

		// Editor
		mCheckerboardTex = Crystal::Texture2D::Create("Assets/Editor/Checkerboard.tga");

		// Environment
		mEnvironmentCubeMap = Crystal::TextureCube::Create("Assets/Textures/environments/Arches_E_PineTree_Radiance.tga");
		mEnvironmentIrradiance = Crystal::TextureCube::Create("Assets/Textures/environments/Arches_E_PineTree_Irradiance.tga");
		mBRDFLUT = Crystal::Texture2D::Create("Assets/Textures/BRDF_LUT.tga");

		mFramebuffer = Crystal::Framebuffer::Create(1280, 720, Crystal::FramebufferFormat::RGBA16F);
		mFinalPresentBuffer = Crystal::Framebuffer::Create(1280, 720, Crystal::FramebufferFormat::RGBA8);

		mPBRMaterial = std::make_shared<Crystal::Material>(mSimplePBRShader);

		float x = -4.0f;
		float Roughness = 0.0f;
		for (int i = 0; i < 8; i++)
		{
			Crystal::Ref<Crystal::MaterialInstance> mi(new Crystal::MaterialInstance(mPBRMaterial));
			mi->Set("u_Metalness", 1.0f);
			mi->Set("u_Roughness", Roughness);
			mi->Set("u_ModelMatrix", translate(mat4(1.0f), vec3(x, 0.0f, 0.0f)));
			x += 1.1f;
			Roughness += 0.15f;
			mMetalSphereMaterialInstances.push_back(mi);
		}

		x = -4.0f;
		Roughness = 0.0f;
		for (int i = 0; i < 8; i++)
		{
			Crystal::Ref<Crystal::MaterialInstance> mi(new Crystal::MaterialInstance(mPBRMaterial));
			mi->Set("u_Metalness", 0.0f);
			mi->Set("u_Roughness", Roughness);
			mi->Set("u_ModelMatrix", translate(mat4(1.0f), vec3(x, 1.2f, 0.0f)));
			x += 1.1f;
			Roughness += 0.15f;
			mDielectricSphereMaterialInstances.push_back(mi);
		}

		// Create Quad
		x = -1;
		float y = -1;
		float width = 2, height = 2;
		struct QuadVertex
		{
			glm::vec3 Position;
			glm::vec2 TexCoord;
		};

		QuadVertex* Data = new QuadVertex[4];

		Data[0].Position = glm::vec3(x, y, 0);
		Data[0].TexCoord = glm::vec2(0, 0);

		Data[1].Position = glm::vec3(x + width, y, 0);
		Data[1].TexCoord = glm::vec2(1, 0);

		Data[2].Position = glm::vec3(x + width, y + height, 0);
		Data[2].TexCoord = glm::vec2(1, 1);

		Data[3].Position = glm::vec3(x, y + height, 0);
		Data[3].TexCoord = glm::vec2(0, 1);

		mVertexBuffer = Crystal::VertexBuffer::Create();
		mVertexBuffer->SetData(Data, 4 * sizeof(QuadVertex));

		uint32_t* indices = new uint32_t[6]{ 0, 1, 2, 2, 3, 0, };
		mIndexBuffer = Crystal::IndexBuffer::Create();
		mIndexBuffer->SetData(indices, 6 * sizeof(uint32_t));

		mLight.Direction = { -0.5f, -0.5f, 1.0f };
		mLight.Radiance = { 1.0f, 1.0f, 1.0f };
	}

	virtual void OnDetach() override
	{
	}

	virtual void OnUpdate(float DeltaTime) override
	{
		// THINGS TO LOOK AT:
		// - BRDF LUT
		// - Cubemap mips and filtering
		// - Tonemapping and proper HDR pipeline
		using namespace Crystal;
		using namespace glm;

		mCamera.Update(DeltaTime);
		auto ViewProjection = mCamera.GetProjectionMatrix() * mCamera.GetViewMatrix();

		mFramebuffer->Bind();
		Renderer::Clear();

		mQuadShader->Bind();
		mQuadShader->SetMat4("u_InverseVP", inverse(ViewProjection));
		mEnvironmentIrradiance->Bind(0);
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();
		Renderer::DrawIndexed(mIndexBuffer->GetCount(), false);

		mPBRMaterial->Set("u_AlbedoColor", mAlbedoInput.Color);
		mPBRMaterial->Set("u_Metalness", mMetalnessInput.Value);
		mPBRMaterial->Set("u_Roughness", mRoughnessInput.Value);
		mPBRMaterial->Set("u_ViewProjectionMatrix", ViewProjection);
		mPBRMaterial->Set("u_ModelMatrix", scale(mat4(1.0f), vec3(mMeshScale)));
		mPBRMaterial->Set("lights", mLight);
		mPBRMaterial->Set("u_CameraPosition", mCamera.GetPosition());
		mPBRMaterial->Set("u_RadiancePrefilter", mRadiancePrefilter ? 1.0f : 0.0f);
		mPBRMaterial->Set("u_AlbedoTexToggle", mAlbedoInput.UseTexture ? 1.0f : 0.0f);
		mPBRMaterial->Set("u_NormalTexToggle", mNormalInput.UseTexture ? 1.0f : 0.0f);
		mPBRMaterial->Set("u_MetalnessTexToggle", mMetalnessInput.UseTexture ? 1.0f : 0.0f);
		mPBRMaterial->Set("u_RoughnessTexToggle", mRoughnessInput.UseTexture ? 1.0f : 0.0f);
		mPBRMaterial->Set("u_EnvMapRotation", mEnvMapRotation);
		mPBRMaterial->Set("u_EnvRadianceTex", mEnvironmentCubeMap);
		mPBRMaterial->Set("u_EnvIrradianceTex", mEnvironmentIrradiance);
		//mPBRMaterial->Set("u_BRDFLUTTexture", mBRDFLUT);

		if (mAlbedoInput.TextureMap)
			mPBRMaterial->Set("u_AlbedoTexture", mAlbedoInput.TextureMap);
		if (mNormalInput.TextureMap)
			mPBRMaterial->Set("u_NormalTexture", mNormalInput.TextureMap);
		if (mMetalnessInput.TextureMap)
			mPBRMaterial->Set("u_MetalnessTexture", mMetalnessInput.TextureMap);
		if (mRoughnessInput.TextureMap)
			mPBRMaterial->Set("u_RoughnessTexture", mRoughnessInput.TextureMap);

		mGridShader->Bind();
		mGridShader->SetMat4("u_MVP", ViewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
		mGridShader->SetFloat("u_Scale", mGridScale);
		mGridShader->SetFloat("u_Res", m_GridSize);

		//We always want to render grid plane first not to override rendering other actors
		mPlaneMesh->Render(DeltaTime, mGridShader);

		if (mScene == Scene::Spheres)
		{
			for (int i = 0; i < 8; i++)
			{
				mMetalSphereMaterialInstances[i]->Bind();
				mSphereMesh->Render(DeltaTime, mSimplePBRShader);
			}

			for (int i = 0; i < 8; i++)
			{
				mDielectricSphereMaterialInstances[i]->Bind();
				mSphereMesh->Render(DeltaTime, mSimplePBRShader);
			}
		}
		else if (mScene == Scene::Model)
		{
			if (mMesh)
			{
				mPBRMaterial->Bind();

				mSimplePBRShader->SetMat4("u_ViewProjectionMatrix", ViewProjection);
				//mSimplePBRShader->SetMat4("u_ModelMatrix", mat4(1.0f), vec3(mMeshScale)));
				mSimplePBRShader->SetMat4("u_ModelMatrix", scale(translate(mat4(1.0f), vec3(0.0f, 0.1f, 0.0f)), vec3(mMeshScale)));
				mSimplePBRShader->SetFloat("u_EnvMapRotation", mEnvMapRotation);
			
				mMesh->Render(DeltaTime, mSimplePBRShader);
			}
		}

		mGridShader->Bind();
		mGridShader->SetMat4("u_MVP", ViewProjection * glm::scale(glm::mat4(1.0f), glm::vec3(16.0f)));
		mGridShader->SetFloat("u_Scale", mGridScale);
		mGridShader->SetFloat("u_Res", m_GridSize);
		mPlaneMesh->Render(DeltaTime, mGridShader);

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
		static bool pOpen = true;

		static bool OptFullscreenPersistant = true;
		static ImGuiDockNodeFlags OptFlags = ImGuiDockNodeFlags_None;
		bool OptFullscreen = OptFullscreenPersistant;

		// We are using the ImGuiWindowFlags_NoDocking flag to make the parent window not dockable into,
		// because it would be confusing to have two docking targets within each others.
		ImGuiWindowFlags WindowFlags = ImGuiWindowFlags_MenuBar | ImGuiWindowFlags_NoDocking;
		if (OptFullscreen)
		{
			ImGuiViewport* Viewport = ImGui::GetMainViewport();
			ImGui::SetNextWindowPos(Viewport->Pos);
			ImGui::SetNextWindowSize(Viewport->Size);
			ImGui::SetNextWindowViewport(Viewport->ID);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
			ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
			WindowFlags |= ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoMove;
			WindowFlags |= ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoNavFocus;
		}

		// When using ImGuiDockNodeFlags_PassthruDockspace, DockSpace() will render our background and handle the pass-thru hole, so we ask Begin() to not render a background.
		if (OptFlags & ImGuiDockNodeFlags_PassthruCentralNode)
			WindowFlags |= ImGuiWindowFlags_NoBackground;

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0.0f, 0.0f));
		ImGui::Begin("DockSpace", &pOpen, WindowFlags);
		ImGui::PopStyleVar();

		if (OptFullscreen)
			ImGui::PopStyleVar(2);

		// Dockspace
		ImGuiIO& io = ImGui::GetIO();
		if (io.ConfigFlags & ImGuiConfigFlags_DockingEnable)
		{
			ImGuiID DockspaceId = ImGui::GetID("MyDockspace");
			ImGui::DockSpace(DockspaceId, ImVec2(0.0f, 0.0f), OptFlags);
		}

		//Log
		{
			ImGui::SetNextWindowSize(ImVec2(500, 400), ImGuiCond_FirstUseEver);
			ImGui::Begin("Log");
			ImGui::LogButtons();
			ImGui::Text("[18:57:20] Crystal: Initialized Core Log\n"
				"[18:57:20] Application: Initialized Client Log\n");
			ImGui::End();
		}

		//Hierarchy view
		ImGui::Begin("Scene");
		if (ImGui::TreeNode("Hierarchy"))
		{
			ImGui::Text("Root");
			ImGui::TreePop();
		}
		ImGui::End();

		// Editor Panel ------------------------------------------------------------------------------
		ImGui::Begin("Settings");
		if (ImGui::TreeNode("Shaders"))
		{
			auto& Shaders = Crystal::Shader::sAllShaders;
			for (auto& Shader : Shaders)
			{
				if (ImGui::TreeNode(Shader->GetName().c_str()))
				{
					std::string ButtonName = "Reload##" + Shader->GetName();
					if (ImGui::Button(ButtonName.c_str()))
						Shader->Reload();
					ImGui::TreePop();
				}
			}
			ImGui::TreePop();
		}

		ImGui::RadioButton("Spheres", (int*)&mScene, (int)Scene::Spheres);
		ImGui::SameLine();
		ImGui::RadioButton("Model", (int*)&mScene, (int)Scene::Model);

		ImGui::SliderFloat3("Light Dir", glm::value_ptr(mLight.Direction), -1, 1);
		ImGui::ColorEdit3("Light Radiance", glm::value_ptr(mLight.Radiance));
		ImGui::SliderFloat("Light Multiplier", &mLightMultiplier, 0.0f, 5.0f);
		ImGui::SliderFloat("Exposure", &mExposure, 0.0f, 10.0f);
		ImGui::SliderFloat("Mesh Scale", &mMeshScale, 0.0f, 2.0f);

		auto cameraForward = mCamera.GetForwardDirection();
		ImGui::Text("Camera Forward: %.2f, %.2f, %.2f", cameraForward.x, cameraForward.y, cameraForward.z);

		ImGui::Separator();
		{
			ImGui::Text("Mesh");
			std::string Fullpath = mMesh ? mMesh->GetFilePath() : "None";
			size_t Found = Fullpath.find_last_of("/\\");
			std::string Path = Found != std::string::npos ? Fullpath.substr(Found + 1) : Fullpath;
			ImGui::Text(Path.c_str()); ImGui::SameLine();
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
						std::string Filename = Crystal::Application::Get().OpenFile("");
						if (Filename != "")
							mAlbedoInput.TextureMap = Crystal::Texture2D::Create(Filename, mAlbedoInput.SRGB);
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

		if (mMesh)
			mMesh->OnImGuiRender();

		ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(0, 0));
		ImGui::Begin("Viewport");
		auto ViewportSize = ImGui::GetContentRegionAvail();
		mFramebuffer->Resize((uint32_t)ViewportSize.x, (uint32_t)ViewportSize.y);
		mFinalPresentBuffer->Resize((uint32_t)ViewportSize.x, (uint32_t)ViewportSize.y);
		mCamera.SetProjectionMatrix(glm::perspectiveFov(glm::radians(45.0f), ViewportSize.x, ViewportSize.y, 0.1f, 10000.0f));
		ImGui::Image((void*)mFinalPresentBuffer->GetColorAttachmentRendererID(), ViewportSize, { 0, 1 }, { 1, 0 });
		ImGui::End();
		ImGui::PopStyleVar();

		if (ImGui::BeginMenuBar())
		{
			if (ImGui::BeginMenu("Docking"))
			{
				// Disabling fullscreen would allow the window to be moved to the front of other windows, 
				// which we can't undo at the moment without finer window depth/z control.
				//ImGui::MenuItem("Fullscreen", NULL, &OptFullscreenPersistant);

				if (ImGui::MenuItem("Flag: NoSplit", "", (OptFlags & ImGuiDockNodeFlags_NoSplit) != 0))                 OptFlags ^= ImGuiDockNodeFlags_NoSplit;
				if (ImGui::MenuItem("Flag: NoDockingInCentralNode", "", (OptFlags & ImGuiDockNodeFlags_NoDockingInCentralNode) != 0))  OptFlags ^= ImGuiDockNodeFlags_NoDockingInCentralNode;
				if (ImGui::MenuItem("Flag: NoResize", "", (OptFlags & ImGuiDockNodeFlags_NoResize) != 0))                OptFlags ^= ImGuiDockNodeFlags_NoResize;
				if (ImGui::MenuItem("Flag: PassthruDockspace", "", (OptFlags & ImGuiDockNodeFlags_PassthruCentralNode) != 0))       OptFlags ^= ImGuiDockNodeFlags_PassthruCentralNode;
				if (ImGui::MenuItem("Flag: AutoHideTabBar", "", (OptFlags & ImGuiDockNodeFlags_AutoHideTabBar) != 0))          OptFlags ^= ImGuiDockNodeFlags_AutoHideTabBar;
				ImGui::Separator();
				if (ImGui::MenuItem("Close DockSpace", NULL, false, pOpen != NULL))
					pOpen = false;
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
	std::shared_ptr <Crystal::Shader> mGridShader;
	std::shared_ptr<Crystal::Mesh> mMesh;
	std::shared_ptr<Crystal::Mesh> mSphereMesh, mPlaneMesh;
	std::shared_ptr<Crystal::Texture2D> mBRDFLUT;
	std::shared_ptr<Crystal::Material> mPBRMaterial;
	std::vector<Crystal::Ref<Crystal::MaterialInstance>> mMetalSphereMaterialInstances;
	std::vector<Crystal::Ref<Crystal::MaterialInstance>> mDielectricSphereMaterialInstances;

	float mGridScale = 16.025f, m_GridSize = 0.025f;
	float mMeshScale = 1.0f;

	struct AlbedoInput
	{
		glm::vec3 Color = { 0.972f, 0.96f, 0.915f };
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
#pragma once

#include "Crystal/Core/Core.h"

#include "Crystal/Renderer/Shader.h"
#include "Crystal/Renderer/Textures/Texture.h"
#include "Crystal/Renderer/Textures/Texture2D.h"
#include "Crystal/Renderer/Textures/TextureCube.h"

#include <unordered_set>

#include "Material.h"

namespace Crystal {

	class Material
	{
		friend class MaterialInstance;
	public:
		Material(const Ref<Shader>& InShader);
		virtual ~Material();

		void Bind() const;

		template <typename T>
		void Set(const std::string& Name, const T& Value)
		{
			auto Decl = FindUniformDeclaration(Name);
			CL_CORE_ASSERT(Decl, "Could not find uniform with name 'x'");
			auto& Buffer = GetUniformBufferTarget(Decl);
			Buffer.Write((byte*)&Value, Decl->GetSize(), Decl->GetOffset());

			for (auto MI : mMaterialInstances)
				MI->OnMaterialValueUpdated(Decl);
		}

		void Set(const std::string& Name, const Ref<Texture>& InTexture)
		{
			auto Decl = FindResourceDeclaration(Name);
			uint32_t Slot = Decl->GetRegister();

			if (mTextures.size() <= Slot)
				mTextures.resize((size_t)Slot + 1);

			mTextures[Slot] = InTexture;
		}

		void Set(const std::string& Name, const Ref<Texture2D>& InTexture)
		{
			Set(Name, (const Ref<Texture>&)InTexture);
		}

		void Set(const std::string& Name, const Ref<TextureCube>& InTexture)
		{
			Set(Name, (const Ref<Texture>&)InTexture);
		}
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		void BindTextures() const;

		ShaderUniformDeclaration* FindUniformDeclaration(const std::string& Name);
		ShaderResourceDeclaration* FindResourceDeclaration(const std::string& Name);
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* UniformDeclaration);

		Ref<Shader> mShader;
		std::unordered_set<MaterialInstance*> mMaterialInstances;

		Buffer mVSUniformStorageBuffer;
		Buffer mPSUniformStorageBuffer;
		std::vector<Ref<Texture>> mTextures;

		int32_t mRenderFlags = 0;
	};

	class MaterialInstance
	{
		friend class Material;
	public:
		MaterialInstance(const Ref<Material>& InMaterial);
		virtual ~MaterialInstance();

		template <typename T>
		void Set(const std::string& Name, const T& Value)
		{
			auto Decl = mMaterial->FindUniformDeclaration(Name);
			CL_CORE_ASSERT(Decl, "Could not find uniform with name 'x'");
			auto& Buffer = GetUniformBufferTarget(Decl);
			Buffer.Write((byte*)& Value, Decl->GetSize(), Decl->GetOffset());

			mOverriddenValues.insert(Name);
		}

		void Set(const std::string& Name, const Ref<Texture>& InTexture)
		{
			auto Decl = mMaterial->FindResourceDeclaration(Name);
			uint32_t Slot = Decl->GetRegister();
			if (mTextures.size() <= Slot)
				mTextures.resize((size_t)Slot + 1);
			mTextures[Slot] = InTexture;
		}

		void Set(const std::string& Name, const Ref<Texture2D>& InTexture)
		{
			Set(Name, (const Ref<Texture>&)InTexture);
		}

		void Set(const std::string& Name, const Ref<TextureCube>& InTexture)
		{
			Set(Name, (const Ref<Texture>&)InTexture);
		}

		void Bind() const;
	private:
		void AllocateStorage();
		void OnShaderReloaded();
		Buffer& GetUniformBufferTarget(ShaderUniformDeclaration* UniformDeclaration);
		void OnMaterialValueUpdated(ShaderUniformDeclaration* Decl);
	private:
		Ref<Material> mMaterial;

		Buffer mVSUniformStorageBuffer;
		Buffer mPSUniformStorageBuffer;
		std::vector<Ref<Texture>> mTextures;

		// TODO: This is temporary; come up with a proper system to track overrides
		std::unordered_set<std::string> mOverriddenValues;
	};

}
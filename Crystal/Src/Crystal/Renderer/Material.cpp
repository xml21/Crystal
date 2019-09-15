#include "Crpch.h"
#include "Material.h"

namespace Crystal {

	Material::Material(const Ref<Shader>& InShader)
		: mShader(InShader)
	{
		mShader->AddShaderReloadedCallback(std::bind(&Material::OnShaderReloaded, this));
		AllocateStorage();
	}

	Material::~Material()
	{
	}

	void Material::AllocateStorage()
	{
		const auto& VSBuffer = mShader->GetVSMaterialUniformBuffer();
		CL_CORE_ASSERT(VSBuffer, "VSBuffer is null!");
		mVSUniformStorageBuffer.Allocate(VSBuffer.GetSize());
		mVSUniformStorageBuffer.ZeroInitialize();

		const auto& PSBuffer = mShader->GetPSMaterialUniformBuffer();
		CL_CORE_ASSERT(PSBuffer, "PSBuffer is null!");
		mPSUniformStorageBuffer.Allocate(PSBuffer.GetSize());
		mPSUniformStorageBuffer.ZeroInitialize();
	}

	void Material::OnShaderReloaded()
	{
		AllocateStorage();

		for (auto MI : mMaterialInstances)
			MI->OnShaderReloaded();
	}

	ShaderUniformDeclaration* Material::FindUniformDeclaration(const std::string& Name)
	{
		if (mVSUniformStorageBuffer)
		{
			auto& Declarations = mShader->GetVSMaterialUniformBuffer().GetUniformDeclarations();
			for (ShaderUniformDeclaration* Uniform : Declarations)
			{
				if (Uniform->GetName() == Name)
					return Uniform;
			}
		}

		if (mPSUniformStorageBuffer)
		{
			auto& Declarations = mShader->GetPSMaterialUniformBuffer().GetUniformDeclarations();
			for (ShaderUniformDeclaration* Uniform : Declarations)
			{
				if (Uniform->GetName() == Name)
					return Uniform;
			}
		}
		return nullptr;
	}

	ShaderResourceDeclaration* Material::FindResourceDeclaration(const std::string& Name)
	{
		auto& Resources = mShader->GetResources();
		for (ShaderResourceDeclaration* Resource : Resources)
		{
			if (Resource->GetName() == Name)
				return Resource;
		}
		return nullptr;
	}

	Buffer& Material::GetUniformBufferTarget(ShaderUniformDeclaration* UniformDeclaration)
	{
		switch (UniformDeclaration->GetDomain())
		{
		case ShaderDomain::Vertex:    return mVSUniformStorageBuffer;
		case ShaderDomain::Pixel:     return mPSUniformStorageBuffer;
		}

		CL_CORE_ASSERT(false, "Invalid uniform Declaration domain! Material does not support this shader type.");
		return mVSUniformStorageBuffer;
	}

	void Material::Bind() const
	{
		mShader->Bind();

		//if (mVSUniformStorageBuffer)
		//	mShader->SetVSMaterialUniformBuffer(mVSUniformStorageBuffer);

		if (mPSUniformStorageBuffer)
			mShader->SetPSMaterialUniformBuffer(mPSUniformStorageBuffer);

		BindTextures();
	}

	void Material::BindTextures() const
	{
		for (size_t i = 0; i < mTextures.size(); i++)
		{
			auto& Texture = mTextures[i];
			if (Texture)
				Texture->Bind(i);
		}
	}

	MaterialInstance::MaterialInstance(const Ref<Material>& InMaterial)
		: mMaterial(InMaterial)
	{
		mMaterial->mMaterialInstances.insert(this);
		AllocateStorage();
	}

	MaterialInstance::~MaterialInstance()
	{
		mMaterial->mMaterialInstances.erase(this);
	}

	void MaterialInstance::OnShaderReloaded()
	{
		AllocateStorage();
		mOverriddenValues.clear();
	}

	void MaterialInstance::AllocateStorage()
	{
		const auto& VSBuffer = mMaterial->mShader->GetVSMaterialUniformBuffer();
		mVSUniformStorageBuffer.Allocate(VSBuffer.GetSize());
		memcpy(mVSUniformStorageBuffer.Data, mMaterial->mVSUniformStorageBuffer.Data, VSBuffer.GetSize());

		const auto& PSBuffer = mMaterial->mShader->GetPSMaterialUniformBuffer();
		mPSUniformStorageBuffer.Allocate(PSBuffer.GetSize());
		memcpy(mPSUniformStorageBuffer.Data, mMaterial->mPSUniformStorageBuffer.Data, PSBuffer.GetSize());
	}

	void MaterialInstance::OnMaterialValueUpdated(ShaderUniformDeclaration* Decl)
	{
		if (mOverriddenValues.find(Decl->GetName()) == mOverriddenValues.end())
		{
			auto& Buffer = GetUniformBufferTarget(Decl);
			auto& MaterialBuffer = mMaterial->GetUniformBufferTarget(Decl);
			Buffer.Write(MaterialBuffer.Data + Decl->GetOffset(), Decl->GetSize(), Decl->GetOffset());
		}
	}

	Buffer& MaterialInstance::GetUniformBufferTarget(ShaderUniformDeclaration* UniformDeclaration)
	{
		switch (UniformDeclaration->GetDomain())
		{
		case ShaderDomain::Vertex:    return mVSUniformStorageBuffer;
		case ShaderDomain::Pixel:     return mPSUniformStorageBuffer;
		}

		CL_CORE_ASSERT(false, "Invalid uniform Declaration domain! Material does not support this shader type.");
		return mVSUniformStorageBuffer;
	}

	void MaterialInstance::Bind() const
	{
		if (mVSUniformStorageBuffer)
			mMaterial->mShader->SetVSMaterialUniformBuffer(mVSUniformStorageBuffer);

		if (mPSUniformStorageBuffer)
			mMaterial->mShader->SetPSMaterialUniformBuffer(mPSUniformStorageBuffer);

		mMaterial->BindTextures();
		for (size_t i = 0; i < mTextures.size(); i++)
		{
			auto& Texture = mTextures[i];
			if (Texture)
				Texture->Bind(i);
		}
	}
}
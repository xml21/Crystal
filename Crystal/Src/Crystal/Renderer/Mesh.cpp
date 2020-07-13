#include "Crpch.h"

#include "Mesh.h"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/quaternion.hpp>

#include <assimp/Scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

#include "Buffers/IndexBuffer.h"
#include "Renderer.h"

#include "imgui/imgui.h"

namespace Crystal
{
	static const uint32_t sMeshImportFlags =
		aiProcess_CalcTangentSpace |        // Create binormals/tangents just in case
		aiProcess_Triangulate |             // Make sure we're triangles
		aiProcess_SortByPType |             // Split meshes by primitive type
		aiProcess_GenNormals |              // Make sure we have legit normals
		aiProcess_GenUVCoords |             // Convert UVs if required 
		aiProcess_OptimizeMeshes |          // Batch draws where possible
		aiProcess_ValidateDataStructure;    // Validation

	struct LogStream : public Assimp::LogStream
	{
		static void Initialize()
		{
			if (Assimp::DefaultLogger::isNullLogger())
			{
				Assimp::DefaultLogger::create("", Assimp::Logger::VERBOSE);
				Assimp::DefaultLogger::get()->attachStream(new LogStream, Assimp::Logger::Err | Assimp::Logger::Warn);
			}
		}

		void write(const char* Message) override
		{
			CL_CORE_LOG_ERROR("Assimp error: {0}", Message);
		}
	};

	static glm::mat4 aiMatrix4x4ToGlm(const aiMatrix4x4 &From)
	{
		glm::mat4 To;
		//the a,b,c,d in assimp is the row ; the 1,2,3,4 is the column
		To[0][0] = From.a1; To[1][0] = From.a2; To[2][0] = From.a3; To[3][0] = From.a4;
		To[0][1] = From.b1; To[1][1] = From.b2; To[2][1] = From.b3; To[3][1] = From.b4;
		To[0][2] = From.c1; To[1][2] = From.c2; To[2][2] = From.c3; To[3][2] = From.c4;
		To[0][3] = From.d1; To[1][3] = From.d2; To[2][3] = From.d3; To[3][3] = From.d4;
		return To;
	}

	Mesh::Mesh(const std::string& Filename)
		: mFilePath(Filename)
	{
		LogStream::Initialize();

		CL_CORE_LOG_INFO("Loading mesh: {0}", Filename.c_str());

		mImporter = std::make_unique<Assimp::Importer>();

		const aiScene* Scene = mImporter->ReadFile(Filename, sMeshImportFlags);
		if (!Scene || !Scene->HasMeshes())
			CL_CORE_LOG_ERROR("Failed to load mesh file: {0}", mFilePath);

		mInverseTransform = glm::inverse(aiMatrix4x4ToGlm(Scene->mRootNode->mTransformation));

		uint32_t VertexCount = 0;
		uint32_t IndexCount = 0;

		mSubmeshes.reserve(Scene->mNumMeshes);
		for (size_t m = 0; m < Scene->mNumMeshes; m++)
		{
			aiMesh* Mesh = Scene->mMeshes[m];

			Submesh Submesh;
			Submesh.BaseVertex = VertexCount;
			Submesh.BaseIndex = IndexCount;
			Submesh.MaterialIndex = Mesh->mMaterialIndex;
			Submesh.IndexCount = Mesh->mNumFaces * 3;
			mSubmeshes.push_back(Submesh);

			VertexCount += Mesh->mNumVertices;
			IndexCount += Submesh.IndexCount;

			CL_CORE_ASSERT(Mesh->HasPositions(), "Meshes require positions.");
			CL_CORE_ASSERT(Mesh->HasNormals(), "Meshes require normals.");

			// Vertices
			for (size_t i = 0; i < Mesh->mNumVertices; i++)
			{
				Vertex Vertex;
				Vertex.Position = { Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z };
				Vertex.Normal = { Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z };

				if (Mesh->HasTangentsAndBitangents())
				{
					Vertex.Tangent = { Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z };
					Vertex.Binormal = { Mesh->mBitangents[i].x, Mesh->mBitangents[i].y, Mesh->mBitangents[i].z };
				}

				if (Mesh->HasTextureCoords(0))
					Vertex.Texcoord = { Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y };
				mVertices.push_back(Vertex);
			}

			// Indices
			for (size_t i = 0; i < Mesh->mNumFaces; i++)
			{
				CL_CORE_ASSERT(Mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
				mIndices.push_back({ Mesh->mFaces[i].mIndices[0], Mesh->mFaces[i].mIndices[1], Mesh->mFaces[i].mIndices[2] });
			}
		}

		// Bones
		for (size_t m = 0; m < Scene->mNumMeshes; m++)
		{
			aiMesh* Mesh = Scene->mMeshes[m];
			Submesh& Submesh = mSubmeshes[m];

			for (size_t i = 0; i < Mesh->mNumBones; i++)
			{
				aiBone* Bone = Mesh->mBones[i];
				std::string BoneName(Bone->mName.data);
				int BoneIndex = 0;

				if (mBoneMapping.find(BoneName) == mBoneMapping.end())
				{
					// Allocate an index for a new bone
					BoneIndex = mBoneCount;
					mBoneCount++;
					BoneInfo BI;
					mBoneInfo.push_back(BI);
					mBoneInfo[BoneIndex].BoneOffset = aiMatrix4x4ToGlm(Bone->mOffsetMatrix);
					mBoneMapping[BoneName] = BoneIndex;
				}
				else
				{
					CL_CORE_LOG_TRACE("Found existing bone in map");
					BoneIndex = mBoneMapping[BoneName];
				}

				for (size_t j = 0; j < Bone->mNumWeights; j++)
				{
					int VertexID = Submesh.BaseVertex + Bone->mWeights[j].mVertexId;
					float Weight = Bone->mWeights[j].mWeight;
					mVertices[VertexID].AddBoneData(BoneIndex, Weight);
				}
			}
		}

		mVertexBuffer = VertexBuffer::Create();
		mVertexBuffer->SetData(mVertices.data(), (unsigned int)mVertices.size() * sizeof(Vertex));

		mIndexBuffer = IndexBuffer::Create();
		mIndexBuffer->SetData(mIndices.data(), (unsigned int)mIndices.size() * sizeof(Index));

		mScene = Scene;
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::Render(float DeltaTime, std::shared_ptr<Shader> Shader)
	{
		if (mAnimationPlaying && mScene->mAnimations)
		{
			mWorldTime += DeltaTime;

			float ticksPerSecond = (float)(mScene->mAnimations[0]->mTicksPerSecond != 0 ? mScene->mAnimations[0]->mTicksPerSecond : 25.0f) * mTimeMultiplier;
			mAnimationTime += DeltaTime * ticksPerSecond;
			mAnimationTime = fmod(mAnimationTime, (float)mScene->mAnimations[0]->mDuration);
		}

		if (mScene->mAnimations)
			BoneTransform(mAnimationTime);

		// TODO: Sort this out
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();

		// TODO: replace with render API calls
		CL_RENDER_S1(Shader, {
			for (Submesh& submesh : self->mSubmeshes)
			{
				glEnableVertexAttribArray(0);
				glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Position));

				glEnableVertexAttribArray(1);
				glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Normal));

				glEnableVertexAttribArray(2);
				glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Tangent));

				glEnableVertexAttribArray(3);
				glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Binormal));

				glEnableVertexAttribArray(4);
				glVertexAttribPointer(4, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Texcoord));

				glEnableVertexAttribArray(5);
				glVertexAttribIPointer(5, 4, GL_INT, sizeof(Vertex), (const void*)offsetof(Vertex, IDs));

				glEnableVertexAttribArray(6);
				glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(Vertex), (const void*)offsetof(Vertex, Weights));

				if (self->mScene->mAnimations)
				{
					for (size_t i = 0; i < self->mBoneTransforms.size(); i++)
					{
						std::string UniformName = std::string("u_BoneTransforms[") + std::to_string(i) + std::string("]");
						Shader->SetMat4FromRenderThread(UniformName, self->mBoneTransforms[i]);
					}
				}
				glDrawElementsBaseVertex(GL_TRIANGLES, submesh.IndexCount, GL_UNSIGNED_INT, (void*)(sizeof(uint32_t) * submesh.BaseIndex), submesh.BaseVertex);
			}
			});
	}

	void Mesh::OnImGuiRender()
	{
		ImGui::Begin("Mesh Debug");
		if (ImGui::CollapsingHeader(mFilePath.c_str(), nullptr, ImGuiTreeNodeFlags_DefaultOpen))
		{
			if (mScene->HasAnimations())
			{
				if (ImGui::CollapsingHeader("Animation", nullptr, ImGuiTreeNodeFlags_DefaultOpen))
				{
					if (ImGui::Button(mAnimationPlaying ? "Pause" : "Play"))
					{
						mAnimationPlaying = !mAnimationPlaying;
					}

					ImGui::SliderFloat("##AnimationTime", &mAnimationTime, 0.0f, (float)mScene->mAnimations[0]->mDuration);
					ImGui::DragFloat("Time Scale", &mTimeMultiplier, 0.05f, 0.0f, 10.0f);
				}
			}
		}

		ImGui::End();
	}

	void Mesh::DumpVertexBuffer()
	{
		// TODO: Convert to ImGui
		CL_CORE_LOG_TRACE("------------------------------------------------------");
		CL_CORE_LOG_TRACE("Vertex Buffer Dump");
		CL_CORE_LOG_TRACE("Mesh: {0}", mFilePath);
		for (size_t i = 0; i < mVertices.size(); i++)
		{
			auto& Vertex = mVertices[i];
			CL_CORE_LOG_TRACE("Vertex: {0}", i);
			CL_CORE_LOG_TRACE("Position: {0}, {1}, {2}", Vertex.Position.x, Vertex.Position.y, Vertex.Position.z);
			CL_CORE_LOG_TRACE("Normal: {0}, {1}, {2}", Vertex.Normal.x, Vertex.Normal.y, Vertex.Normal.z);
			CL_CORE_LOG_TRACE("Binormal: {0}, {1}, {2}", Vertex.Binormal.x, Vertex.Binormal.y, Vertex.Binormal.z);
			CL_CORE_LOG_TRACE("Tangent: {0}, {1}, {2}", Vertex.Tangent.x, Vertex.Tangent.y, Vertex.Tangent.z);
			CL_CORE_LOG_TRACE("TexCoord: {0}, {1}", Vertex.Texcoord.x, Vertex.Texcoord.y);
			CL_CORE_LOG_TRACE("--");
		}
		CL_CORE_LOG_TRACE("------------------------------------------------------");
	}

	void Mesh::BoneTransform(float Time)
	{
		ReadNodeHierarchy(Time, mScene->mRootNode, glm::mat4(1.0f));
		mBoneTransforms.resize(mBoneCount);
		for (size_t i = 0; i < mBoneCount; i++)
			mBoneTransforms[i] = mBoneInfo[i].FinalTransformation;
	}

	void Mesh::ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform)
	{
		std::string name(pNode->mName.data);
		const aiAnimation* animation = mScene->mAnimations[0];
		glm::mat4 nodeTransform(aiMatrix4x4ToGlm(pNode->mTransformation));
		const aiNodeAnim* NodeAnim = FindNodeAnim(animation, name);

		if (NodeAnim)
		{
			glm::vec3 translation = InterpolateTranslation(AnimationTime, NodeAnim);
			glm::mat4 translationMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(translation.x, translation.y, translation.z));

			glm::quat rotation = InterpolateRotation(AnimationTime, NodeAnim);
			glm::mat4 rotationMatrix = glm::toMat4(rotation);

			glm::vec3 scale = InterpolateScale(AnimationTime, NodeAnim);
			glm::mat4 scaleMatrix = glm::scale(glm::mat4(1.0f), glm::vec3(scale.x, scale.y, scale.z));

			nodeTransform = translationMatrix * rotationMatrix * scaleMatrix;
		}

		glm::mat4 transform = ParentTransform * nodeTransform;

		if (mBoneMapping.find(name) != mBoneMapping.end())
		{
			uint32_t BoneIndex = mBoneMapping[name];
			mBoneInfo[BoneIndex].FinalTransformation = mInverseTransform * transform * mBoneInfo[BoneIndex].BoneOffset;
		}

		for (uint32_t i = 0; i < pNode->mNumChildren; i++)
			ReadNodeHierarchy(AnimationTime, pNode->mChildren[i], transform);
	}

	const aiNodeAnim* Mesh::FindNodeAnim(const aiAnimation* Animation, const std::string& NodeName)
	{
		for (uint32_t i = 0; i < Animation->mNumChannels; i++)
		{
			const aiNodeAnim* NodeAnim = Animation->mChannels[i];
			if (std::string(NodeAnim->mNodeName.data) == NodeName)
				return NodeAnim;
		}
		return nullptr;
	}

	uint32_t Mesh::FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		for (uint32_t i = 0; i < pNodeAnim->mNumPositionKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mPositionKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	uint32_t Mesh::FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		CL_CORE_ASSERT(pNodeAnim->mNumRotationKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumRotationKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mRotationKeys[i + 1].mTime)
				return i;
		}

		return 0;

	}

	uint32_t Mesh::FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim)
	{
		CL_CORE_ASSERT(pNodeAnim->mNumScalingKeys > 0);

		for (uint32_t i = 0; i < pNodeAnim->mNumScalingKeys - 1; i++)
		{
			if (AnimationTime < (float)pNodeAnim->mScalingKeys[i + 1].mTime)
				return i;
		}

		return 0;
	}

	glm::vec3 Mesh::InterpolateTranslation(float AnimationTime, const aiNodeAnim* NodeAnim)
	{
		if (NodeAnim->mNumPositionKeys == 1)
		{
			// No interpolation necessary for single value
			auto V = NodeAnim->mPositionKeys[0].mValue;
			return { V.x, V.y, V.z };
		}

		uint32_t PositionIndex = FindPosition(AnimationTime, NodeAnim);
		uint32_t NextPositionIndex = (PositionIndex + 1);

		CL_CORE_ASSERT(NextPositionIndex < NodeAnim->mNumPositionKeys);

		float DeltaTime = (float)(NodeAnim->mPositionKeys[NextPositionIndex].mTime - NodeAnim->mPositionKeys[PositionIndex].mTime);
		float Factor = (AnimationTime - (float)NodeAnim->mPositionKeys[PositionIndex].mTime) / DeltaTime;

		if (Factor < 0.0f)
			Factor = 0.0f;

		CL_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");

		const aiVector3D& Start = NodeAnim->mPositionKeys[PositionIndex].mValue;
		const aiVector3D& End = NodeAnim->mPositionKeys[NextPositionIndex].mValue;

		aiVector3D Delta = End - Start;
		auto AiVec = Start + Factor * Delta;

		return { AiVec.x, AiVec.y, AiVec.z };
	}

	glm::quat Mesh::InterpolateRotation(float AnimationTime, const aiNodeAnim* NodeAnim)
	{
		if (NodeAnim->mNumRotationKeys == 1)
		{
			// No interpolation necessary for single value
			auto V = NodeAnim->mRotationKeys[0].mValue;
			return glm::quat(V.w, V.x, V.y, V.z);
		}

		uint32_t RotationIndex = FindRotation(AnimationTime, NodeAnim);
		uint32_t NextRotationIndex = (RotationIndex + 1);

		CL_CORE_ASSERT(NextRotationIndex < NodeAnim->mNumRotationKeys);

		float DeltaTime = (float)(NodeAnim->mRotationKeys[NextRotationIndex].mTime - NodeAnim->mRotationKeys[RotationIndex].mTime);
		float Factor = (AnimationTime - (float)NodeAnim->mRotationKeys[RotationIndex].mTime) / DeltaTime;

		if (Factor < 0.0f)
			Factor = 0.0f;

		CL_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");

		const aiQuaternion& StartRotationQ = NodeAnim->mRotationKeys[RotationIndex].mValue;
		const aiQuaternion& EndRotationQ = NodeAnim->mRotationKeys[NextRotationIndex].mValue;

		auto Q = aiQuaternion();
		aiQuaternion::Interpolate(Q, StartRotationQ, EndRotationQ, Factor);
		Q = Q.Normalize();

		return glm::quat(Q.w, Q.x, Q.y, Q.z);
	}

	glm::vec3 Mesh::InterpolateScale(float AnimationTime, const aiNodeAnim* NodeAnim)
	{
		if (NodeAnim->mNumScalingKeys == 1)
		{
			// No interpolation necessary for single value
			auto v = NodeAnim->mScalingKeys[0].mValue;
			return { v.x, v.y, v.z };
		}

		uint32_t Index = FindScaling(AnimationTime, NodeAnim);
		uint32_t NextIndex = (Index + 1);
		CL_CORE_ASSERT(NextIndex < NodeAnim->mNumScalingKeys);
		float DeltaTime = (float)(NodeAnim->mScalingKeys[NextIndex].mTime - NodeAnim->mScalingKeys[Index].mTime);
		float Factor = (AnimationTime - (float)NodeAnim->mScalingKeys[Index].mTime) / DeltaTime;

		if (Factor < 0.0f)
			Factor = 0.0f;

		CL_CORE_ASSERT(Factor <= 1.0f, "Factor must be below 1.0f");

		const auto& Start = NodeAnim->mScalingKeys[Index].mValue;
		const auto& End = NodeAnim->mScalingKeys[NextIndex].mValue;
		auto Delta = End - Start;
		auto aiVec = Start + Factor * Delta;

		return { aiVec.x, aiVec.y, aiVec.z };
	}
}
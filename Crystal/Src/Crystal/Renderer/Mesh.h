#pragma once
#include <vector>
#include <glm/glm.hpp>

#include "Crystal/Time/Time.h"

#include "Crystal/Renderer/Buffers/IndexBuffer.h"
#include "Crystal/Renderer/Buffers/VertexBuffer.h"

#include "Crystal/Renderer/Shader.h"

struct aiNode;
struct aiAnimation;
struct aiNodeAnim;
struct aiScene;

namespace Assimp {
	class Importer;
}

namespace Crystal {

	struct Vertex
	{
		glm::vec3 Position;
		glm::vec3 Normal;
		glm::vec3 Tangent;
		glm::vec3 Binormal;
		glm::vec2 Texcoord;

		uint32_t IDs[4] = { 0, 0,0, 0 };
		float Weights[4]{ 0.0f, 0.0f, 0.0f, 0.0f };

		void AddBoneData(uint32_t BoneID, float Weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] == 0.0)
				{
					IDs[i] = BoneID;
					Weights[i] = Weight;
					return;
				}
			}

			// TODO: Keep top weights
			CL_CORE_LOG_WARN("Vertex has more than four bones/weights affecting it, extra data will be discarded (BoneID={0}, Weight={1})", BoneID, Weight);
		}
	};

	static const int NumAttributes = 5;

	struct Index
	{
		uint32_t V1, V2, V3;
	};

	static_assert(sizeof(Index) == 3 * sizeof(uint32_t));

	struct BoneInfo
	{
		glm::mat4 BoneOffset;
		glm::mat4 FinalTransformation;
	};

	struct VertexBoneData
	{
		uint32_t IDs[4];
		float Weights[4];

		VertexBoneData()
		{
			memset(IDs, 0, sizeof(IDs));
			memset(Weights, 0, sizeof(Weights));
		};

		void AddBoneData(uint32_t BoneID, float Weight)
		{
			for (size_t i = 0; i < 4; i++)
			{
				if (Weights[i] == 0.0)
				{
					IDs[i] = BoneID;
					Weights[i] = Weight;
					return;
				}
			}

			// should never get here - more bones than we have space for
			CL_CORE_ASSERT(false, "Too many bones!");
		}
	};

	class Submesh
	{
	public:
		uint32_t BaseVertex;
		uint32_t BaseIndex;
		uint32_t MaterialIndex;
		uint32_t IndexCount;
	};

	class Mesh
	{
	public:
		Mesh(const std::string& Filename);
		~Mesh();

		void Render(float DeltaTime, std::shared_ptr<Shader> Shader);
		void OnImGuiRender();
		void DumpVertexBuffer();

		inline const std::string& GetFilePath() const { return mFilePath; }

	private:
		void BoneTransform(float Time);
		void ReadNodeHierarchy(float AnimationTime, const aiNode* pNode, const glm::mat4& ParentTransform);

		const aiNodeAnim* FindNodeAnim(const aiAnimation* Animation, const std::string& NodeName);
		uint32_t FindPosition(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindRotation(float AnimationTime, const aiNodeAnim* pNodeAnim);
		uint32_t FindScaling(float AnimationTime, const aiNodeAnim* pNodeAnim);
		glm::vec3 InterpolateTranslation(float AnimationTime, const aiNodeAnim* NodeAnim);
		glm::quat InterpolateRotation(float AnimationTime, const aiNodeAnim* NodeAnim);
		glm::vec3 InterpolateScale(float AnimationTime, const aiNodeAnim* NodeAnim);

		std::vector<Submesh> mSubmeshes;

		std::unique_ptr<Assimp::Importer> mImporter;

		glm::mat4 mInverseTransform;

		uint32_t mBoneCount = 0;
		std::vector<BoneInfo> mBoneInfo;

		std::vector<Vertex> mVertices;
		std::vector<Index> mIndices;

		Ref<VertexBuffer> mVertexBuffer;
		Ref<IndexBuffer> mIndexBuffer;

		std::unordered_map<std::string, uint32_t> mBoneMapping;
		std::vector<glm::mat4> mBoneTransforms;
		const aiScene* mScene;

		// Animation
		float mAnimationTime = 0.0f;
		float mWorldTime = 0.0f;
		float mTimeMultiplier = 1.0f;
		bool mAnimationPlaying = true;

		std::string mFilePath;
	};
}
#include "Crpch.h"

#include "Mesh.h"

#include <assimp/scene.h>
#include <assimp/postprocess.h>
#include <assimp/Importer.hpp>
#include <assimp/DefaultLogger.hpp>
#include <assimp/LogStream.hpp>

#include <glad/glad.h>

#include "Buffers/IndexBuffer.h"
#include "Renderer.h"

namespace Crystal
{
	namespace {
		const unsigned int ImportFlags =
			aiProcess_CalcTangentSpace |
			aiProcess_Triangulate |
			aiProcess_SortByPType |
			aiProcess_PreTransformVertices |
			aiProcess_GenNormals |
			aiProcess_GenUVCoords |
			aiProcess_OptimizeMeshes |
			aiProcess_Debone |
			aiProcess_ValidateDataStructure;
	}

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

	Mesh::Mesh(const std::string& Filename)
		: mFilePath(Filename)
	{
		LogStream::Initialize();

		CL_CORE_LOG_INFO("Loading mesh: {0}", mFilePath.c_str());

		Assimp::Importer importer;

		const aiScene* Scene = importer.ReadFile(mFilePath, ImportFlags);
		if (!Scene || !Scene->HasMeshes())
			CL_CORE_LOG_ERROR("Failed to load mesh file: {0}", mFilePath);

		aiMesh* Mesh = Scene->mMeshes[0];

		CL_CORE_ASSERT(Mesh->HasPositions(), "Meshes require positions.");
		CL_CORE_ASSERT(Mesh->HasNormals(), "Meshes require normals.");

		mVertices.reserve(Mesh->mNumVertices);

		// Extract vertices from model
		for (size_t i = 0; i < mVertices.capacity(); i++)
		{
			Vertex vertex;
			vertex.Position = { Mesh->mVertices[i].x, Mesh->mVertices[i].y, Mesh->mVertices[i].z };
			vertex.Normal = { Mesh->mNormals[i].x, Mesh->mNormals[i].y, Mesh->mNormals[i].z };

			if (Mesh->HasTangentsAndBitangents())
			{
				vertex.Tangent = { Mesh->mTangents[i].x, Mesh->mTangents[i].y, Mesh->mTangents[i].z };
				vertex.Binormal = { Mesh->mBitangents[i].x, Mesh->mBitangents[i].y, Mesh->mBitangents[i].z };
			}

			if (Mesh->HasTextureCoords(0))
				vertex.Texcoord = { Mesh->mTextureCoords[0][i].x, Mesh->mTextureCoords[0][i].y };
			mVertices.push_back(vertex);
		}

		mVertexBuffer = VertexBuffer::Create();
		mVertexBuffer->SetData(mVertices.data(), mVertices.size() * sizeof(Vertex));

		// Extract indices from model
		mIndices.reserve(Mesh->mNumFaces);
		for (size_t i = 0; i < mIndices.capacity(); i++)
		{
			CL_CORE_ASSERT(Mesh->mFaces[i].mNumIndices == 3, "Must have 3 indices.");
			mIndices.push_back({ Mesh->mFaces[i].mIndices[0], Mesh->mFaces[i].mIndices[1], Mesh->mFaces[i].mIndices[2] });
		}

		mIndexBuffer = IndexBuffer::Create();
		mIndexBuffer->SetData(mIndices.data(), mIndices.size() * sizeof(Index));
	}

	Mesh::~Mesh()
	{

	}

	void Mesh::Render()
	{
		//TODO: Sorting ------------------------------------------------------------------------------------------
		mVertexBuffer->Bind();
		mIndexBuffer->Bind();

		CL_RENDER_S({
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
		});
			
		Renderer::DrawIndexed(mIndexBuffer->GetCount());
		//TODO: Sorting ------------------------------------------------------------------------------------------
	}

}
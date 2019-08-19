#include "Crpch.h"

#include "OpenGLVertexArray.h"

#include "glad/glad.h"

namespace Crystal
{
	static GLenum ShaderDataTypeToOpenGLBaseType(ShaderDataType type)
	{
		switch (type)
		{
		case Crystal::ShaderDataType::Float:    return GL_FLOAT;
		case Crystal::ShaderDataType::Float2:   return GL_FLOAT;
		case Crystal::ShaderDataType::Float3:   return GL_FLOAT;
		case Crystal::ShaderDataType::Float4:   return GL_FLOAT;
		case Crystal::ShaderDataType::Mat3:     return GL_FLOAT;
		case Crystal::ShaderDataType::Mat4:     return GL_FLOAT;
		case Crystal::ShaderDataType::Int:      return GL_INT;
		case Crystal::ShaderDataType::Int2:     return GL_INT;
		case Crystal::ShaderDataType::Int3:     return GL_INT;
		case Crystal::ShaderDataType::Int4:     return GL_INT;
		case Crystal::ShaderDataType::Bool:     return GL_BOOL;
		}

		CL_CORE_ASSERT(false, "Unknown ShaderDataType!");
		return 0;
	}

	OpenGLVertexArray::OpenGLVertexArray()
	{
		glCreateVertexArrays(1, &mRendererID);
	}

	OpenGLVertexArray::~OpenGLVertexArray()
	{
		glDeleteVertexArrays(1, &mRendererID);
	}

	void OpenGLVertexArray::Bind() const
	{
		glBindVertexArray(mRendererID);
	}

	void OpenGLVertexArray::Unbind() const
	{
		glBindVertexArray(0);
	}

	void OpenGLVertexArray::AddVertexBuffer(const Ref<VertexBuffer>& vertexBuffer)
	{
		CL_CORE_ASSERT(vertexBuffer->GetLayout().GetElements().size(), "Vertex Buffer has no layout!");

		glBindVertexArray(mRendererID);
		vertexBuffer->Bind();

		uint32_t Index = 0;
		const auto& Layout = vertexBuffer->GetLayout();

		for (const auto& Element : Layout)
		{
			glEnableVertexAttribArray(Index);
			glVertexAttribPointer(Index, Element.GetComponentCount(), ShaderDataTypeToOpenGLBaseType(Element.GetType()),
				Element.GetNormalized() ? GL_TRUE : GL_FALSE, Layout.GetStride(), (const void*)Element.GetOffset());
			Index++;
		}

		mVertexBuffers.push_back(vertexBuffer);
	}

	void OpenGLVertexArray::SetIndexBuffer(const Ref<IndexBuffer>& indexBuffer)
	{
		glBindVertexArray(mRendererID);
		indexBuffer->Bind();

		mIndexBuffer = indexBuffer;
	}

}

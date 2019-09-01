#include "Crpch.h"
#include "OpenGLIndexBuffer.h"

#include "glad/glad.h"

namespace Crystal
{
	OpenGLIndexBuffer::OpenGLIndexBuffer(unsigned int Size)
		: mRendererID(0), mSize(Size)
	{
		CL_RENDER_S({
			glGenBuffers(1, &self->mRendererID);
		});
	}

	OpenGLIndexBuffer::~OpenGLIndexBuffer()
	{
		CL_RENDER_S({
			glDeleteBuffers(1, &self->mRendererID);
		});
	}

	void OpenGLIndexBuffer::SetData(void* Buffer, unsigned int Size, unsigned int Offset /*= 0*/)
	{
		mSize = Size;

		CL_RENDER_S3(Buffer, Size, Offset, {
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->mRendererID);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, Size, Buffer, GL_STATIC_DRAW);
		});
	}

	void OpenGLIndexBuffer::Bind() const
	{
		CL_RENDER_S({
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, self->mRendererID);
		});
	}

	void OpenGLIndexBuffer::UnBind() const
	{
		CL_RENDER_S({
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
		});
	}
}


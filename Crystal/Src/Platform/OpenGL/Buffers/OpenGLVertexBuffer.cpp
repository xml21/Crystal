#include "Crpch.h"
#include "OpenGLVertexBuffer.h"

#include "glad/glad.h"

namespace Crystal
{
	OpenGLVertexBuffer::OpenGLVertexBuffer(unsigned int Size)
		: mRendererID(0), mSize(Size)
	{
		CL_RENDER_S({
			glGenBuffers(1, &self->mRendererID);
		});
	}

	OpenGLVertexBuffer::~OpenGLVertexBuffer()
	{
		CL_RENDER_S({
			glDeleteBuffers(1, &self->mRendererID);
		});
	}

	void OpenGLVertexBuffer::SetData(void* Buffer, unsigned int Size, unsigned int Offset /*= 0*/)
	{
		mSize = Size;

		CL_RENDER_S3(Buffer, Size, Offset, {
			glBindBuffer(GL_ARRAY_BUFFER, self->mRendererID);
			//Upload data from CPU to GPU
			glBufferData(GL_ARRAY_BUFFER, Size, Buffer, GL_STATIC_DRAW);
		});
	}

	void OpenGLVertexBuffer::Bind() const
	{
		CL_RENDER_S({
			glBindBuffer(GL_ARRAY_BUFFER, self->mRendererID);

			//Tmp solution (Providing positions and texcoord attributes) ----------------------------------------
			glEnableVertexAttribArray(0);
			glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0);

			glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (const void*)(3 * sizeof(float)));
			//Tmp solution (Providing positions and texcoord attributes) ----------------------------------------
		});
	}

	void OpenGLVertexBuffer::UnBind() const
	{
		CL_RENDER_S({
			glBindBuffer(GL_ARRAY_BUFFER, 0);
		});
	}
}

#include "Crpch.h"

#include "OpenGLFramebuffer.h"

#include "Crystal/Renderer/Renderer.h"
#include <glad/glad.h>

namespace Crystal 
{
	OpenGLFramebuffer::OpenGLFramebuffer(uint32_t Width, uint32_t Height, FramebufferFormat Format)
		: mWidth(Width), mHeight(Height), mFormat(Format)
	{
		Resize(Width, Height);
	}

	OpenGLFramebuffer::~OpenGLFramebuffer()
	{
		CL_RENDER_S({
			glDeleteFramebuffers(1, &self->mRendererID);
		});
	}

	void OpenGLFramebuffer::Resize(uint32_t Width, uint32_t Height)
	{
		if (mWidth == Width && mHeight == Height)
			return;

		mWidth = Width;
		mHeight = Height;

		CL_RENDER_S({
			if(self->mRendererID)
			{
				glDeleteFramebuffers(1, &self->mRendererID);
				glDeleteTextures(1, &self->mColorAttachment);
				glDeleteTextures(1, &self->mDepthAttachment);
			}

			glGenFramebuffers(1, &self->mRendererID);
			glBindFramebuffer(GL_FRAMEBUFFER, self->mRendererID);

			glGenTextures(1, &self->mColorAttachment);
			glBindTexture(GL_TEXTURE_2D, self->mColorAttachment);

			// TODO: Create Crystal texture object based on format here
			if (self->mFormat == FramebufferFormat::RGBA16F)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA16F, self->mWidth, self->mHeight, 0, GL_RGBA, GL_FLOAT, nullptr);
			}
			else if (self->mFormat == FramebufferFormat::RGBA8)
			{
				glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, self->mWidth, self->mHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
			}

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, self->mColorAttachment, 0);

			glGenTextures(1, &self->mDepthAttachment);
			glBindTexture(GL_TEXTURE_2D, self->mDepthAttachment);
			glTexImage2D(
				GL_TEXTURE_2D, 0, GL_DEPTH24_STENCIL8, self->mWidth, self->mHeight, 0,
				GL_DEPTH_STENCIL, GL_UNSIGNED_INT_24_8, NULL
			);

			glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_TEXTURE_2D, self->mDepthAttachment, 0);

			if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				CL_CORE_LOG_ERROR("Framebuffer is incomplete!");

			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFramebuffer::Bind() const
	{
		CL_RENDER_S({
			glBindFramebuffer(GL_FRAMEBUFFER, self->mRendererID);
			glViewport(0, 0, self->mWidth, self->mHeight);
		});
	}

	void OpenGLFramebuffer::Unbind() const
	{
		CL_RENDER_S({
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
		});
	}

	void OpenGLFramebuffer::BindTexture(uint32_t Slot) const
	{
		CL_RENDER_S1(Slot, {
			glActiveTexture(GL_TEXTURE0 + Slot);
			glBindTexture(GL_TEXTURE_2D, self->mColorAttachment);
		});
	}
}
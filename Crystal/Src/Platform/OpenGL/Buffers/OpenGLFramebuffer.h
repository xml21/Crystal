#pragma once

#include "Crystal/Renderer/Framebuffer.h"

namespace Crystal {

	class OpenGLFramebuffer : public Framebuffer
	{
	public:
		OpenGLFramebuffer(uint32_t Width, uint32_t Height, FramebufferFormat Format);
		virtual ~OpenGLFramebuffer();

		virtual void Resize(uint32_t Width, uint32_t Height) override;

		virtual void Bind() const override;
		virtual void Unbind() const override;

		virtual void BindTexture(uint32_t Slot = 0) const override;

		virtual RendererID GetRendererID() const { return mRendererID; }
		virtual RendererID GetColorAttachmentRendererID() const { return mColorAttachment; }
		virtual RendererID GetDepthAttachmentRendererID() const { return mDepthAttachment; }

		virtual uint32_t GetWidth() const { return mWidth; }
		virtual uint32_t GetHeight() const { return mHeight; }
		virtual FramebufferFormat GetFormat() const { return mFormat; }
	private:
		RendererID mRendererID = 0;
		uint32_t mWidth, mHeight;
		FramebufferFormat mFormat;

		RendererID mColorAttachment, mDepthAttachment;
	};
}

#pragma once

#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal {

	enum class FramebufferFormat
	{
		None = 0,
		RGBA8 = 1,
		RGBA16F = 2
	};

	class Framebuffer
	{
	public:
		static Ref<Framebuffer> Create(uint32_t Width, uint32_t Height, FramebufferFormat ormat);

		virtual ~Framebuffer() {}
		virtual void Bind() const = 0;
		virtual void Unbind() const = 0;

		virtual void Resize(uint32_t Width, uint32_t Height) = 0;

		virtual void BindTexture(uint32_t Slot = 0) const = 0;

		virtual RendererID GetRendererID() const = 0;
		virtual RendererID GetColorAttachmentRendererID() const = 0;
		virtual RendererID GetDepthAttachmentRendererID() const = 0;
	};

	class FramebufferPool final
	{
	public:
		FramebufferPool(uint32_t MaxFBs = 32);
		~FramebufferPool();

		std::weak_ptr<Framebuffer> AllocateBuffer();
		void Add(Ref<Framebuffer> NewFramebuffer);

		const std::vector<Ref<Framebuffer>>& GetAll() const { return mPool; }

		inline static Ref<FramebufferPool> GetGlobal() { return sInstance; }

	private:
		std::vector<Ref<Framebuffer>> mPool;

		static Ref<FramebufferPool> sInstance;
	};
}

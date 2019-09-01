#include "Crpch.h"
#include "Framebuffer.h"
#include "RendererAPI.h"

#include "Platform/OpenGL/Buffers/OpenGLFramebuffer.h"

namespace Crystal
{
	Ref<Crystal::Framebuffer> Framebuffer::Create(uint32_t Width, uint32_t Height, FramebufferFormat Format)
	{
		Ref<Crystal::Framebuffer> Result = nullptr;

		switch (RendererAPI::GetAPI())
		{
			case API::NONE:		return nullptr;
			case API::OpenGL:	Result = std::make_shared<OpenGLFramebuffer>(Width, Height, Format);
		}

		FramebufferPool::GetGlobal()->Add(Result);

		return Result;
	}

	Ref<FramebufferPool> FramebufferPool::sInstance = std::make_shared<FramebufferPool>();

	void FramebufferPool::Add(Ref<Framebuffer> NewFramebuffer)
	{
		mPool.push_back(NewFramebuffer);
	}

	FramebufferPool::FramebufferPool(uint32_t MaxFBs /*= 32*/)
	{

	}

	FramebufferPool::~FramebufferPool()
	{

	}

	std::weak_ptr<Crystal::Framebuffer> FramebufferPool::AllocateBuffer()
	{
		return std::weak_ptr<Framebuffer>();
	}
}
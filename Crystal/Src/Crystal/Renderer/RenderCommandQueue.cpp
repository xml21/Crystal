#include "Crpch.h"

#include "RenderCommandQueue.h"


namespace Crystal {

	RenderCommandQueue::RenderCommandQueue()
	{
		mCommandBuffer = new unsigned char[10 * 1024 * 1024]; // 10mb buffer
		mCommandBufferPtr = mCommandBuffer;
		memset(mCommandBuffer, 0, 10 * 1024 * 1024);
	}

	RenderCommandQueue::~RenderCommandQueue()
	{
		delete[] mCommandBuffer;
	}

	void* RenderCommandQueue::Allocate(RenderCommandFn Fn, unsigned int Size)
	{
		*(RenderCommandFn*)mCommandBufferPtr = Fn;
		mCommandBufferPtr += sizeof(RenderCommandFn);

		*(int*)mCommandBufferPtr = Size;
		mCommandBufferPtr += sizeof(unsigned int);

		void* Memory = mCommandBufferPtr;
		mCommandBufferPtr += Size;

		mCommandCount++;
		return Memory;
	}

	void RenderCommandQueue::Execute()
	{
		byte* Buffer = mCommandBuffer;

		for (unsigned int i = 0; i < mCommandCount; i++)
		{
			RenderCommandFn Function = *(RenderCommandFn*)Buffer;
			Buffer += sizeof(RenderCommandFn);

			unsigned int Size = *(unsigned int*)Buffer;
			Buffer += sizeof(unsigned int);
			Function(Buffer);
			Buffer += Size;
		}

		mCommandBufferPtr = mCommandBuffer;
		mCommandCount = 0;
	}

}
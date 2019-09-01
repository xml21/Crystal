#pragma once

#include "Crpch.h"

namespace Crystal {

	/* Render command queue class made for rendering objects sorting/optimization purposes. */
	class CRYSTAL_API RenderCommandQueue
	{
	public:
		typedef void(*RenderCommandFn)(void*);

		RenderCommandQueue();
		~RenderCommandQueue();

		void* Allocate(RenderCommandFn Func, unsigned int Size);

		void Execute();

	private:
		unsigned char* mCommandBuffer;
		unsigned char* mCommandBufferPtr;
		unsigned int mCommandCount = 0;
	};
}

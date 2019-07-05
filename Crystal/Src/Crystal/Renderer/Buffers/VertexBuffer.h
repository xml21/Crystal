#pragma once

namespace Crystal
{
	class VertexBuffer
	{
	public:
		virtual ~VertexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		//TODO: Consider adding smart pointer for reference counting
		static VertexBuffer* Create(float* vertices, uint32_t size);
	};
}

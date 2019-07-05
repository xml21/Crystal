#pragma once

namespace Crystal
{
	class IndexBuffer
	{
	public:
		virtual ~IndexBuffer() {}

		virtual void Bind() const = 0;
		virtual void UnBind() const = 0;

		virtual uint32_t GetCount() const = 0;

		//TODO: Consider adding smart pointer for reference counting
		static IndexBuffer* Create(uint32_t* indices, uint32_t size);
	};
}
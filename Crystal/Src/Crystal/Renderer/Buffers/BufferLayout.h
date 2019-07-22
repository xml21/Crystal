#pragma once

namespace Crystal
{

	enum class ShaderDataType
	{
		None = 0, Float, Float2, Float3, Float4, Mat3, Mat4, Int, Int2, Int3, Int4, Bool
	};

	static uint32_t GetShaderDataTypeSize(ShaderDataType Type)
	{
		switch (Type)
		{
		case ShaderDataType::Float:    return 4;
		case ShaderDataType::Float2:   return 4 * 2;
		case ShaderDataType::Float3:   return 4 * 3;
		case ShaderDataType::Float4:   return 4 * 4;
		case ShaderDataType::Mat3:     return 4 * 3 * 3;
		case ShaderDataType::Mat4:     return 4 * 4 * 4;
		case ShaderDataType::Int:      return 4;
		case ShaderDataType::Int2:     return 4 * 2;
		case ShaderDataType::Int3:     return 4 * 3;
		case ShaderDataType::Int4:     return 4 * 4;
		case ShaderDataType::Bool:     return 1;
		}

		CL_CORE_ASSERT(false, "Unknown ShaderDataType");
		return 0;
	}

	struct BufferElement
	{
		std::string Name;

		ShaderDataType Type;

		uint32_t Offset;
		uint32_t Size;

		bool bNormalized;

		inline const ShaderDataType& GetType() const { return Type; }
		inline uint32_t GetOffset() const { return Offset; }
		inline bool GetNormalized() const { return bNormalized; }

		void SetType(const ShaderDataType& type) { Type = type; }
		void SetOffset(const uint32_t offset) { Offset = offset; }
		void SetNormalized(const bool normalized) { bNormalized = normalized; }

		uint32_t GetComponentCount() const
		{
			switch (Type)
			{
			case ShaderDataType::Float:   return 1;
			case ShaderDataType::Float2:  return 2;
			case ShaderDataType::Float3:  return 3;
			case ShaderDataType::Float4:  return 4;
			case ShaderDataType::Mat3:    return 3 * 3;
			case ShaderDataType::Mat4:    return 4 * 4;
			case ShaderDataType::Int:     return 1;
			case ShaderDataType::Int2:    return 2;
			case ShaderDataType::Int3:    return 3;
			case ShaderDataType::Int4:    return 4;
			case ShaderDataType::Bool:    return 1;
			}

			CL_CORE_ASSERT(false, "Unknown ShaderDataType!");
			return 0;
		}

		BufferElement(ShaderDataType type, const std::string name, bool normalized = false)
			: Type(type), Name(name), Offset(0), Size(GetShaderDataTypeSize(type)), bNormalized(normalized)
		{

		}
	};

	class BufferLayout
	{
	public:
		BufferLayout () = default;
		BufferLayout(const std::initializer_list<BufferElement>& elements);

		inline const std::vector<BufferElement>& GetElements() const { return mElements; };
		inline uint32_t GetStride() const { return mStride; };

		std::vector<BufferElement>::iterator begin() { return mElements.begin(); }
		std::vector<BufferElement>::iterator end() { return mElements.end(); }
		std::vector<BufferElement>::const_iterator begin() const { return mElements.begin(); }
		std::vector<BufferElement>::const_iterator end() const { return mElements.end(); }

	private:
		std::vector<BufferElement> mElements;
		uint32_t mStride = 0;

		void CalculateOffsetAndStride();
	};
}


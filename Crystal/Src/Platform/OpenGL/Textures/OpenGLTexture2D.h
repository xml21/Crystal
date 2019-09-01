#pragma once

#include "Crystal/Renderer/Textures/Texture2D.h"
#include "Crystal/Renderer/RendererAPI.h"

namespace Crystal
{
	class OpenGLTexture2D : public Texture2D
	{
	public:
		OpenGLTexture2D(const std::string& Path, bool sRGB);
		OpenGLTexture2D(TextureFormat Format, unsigned int Width, unsigned int Height);
		virtual ~OpenGLTexture2D();

		virtual TextureFormat GetFormat() const { return mFormat; }
		uint32_t GetWidth() const override { return mWidth; };
		uint32_t GetHeight() const override { return mHeight; };

		virtual RendererID GetRendererID() const override { return mRendererID; }
		virtual const std::string& GetPath() const override { return mFilePath; }

		void Bind(unsigned int Slot = 0) const override;

	private:
		uint32_t mWidth, mHeight;
		RendererID mRendererID;

		TextureFormat mFormat;

		unsigned char* mImageData;

		std::string mFilePath;
	};
}

#pragma once

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Renderer/Textures/TextureCube.h"

namespace Crystal
{
	class OpenGLTextureCube : public TextureCube
	{
	public:
		OpenGLTextureCube(const std::string& Path);
		virtual ~OpenGLTextureCube();

		virtual TextureFormat GetFormat() const { return mFormat; }
		virtual unsigned int GetWidth() const { return mWidth; }
		virtual unsigned int GetHeight() const { return mHeight; }

		virtual const std::string& GetPath() const override { return mFilePath; }

		virtual RendererID GetRendererID() const override { return mRendererID; }

		virtual void Bind(unsigned int Slot = 0) const override;

	private:
		RendererID mRendererID;
		TextureFormat mFormat;
		unsigned int mWidth, mHeight;

		unsigned char* mImageData;

		std::string mFilePath;
	};
}


#include "Crpch.h"

#include "OpenGLTexture2D.h"
#include "OpenGLTexture.h"

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Renderer/Renderer.h"

#include "glad/glad.h"
#include "stb_image.h"

namespace Crystal
{
	OpenGLTexture2D::OpenGLTexture2D(TextureFormat Format, unsigned int Width, unsigned int Height)
		: mFormat(Format), mWidth(Width), mHeight(Height)
	{
		auto self = this;

		CL_RENDER_1(self, {
			glGenTextures(1, &self->mRendererID);
			glBindTexture(GL_TEXTURE_2D, self->mRendererID);

			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameterf(self->mRendererID, GL_TEXTURE_MAX_ANISOTROPY, RendererAPI::GetCapabilities().MaxAnisotropy);

			glTexImage2D(GL_TEXTURE_2D, 0, CrystalToOpenGLTextureFormat(self->mFormat), self->mWidth, self->mHeight, 0, CrystalToOpenGLTextureFormat(self->mFormat), GL_UNSIGNED_BYTE, nullptr);
			glGenerateMipmap(GL_TEXTURE_2D);

			glBindTexture(GL_TEXTURE_2D, 0);
		});
	}

	OpenGLTexture2D::OpenGLTexture2D(const std::string& Path, bool sRGB)
		: mFilePath(Path)
	{
		int width, height, channels;
		CL_CORE_LOG_INFO("Loading texture {0}, sRGB={1}", Path, sRGB);
		mImageData = stbi_load(Path.c_str(), &width, &height, &channels, sRGB ? STBI_rgb : STBI_rgb_alpha);

		mWidth = width;
		mHeight = height;
		mFormat = TextureFormat::RGBA;
		
		CL_RENDER_S1(sRGB, {
			// TODO: Consolidate properly
			if (sRGB)
			{
				glCreateTextures(GL_TEXTURE_2D, 1, &self->mRendererID);
				int Levels = CalculateMipMapCount(self->mWidth, self->mHeight);
				CL_CORE_LOG_INFO("Creating sRGB texture width {0} mips", Levels);
				glTextureStorage2D(self->mRendererID, Levels, GL_SRGB8, self->mWidth, self->mHeight);
				glTextureParameteri(self->mRendererID, GL_TEXTURE_MIN_FILTER, Levels > 1 ? GL_LINEAR_MIPMAP_LINEAR : GL_LINEAR);
				glTextureParameteri(self->mRendererID, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

				glTextureSubImage2D(self->mRendererID, 0, 0, 0, self->mWidth, self->mHeight, GL_RGB, GL_UNSIGNED_BYTE, self->mImageData);
				glGenerateTextureMipmap(self->mRendererID);
			}
			else
			{
				glGenTextures(1, &self->mRendererID);
				glBindTexture(GL_TEXTURE_2D, self->mRendererID);

				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
				glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

				glTexImage2D(GL_TEXTURE_2D, 0, CrystalToOpenGLTextureFormat(self->mFormat), self->mWidth, self->mHeight, 0, sRGB ? GL_SRGB8 : CrystalToOpenGLTextureFormat(self->mFormat), GL_UNSIGNED_BYTE, self->mImageData);
				glGenerateMipmap(GL_TEXTURE_2D);

				glBindTexture(GL_TEXTURE_2D, 0);
			}
			stbi_image_free(self->mImageData);
		});
	}

	OpenGLTexture2D::~OpenGLTexture2D()
	{
		auto self = this;

		CL_RENDER_1(self, {
			glDeleteTextures(1, &self->mRendererID);
		});
	}

	void OpenGLTexture2D::Bind(unsigned int Slot) const
	{
		CL_RENDER_S1(Slot, {
			glBindTextureUnit(Slot, self->mRendererID);
		})
	}
}
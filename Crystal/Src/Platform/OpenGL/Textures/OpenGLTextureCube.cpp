#include "Crpch.h"
#include "OpenGLTextureCube.h"

#include "glad/glad.h"
#include "stb_image.h"

#include "Crystal/Renderer/RendererAPI.h"
#include "Crystal/Renderer/Renderer.h"

#include "OpenGLTexture.h"

namespace Crystal
{
	OpenGLTextureCube::OpenGLTextureCube(const std::string& Path)
		: mFilePath(Path)
	{
		int Width, Height, Channels;
		stbi_set_flip_vertically_on_load(false);
		mImageData = stbi_load(Path.c_str(), &Width, &Height, &Channels, STBI_rgb);

		mWidth = Width;
		mHeight = Height;
		mFormat = TextureFormat::RGB;

		unsigned int FaceWidth = mWidth / 4;
		unsigned int FaceHeight = mHeight / 3;
		CL_CORE_ASSERT(FaceWidth == FaceHeight, "Non-square Faces!");

		std::array<unsigned char*, 6> Faces;
		for (size_t i = 0; i < Faces.size(); i++)
			Faces[i] = new unsigned char[FaceWidth * FaceHeight * 3]; // 3 BPP

		int FaceIndex = 0;

		for (size_t i = 0; i < 4; i++)
		{
			for (size_t y = 0; y < FaceHeight; y++)
			{
				size_t yOffset = y + FaceHeight;
				for (size_t x = 0; x < FaceWidth; x++)
				{
					size_t xOffset = x + i * FaceWidth;
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 0] = mImageData[(xOffset + yOffset * mWidth) * 3 + 0];
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 1] = mImageData[(xOffset + yOffset * mWidth) * 3 + 1];
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 2] = mImageData[(xOffset + yOffset * mWidth) * 3 + 2];
				}
			}
			FaceIndex++;
		}

		for (size_t i = 0; i < 3; i++)
		{
			// Skip the middle one
			if (i == 1)
				continue;

			for (size_t y = 0; y < FaceHeight; y++)
			{
				size_t yOffset = y + i * FaceHeight;
				for (size_t x = 0; x < FaceWidth; x++)
				{
					size_t xOffset = x + FaceWidth;
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 0] = mImageData[(xOffset + yOffset * mWidth) * 3 + 0];
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 1] = mImageData[(xOffset + yOffset * mWidth) * 3 + 1];
					Faces[FaceIndex][(x + y * FaceWidth) * 3 + 2] = mImageData[(xOffset + yOffset * mWidth) * 3 + 2];
				}
			}
			FaceIndex++;
		}

		CL_RENDER_S3(Faces, FaceWidth, FaceHeight, {
			glGenTextures(1, &self->mRendererID);
			glBindTexture(GL_TEXTURE_CUBE_MAP, self->mRendererID);

			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
			glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
			glTextureParameterf(self->mRendererID, GL_TEXTURE_MAX_ANISOTROPY, Crystal::RendererAPI::GetCapabilities().MaxAnisotropy);

			auto Format = Crystal::CrystalToOpenGLTextureFormat(self->mFormat);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[2]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[0]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[4]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[5]);

			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[1]);
			glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0, Format, FaceWidth, FaceHeight, 0, Format, GL_UNSIGNED_BYTE, Faces[3]);

			glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

			glBindTexture(GL_TEXTURE_2D, 0);

			for (size_t i = 0; i < Faces.size(); i++)
				delete[] Faces[i];

			stbi_image_free(self->mImageData);
		});
	}

	OpenGLTextureCube::~OpenGLTextureCube()
	{
		auto self = this;
		CL_RENDER_1(self, {
			glDeleteTextures(1, &self->mRendererID);
		});
	}

	void OpenGLTextureCube::Bind(unsigned int Slot) const
	{
		CL_RENDER_S1(Slot, {
			glActiveTexture(GL_TEXTURE0 + Slot);
			glBindTexture(GL_TEXTURE_CUBE_MAP, self->mRendererID);
		});
	}
}


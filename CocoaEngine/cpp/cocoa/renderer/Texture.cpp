#include "externalLibs.h"

#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/core/AssetManager.h"

#include <stb_image.h>

namespace Cocoa
{
	Texture Texture::nullTexture = {};

	Texture::Texture()
	{
		m_IsNull = true;
	}

	Texture::Texture(int width, int height, bool isDefault)
	{
		m_IsDefault = isDefault;
		m_Path = "";
		m_Height = height;
		m_Width = width;

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

		m_BoundingBox = Physics2DSystem::AABBFrom(glm::vec2{ 0, 0 }, glm::vec2{ width, height });
		m_PixelsFreed = true;

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			Log::Error("Error creating texture (width, height) (GL ERROR): 0x%x", error);
		}
	}

	Texture::Texture(const CPath& resourceName, bool isDefault)
	{
		m_IsDefault = isDefault;
		m_Height = 0;
		m_Width = 0;
		m_ID = -1;
		this->m_Path = CPath(resourceName);
	}

	void Texture::Load()
	{
		int channels;
		int width;
		int height;

		m_PixelBuffer = stbi_load(m_Path.Filepath(), &width, &height, &channels, 0);
		Log::Assert((m_PixelBuffer != nullptr), "STB failed to load image: %s\n-> STB Failure Reason: %s", m_Path.Filepath(), stbi_failure_reason());

		m_Width = width;
		m_Height = height;
		m_BytesPerPixel = channels;

		// Generate bounding box for this texture, this can be attached to any object using this texture
		m_BoundingBox = Physics2D::GetBoundingBoxForPixels(m_PixelBuffer, width, height, channels);

		glGenTextures(1, &m_ID);
		glBindTexture(GL_TEXTURE_2D, m_ID);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (channels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, m_PixelBuffer);
		}
		else if (channels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, m_PixelBuffer);
		}
		else
		{
			Log::Assert(false, "Unknown number of channels '%d'. In File: '%s'", channels, m_Path.Filepath());
		}

		GLenum error = glGetError();
		if (error != GL_NO_ERROR)
		{
			Log::Error("Error loading texture %s (GL ERROR): 0x%x", m_Path.Filepath(), error);
		}
	}

	void Texture::Delete()
	{
		if (m_PixelBuffer)
		{
			stbi_image_free(m_PixelBuffer);
			m_PixelBuffer = nullptr;
			m_PixelsFreed = true;
		}
		glDeleteTextures(1, &m_ID);
	}

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_ID);
	}

	void Texture::FreePixels()
	{
		if (!m_PixelsFreed)
		{
			stbi_image_free(m_PixelBuffer);
			m_PixelBuffer = nullptr;
			m_PixelsFreed = true;
		}
	}

	const uint8* Texture::GetPixelBuffer() const
	{
		return m_PixelBuffer;
	}

	json Texture::Serialize()
	{
		return {
			{"Filepath", m_Path.Filepath()}
		};
	}
}
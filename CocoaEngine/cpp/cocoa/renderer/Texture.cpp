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
		m_Height = 0;
		m_Width = 0;
		m_IsDefault = false;
		m_Id = -1;
	}

	Texture::Texture(int width, int height, bool isDefault)
	{
		m_IsDefault = isDefault;
		m_Path = "";
		m_Height = height;
		m_Width = width;

		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

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
		m_Id = -1;
		this->m_Path = CPath(resourceName);
	}

	void Texture::Load()
	{
		int channels;
		int width;
		int height;

		unsigned char* pixels = stbi_load(m_Path.Filepath(), &width, &height, &channels, 0);
		Log::Assert((pixels != nullptr), "STB failed to load image: %s\n-> STB Failure Reason: %s", m_Path.Filepath(), stbi_failure_reason());

		m_Width = width;
		m_Height = height;
		int bytesPerPixel = channels;

		glGenTextures(1, &m_Id);
		glBindTexture(GL_TEXTURE_2D, m_Id);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

		if (channels == 4)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
		}
		else if (channels == 3)
		{
			glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB8, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, pixels);
		}
		else
		{
			Log::Assert(false, "Unknown number of channels '%d'. In File: '%s'", channels, m_Path.Filepath());
		}

		stbi_image_free(pixels);
	}

	void Texture::Delete()
	{
		glDeleteTextures(1, &m_Id);
	}

	void Texture::Bind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);
	}

	void Texture::Unbind() const
	{
		glBindTexture(GL_TEXTURE_2D, m_Id);
	}

	json Texture::Serialize()
	{
		return {
			{"Filepath", m_Path.Filepath()}
		};
	}
}
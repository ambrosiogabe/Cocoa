#include "externalLibs.h"

#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"
#include "jade/physics2d/Physics2D.h"
#include "jade/physics2d/Physics2DSystem.h"

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

namespace Jade {
    Texture::Texture(int width, int height) {
        m_Filepath = "";
        m_Height = height;
        m_Width = width;

        glGenTextures(1, &m_ID);
        glBindTexture(GL_TEXTURE_2D, m_ID);

        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);

        m_BoundingBox = Physics2DSystem::AABBFrom(glm::vec2{ 0, 0 }, glm::vec2{ width, height });
        m_PixelsFreed = true;
    }

    Texture::Texture(const char* resourceName) {
        GenerateTypeId<Texture>();
        m_Filepath = resourceName;
    }

    void Texture::Load()
    {
        int channels;
        int width;
        int height;

        m_PixelBuffer = stbi_load(m_Filepath, &width, &height, &channels, 0);
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
            Log::Assert(false, "Unknown number of channels '%d'. In File: '%s'", channels, m_Filepath);
        }
    }

    void Texture::Unload()
    {
        stbi_image_free(m_PixelBuffer);
        m_PixelBuffer = nullptr;
        m_PixelsFreed = true;
        glDeleteTextures(1, &m_ID);
        m_Loaded = false;
    }

    void Texture::Bind() {
        glBindTexture(GL_TEXTURE_2D, m_ID);
    }

    void Texture::Unbind() {
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

    const uint8* Texture::GetPixelBuffer()
    {
        return m_PixelBuffer;
    }
}
#pragma once

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/core/Core.h"

namespace Jade {
    class Texture {
    public:
        Texture(const char* resourceName, bool freePixelsOnLoad=true);
        Texture(int width, int height);

        void Bind();
        void Unbind();

        int GetId() const { return m_ID; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        const char* GetFilepath() const { return m_Filepath; }
        void FreePixels();
        const uint8* GetPixelBuffer();
        int BytesPerPixel() const { return m_BytesPerPixel; }

    private:
        unsigned int m_ID;
        int m_Width, m_Height;
        int m_BytesPerPixel = 3;

        uint8* m_PixelBuffer = nullptr;
        bool m_PixelsFreed = false;

        const char* m_Filepath;
        BoundingBox m_BoundingBox;
    };
}
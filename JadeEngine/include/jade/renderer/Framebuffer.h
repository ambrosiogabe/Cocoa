#pragma once

#include "jade/renderer/Texture.h"

namespace Jade {
    class JADE Framebuffer {
    public:
        Framebuffer(int width, int height)
            : m_Width(width), m_Height(height) {
            Init();
        }

        void Init();

        Texture* GetTexture() const { return m_Texture; }
        unsigned int GetId() const { return m_ID; }
        int GetWidth() const { return m_Width; }
        int GetHight() const { return m_Height; }

    private:
        int m_Width = 0;
        int m_Height = 0;
        unsigned int m_ID = 0;
        Texture* m_Texture = nullptr;
    };
}
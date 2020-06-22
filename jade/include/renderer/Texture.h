#pragma once

namespace Jade {
    class Texture {
    public:
        Texture(const char* resourceName);
        Texture(int width, int height);

        void Bind();
        void Unbind();

        int GetId() const { return m_ID; }
        int GetWidth() const { return m_Width; }
        int GetHeight() const { return m_Height; }
        const char* GetFilepath() const { return m_Filepath; }

    private:
        unsigned int m_ID;
        int m_Width, m_Height;
        const char* m_Filepath;
    };
}
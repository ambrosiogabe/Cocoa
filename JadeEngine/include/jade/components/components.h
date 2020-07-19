#pragma once
#include "externalLibs.h"

#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"
#include "jade/physics2d/Physics2D.h"

namespace Jade {
    struct Sprite {
        const char* m_PictureFile = nullptr;
        int m_Width = 32;
        int m_Height = 32;
        Texture* m_Texture = nullptr;
        AABB m_BoundingBox = Physics2DSystem::AABBFrom({ 0, 0 }, { m_Width, m_Height });
        glm::vec2 m_TexCoords[4] = {
            glm::vec2(1.0f, 1.0f),
            glm::vec2(1.0f, 0.0f),
            glm::vec2(0.0f, 0.0f),
            glm::vec2(0.0f, 1.0f)
        };
    };

    class Spritesheet {
    public:
        Texture* m_Texture;
        std::vector<Sprite> m_Sprites;

        Spritesheet(Texture* texture, int spriteWidth, int spriteHeight, int numSprites, int spacing) {
            // NOTE: If you don't reserve the space before hand, when the vector grows it may
            // change the pointers it holds
            m_Sprites.reserve(numSprites);

            m_Texture = texture;
            const uint8* rawPixels = texture->GetPixelBuffer();

            int bytesPerPixel = m_Texture->BytesPerPixel();
            int area = spriteWidth * spriteHeight * bytesPerPixel;
            // This will be used to house temporary sprite data to calculate bounding boxes for all sprites
            std::unique_ptr<uint8> tmpSubImage(new uint8[area]);

            int currentX = 0;
            int currentY = m_Texture->GetHeight() - spriteHeight;
            for (int i=0; i < numSprites; i++) 
            {
                float topY = (currentY + spriteHeight) / (float)texture->GetHeight();
                float rightX = (currentX + spriteWidth) / (float)texture->GetWidth();
                float leftX = currentX / (float)texture->GetWidth();
                float bottomY = currentY / (float)texture->GetHeight();

                for (int y = 0; y < spriteHeight; y++)
                {
                    for (int x = 0; x < spriteWidth; x++)
                    {
                        int absY = y + currentY;
                        int absX = x + currentX;
                        int offset = (absX + absY * m_Texture->GetWidth()) * bytesPerPixel;
                        const uint8* pixel = rawPixels + offset;
                        tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 0] = *(pixel + 0); // R
                        tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 1] = *(pixel + 1); // G
                        tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 2] = *(pixel + 2); // B
                        tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 3] = *(pixel + 3); // A
                    }
                }
                AABB boundingBox = Physics2D::GetBoundingBoxForPixels(tmpSubImage.get(), spriteWidth, spriteHeight, texture->BytesPerPixel());

                Sprite sprite = {
                    texture->GetFilepath(),
                    spriteWidth,
                    spriteHeight,
                    texture,
                    boundingBox, 
                    {
                        glm::vec2(rightX, topY),
                        glm::vec2(rightX, bottomY),
                        glm::vec2(leftX, bottomY),
                        glm::vec2(leftX, topY)
                    }
                };
                m_Sprites.push_back(sprite);

                currentX += spriteWidth + spacing;
                if (currentX >= texture->GetWidth()) {
                    currentX = 0;
                    currentY -= spriteHeight + spacing;
                }
            }
            m_Texture->FreePixels();
        }

        Sprite& GetSprite(int index) {
            return m_Sprites[index];
        }

        int Size()
        {
            return (int)m_Sprites.size();
        }
    };

    static Sprite* defaultSprite = new Sprite();
    struct SpriteRenderer {
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Sprite* m_Sprite = defaultSprite;
    };
}
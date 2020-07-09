#pragma once

#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"
#include "jade/physics2d/Physics2D.h"

#include <entt.h>
#include <glm/vec4.hpp>
#include <glm/vec3.hpp>
#include <glm/vec2.hpp>
#include <glm/gtx/quaternion.hpp>
#include <glm/gtx/euler_angles.hpp>
#include <vector>

namespace Jade {
    struct Sprite {
        const char* m_PictureFile = nullptr;
        int m_Width = 32;
        int m_Height = 32;
        Texture* m_Texture = nullptr;
        BoundingBox m_BoundingBox = BoundingBox();
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
            // This will be used to house temporary sprite data to calculate bounding boxes for all sprites
            int bytesPerPixel = m_Texture->BytesPerPixel();
            std::unique_ptr<uint8[]> tmpSubImage(new uint8[spriteWidth * spriteHeight * bytesPerPixel]);

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
                        const uint8* pixel = rawPixels + ((absX + absY * m_Texture->GetWidth()) * bytesPerPixel);
                        tmpSubImage[(x + y * spriteWidth) * bytesPerPixel + 0] = *(pixel + 0); // R
                        tmpSubImage[(x + y * spriteWidth) * bytesPerPixel + 1] = *(pixel + 1); // G
                        tmpSubImage[(x + y * spriteWidth) * bytesPerPixel + 2] = *(pixel + 2); // B
                        tmpSubImage[(x + y * spriteWidth) * bytesPerPixel + 3] = *(pixel + 3); // A
                    }
                }
                BoundingBox boundingBox = Physics2D::GetBoundingBoxForPixels(tmpSubImage.get(), spriteWidth, spriteHeight, texture->BytesPerPixel());

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
            return m_Sprites.size();
        }
    };

    static Sprite* defaultSprite = new Sprite();
    struct SpriteRenderer {
        glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
        Sprite* m_Sprite = defaultSprite;
    };
}
#pragma once
#include "externalLibs.h"

#include "jade/physics2d/Physics2D.h"
#include "jade/renderer/Texture.h"
#include "jade/util/Log.h"

namespace Jade
{
	struct Sprite
	{
		std::shared_ptr<Texture> m_Texture = nullptr;
		int m_Width = 32;
		int m_Height = 32;
		AABB m_BoundingBox = Physics2DSystem::AABBFrom({ 0, 0 }, { m_Width, m_Height });
		glm::vec2 m_TexCoords[4] = {
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f)
		};
	};

	class Spritesheet
	{
	public:
		std::shared_ptr<Texture> m_Texture;
		std::vector<Sprite> m_Sprites;

		Spritesheet(Texture* texture, std::vector<Sprite> sprites)
			: m_Sprites(std::move(sprites)), m_Texture(texture)
		{
		}

		Spritesheet(std::shared_ptr<Texture> texture, int spriteWidth, int spriteHeight, int numSprites, int spacing)
		{
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
			for (int i = 0; i < numSprites; i++)
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
					texture,
					spriteWidth,
					spriteHeight,
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
				if (currentX >= texture->GetWidth())
				{
					currentX = 0;
					currentY -= spriteHeight + spacing;
				}
			}
		}

		Sprite GetSprite(int index)
		{
			return m_Sprites[index];
		}

		int Size()
		{
			return (int)m_Sprites.size();
		}
	};

	struct SpriteRenderer
	{
		glm::vec4 m_Color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
		int m_ZIndex = 0;
		Sprite m_Sprite{};
	};
}
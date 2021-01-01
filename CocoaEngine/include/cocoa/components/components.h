#pragma once
#include "externalLibs.h"

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/renderer/TextureHandle.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	struct Sprite
	{
		TextureHandle m_Texture = TextureHandle::null;
		int m_Width = 32;
		int m_Height = 32;
		glm::vec2 m_TexCoords[4] = {
			{1, 1},
			{1, 0},
			{0, 0},
			{0, 1}
		};
	};

	class Spritesheet
	{
	public:
		TextureHandle m_TextureHandle;
		std::vector<Sprite> m_Sprites;

		Spritesheet(TextureHandle textureHandle, std::vector<Sprite> sprites)
			: m_Sprites(std::move(sprites)), m_TextureHandle(textureHandle) {}

		Spritesheet(TextureHandle textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing)
		{
			m_TextureHandle = textureHandle;
			std::shared_ptr<Texture> texture = textureHandle.Get();

			// NOTE: If you don't reserve the space before hand, when the vector grows it may
			// change the pointers it holds
			m_Sprites.reserve(numSprites);

			const uint8* rawPixels = texture->GetPixelBuffer();

			int bytesPerPixel = texture->BytesPerPixel();
			int area = spriteWidth * spriteHeight * bytesPerPixel;
			// This will be used to house temporary sprite data to calculate bounding boxes for all sprites
			std::unique_ptr<uint8> tmpSubImage(new uint8[area]);

			int currentX = 0;
			int currentY = texture->GetHeight() - spriteHeight;
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
						int offset = (absX + absY * texture->GetWidth()) * bytesPerPixel;
						const uint8* pixel = rawPixels + offset;
						tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 0] = *(pixel + 0); // R
						tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 1] = *(pixel + 1); // G
						tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 2] = *(pixel + 2); // B
						tmpSubImage.get()[(x + y * spriteWidth) * bytesPerPixel + 3] = *(pixel + 3); // A
					}
				}
				AABB boundingBox = Physics2D::GetBoundingBoxForPixels(tmpSubImage.get(), spriteWidth, spriteHeight, texture->BytesPerPixel());

				Sprite sprite{
					textureHandle,
					spriteWidth,
					spriteHeight,
					{
						{rightX, topY},
						{rightX, bottomY},
						{leftX, bottomY},
						{leftX, topY}
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
		glm::vec4 m_Color = glm::vec4(1, 1, 1, 1);
		int m_ZIndex = 0;
		Sprite m_Sprite;
	};
}
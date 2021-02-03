#pragma once
#include "externalLibs.h"

#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/util/Log.h"
#include "cocoa/core/Handle.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	struct Sprite
	{
		Handle<Texture> m_Texture = {};
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
		Handle<Texture> m_TextureHandle;
		std::vector<Sprite> m_Sprites;

		Spritesheet(Handle<Texture> textureHandle, std::vector<Sprite> sprites)
			: m_Sprites(std::move(sprites)), m_TextureHandle(textureHandle) {}

		Spritesheet(Handle<Texture> textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing)
		{
			m_TextureHandle = textureHandle;
			const Texture& texture = AssetManager::GetTexture(textureHandle.m_AssetId);

			// NOTE: If you don't reserve the space before hand, when the vector grows it may
			// change the pointers it holds
			m_Sprites.reserve(numSprites);

			int currentX = 0;
			int currentY = texture.Height - spriteHeight;
			for (int i = 0; i < numSprites; i++)
			{
				float topY = (currentY + spriteHeight) / (float)texture.Height;
				float rightX = (currentX + spriteWidth) / (float)texture.Width;
				float leftX = currentX / (float)texture.Width;
				float bottomY = currentY / (float)texture.Height;

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
				if (currentX >= texture.Width)
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

	struct FontRenderer
	{
		glm::vec4 m_Color = glm::vec4(1, 1, 1, 1);
		int m_ZIndex = 0;
		Handle<Font> m_Font;
		std::string text;
		int fontSize;
	};
}
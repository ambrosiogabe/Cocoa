#include "cocoa/components/Spritesheet.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace NSpritesheet
	{
		Spritesheet CreateSpritesheet(Handle<Texture> textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing)
		{
			Spritesheet res;
			res.TextureHandle = textureHandle;
			const Texture& texture = AssetManager::GetTexture(textureHandle.AssetId);

			// NOTE: If you don't reserve the space before hand, when the vector grows it will
			// change the pointers it holds
			res.Sprites.reserve(numSprites);

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
					{
						{rightX, topY},
						{rightX, bottomY},
						{leftX, bottomY},
						{leftX, topY}
					}
				};
				res.Sprites.push_back(sprite);

				currentX += spriteWidth + spacing;
				if (currentX >= texture.Width)
				{
					currentX = 0;
					currentY -= spriteHeight + spacing;
				}
			}

			return res;
		}

		Sprite GetSprite(const Spritesheet& spritesheet, int index)
		{
			Logger::Assert(index >= 0 && index < spritesheet.Sprites.size(), "Index out of bounds exception.");
			return spritesheet.Sprites[index];
		}

		int Size(const Spritesheet& spritesheet)
		{
			return (int)spritesheet.Sprites.size();
		}
	};
}
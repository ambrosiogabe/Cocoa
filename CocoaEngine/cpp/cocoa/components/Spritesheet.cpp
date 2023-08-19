#include "cocoa/components/Spritesheet.h"
#include "cocoa/core/AssetManager.h"

namespace Cocoa
{
	namespace NSpritesheet
	{
		Spritesheet createSpritesheet(Handle<Texture> textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing)
		{
			Spritesheet res;
			res.textureHandle = textureHandle;
			const Texture& texture = AssetManager::getTexture(textureHandle.assetId);

			// NOTE: If you don't reserve the space before hand, when the vector grows it will
			// change the pointers it holds
			res.sprites.reserve(numSprites);

			int currentX = 0;
			int currentY = texture.height - spriteHeight;
			for (int i = 0; i < numSprites; i++)
			{
				float topY = (currentY + spriteHeight) / (float)texture.height;
				float rightX = (currentX + spriteWidth) / (float)texture.width;
				float leftX = currentX / (float)texture.width;
				float bottomY = currentY / (float)texture.height;

				Sprite sprite{
					textureHandle,
					{
						{rightX, topY},
						{rightX, bottomY},
						{leftX, bottomY},
						{leftX, topY}
					}
				};
				res.sprites.push_back(sprite);

				currentX += spriteWidth + spacing;
				if (currentX >= texture.width)
				{
					currentX = 0;
					currentY -= spriteHeight + spacing;
				}
			}

			return res;
		}

		Sprite getSprite(const Spritesheet& spritesheet, int index)
		{
			Logger::Assert(index >= 0 && index < spritesheet.sprites.size(), "Index out of bounds exception.");
			return spritesheet.sprites[index];
		}

		int size(const Spritesheet& spritesheet)
		{
			return (int)spritesheet.sprites.size();
		}
	};
}
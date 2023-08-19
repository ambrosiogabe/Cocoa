#ifndef COCOA_ENGINE_SPRITESHEET_H
#define COCOA_ENGINE_SPRITESHEET_H
#include "externalLibs.h"
#include "cocoa/components/Sprite.h"

namespace Cocoa
{
	struct Spritesheet
	{
		Handle<Texture> textureHandle;
		std::vector<Sprite> sprites;
	};

	namespace NSpritesheet
	{
		COCOA Spritesheet createSpritesheet(Handle<Texture> textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing);

		COCOA Sprite getSprite(const Spritesheet& spritesheet, int index);

		COCOA int size(const Spritesheet& spritesheet);
	};
}

#endif

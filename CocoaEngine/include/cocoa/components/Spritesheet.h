#pragma once
#include "externalLibs.h"
#include "cocoa/components/Sprite.h"

namespace Cocoa
{
	struct Spritesheet
	{
		Handle<Texture> TextureHandle;
		std::vector<Sprite> Sprites;
	};

	namespace NSpritesheet
	{
		COCOA Spritesheet CreateSpritesheet(Handle<Texture> textureHandle, int spriteWidth, int spriteHeight, int numSprites, int spacing);

		COCOA Sprite GetSprite(const Spritesheet& spritesheet, int index);

		COCOA int Size(const Spritesheet& spritesheet);
	};
}
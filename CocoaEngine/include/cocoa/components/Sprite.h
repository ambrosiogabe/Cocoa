#ifndef COCOA_ENGINE_SPRITE_H
#define COCOA_ENGINE_SPRITE_H
#include "externalLibs.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	struct Sprite
	{
		Handle<Texture> texture = {};
		glm::vec2 texCoords[4] = {
			{1, 1},
			{1, 0},
			{0, 0},
			{0, 1}
		};
	};
}

#endif

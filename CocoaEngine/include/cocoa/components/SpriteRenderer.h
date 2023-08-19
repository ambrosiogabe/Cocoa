#ifndef COCOA_ENGINE_SPRITE_RENDERER_H
#define COCOA_ENGINE_SPRITE_RENDERER_H
#include "externalLibs.h"

#include "cocoa/components/Sprite.h"

namespace Cocoa
{
	struct SpriteRenderer
	{
		glm::vec4 color = glm::vec4(1, 1, 1, 1);
		int zIndex = 0;
		Sprite sprite;
	};
}

#endif

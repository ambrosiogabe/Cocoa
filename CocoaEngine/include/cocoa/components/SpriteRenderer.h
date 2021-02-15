#pragma once
#include "externalLibs.h"

#include "cocoa/components/Sprite.h"

namespace Cocoa
{
	struct SpriteRenderer
	{
		glm::vec4 m_Color = glm::vec4(1, 1, 1, 1);
		int m_ZIndex = 0;
		Sprite m_Sprite;
	};
}
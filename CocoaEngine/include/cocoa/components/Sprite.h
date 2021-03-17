#pragma once
#include "externalLibs.h"
#include "cocoa/renderer/Texture.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	struct Sprite
	{
		Handle<Texture> m_Texture = {};
		glm::vec2 m_TexCoords[4] = {
			{1, 1},
			{1, 0},
			{0, 0},
			{0, 1}
		};
	};
}
#pragma once
#include "externalLibs.h"

#include "cocoa/core/Core.h"
#include "cocoa/util/Log.h"

namespace Cocoa
{
	struct Line2D
	{
		glm::vec3 Color;
		glm::vec2 Verts[4];
		float Stroke;
		int Lifetime;
		bool OnTop;
	};

	namespace NLine2D
	{
		COCOA Line2D Create(glm::vec2 from, glm::vec2 to, glm::vec3 color, float stroke, int lifetime, bool onTop);
	}
}
#ifndef COCOA_ENGINE_LINE_2D_H
#define COCOA_ENGINE_LINE_2D_H
#include "externalLibs.h"

#include "cocoa/core/Core.h"

namespace Cocoa
{
	struct Line2D
	{
		glm::vec3 color;
		glm::vec2 vertices[4];
		float stroke;
		int lifetime;
		bool onTop;
	};

	namespace NLine2D
	{
		COCOA Line2D create(glm::vec2 from, glm::vec2 to, glm::vec3 color, float stroke, int lifetime, bool onTop);
	}
}

#endif

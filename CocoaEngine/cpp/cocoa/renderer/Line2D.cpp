#include "cocoa/renderer/Line2D.h"

namespace Cocoa
{
	namespace NLine2D
	{
		Line2D Create(glm::vec2 from, glm::vec2 to, glm::vec3 color, float stroke, int lifetime, bool onTop)
		{
			Line2D res;
			glm::vec2 line = from - to;
			glm::vec2 normal = glm::vec2(line.y, -line.x);
			normal = glm::normalize(normal);

			float halfStroke = stroke / 2.0f;
			res.Verts[0] = from + (normal * halfStroke);
			res.Verts[1] = from - (normal * halfStroke);
			res.Verts[2] = res.Verts[0] - line;
			res.Verts[3] = res.Verts[1] - line;

			res.OnTop = onTop;
			return res;
		}
	}
}
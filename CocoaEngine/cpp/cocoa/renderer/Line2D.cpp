#include "cocoa/renderer/Line2D.h"
#include "cocoa/util/CMath.h"

namespace Cocoa
{
	namespace NLine2D
	{
		Line2D create(glm::vec2 from, glm::vec2 to, glm::vec3 color, float stroke, int lifetime, bool onTop)
		{
			Line2D res;
			glm::vec2 line = from - to;
			glm::vec2 normal = glm::vec2(line.y, -line.x);
			normal = glm::normalize(normal);

			float halfStroke = stroke / 2.0f;
			res.vertices[0] = to - (normal * halfStroke);
			res.vertices[1] = to + (normal * halfStroke);
			res.vertices[2] = res.vertices[1] + line;
			res.vertices[3] = res.vertices[0] + line;
#if _COCOA_DEBUG
			glm::vec2 testVec = res.vertices[1] - res.vertices[0];
			float testLength = glm::length(testVec);
			if (!CMath::Compare(stroke, testLength, 0.001f))
			{
				Logger::Warning("Invalid result when computing line2D position. Stroke width does not match. Stroke %2.3f, Actual Length: %2.3f", stroke, testLength);
			}
#endif

			res.onTop = onTop;
			res.lifetime = lifetime;
			res.color = color;
			return res;
		}
	}
}
#include "cocoa/renderer/Line2D.h"
#include "cocoa/util/CMath.h"

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
			res.Verts[0] = to - (normal * halfStroke);
			res.Verts[1] = to + (normal * halfStroke);
			res.Verts[2] = res.Verts[1] + line;
			res.Verts[3] = res.Verts[0] + line;
#if _COCOA_DEBUG
			glm::vec2 testVec = res.Verts[1] - res.Verts[0];
			float testLength = glm::length(testVec);
			if (!CMath::Compare(stroke, testLength, 0.001f))
			{
				Logger::Warning("Invalid result when computing line2D position. Stroke width does not match. Stroke %2.3f, Actual Length: %2.3f", stroke, testLength);
			}
#endif

			res.OnTop = onTop;
			res.Lifetime = lifetime;
			res.Color = color;
			return res;
		}
	}
}
#pragma once

#include "jade/renderer/Line2D.h"
#include "jade/physics2d/primitives/Circle.h"
#include "jade/physics2d/primitives/Raycast2D.h"
#include "jade/physics2d/primitives/Box2D.h"
#include "jade/physics2d/primitives/AABB.h"

#include <glm/vec2.hpp>

namespace Jade
{
	namespace CollisionDetector2D
	{
		bool PointOnLine(const glm::vec2& point, const Line2D& line);
		bool PointInCircle(const glm::vec2& point, const Circle& circle);
		bool PointInBox2D(const glm::vec2& point, const Box2D& box);
		bool PointInAABB(const glm::vec2& point, const AABB& box);

		bool LineAndCircle(const Line2D& line, const Circle& circle);
		bool LineAndBox2D(const Line2D& line, const Box2D& box);
		bool LineAndAABB(const Line2D& line, const AABB& box);

		bool RayAndCircle(const Raycast2D& ray, const Circle& circle);
		bool RayAndBox2D(const Raycast2D& ray, const Box2D& box);
		bool RayAndAABB(const Raycast2D& ray, const AABB& box);

		bool CircleAndLine(const Circle& circle, const Line2D& line);
		bool CircleAndRay(const Circle& circle, const Raycast2D& ray);
		bool CircleAndCircle(const Circle& c1, const Circle& c2);
		bool CircleAndBox2D(const Circle& circle, const Box2D& box);
		bool CircleAndAABB(const Circle& circle, const AABB& box);

		bool AABBAndCircle(const AABB& box, const Circle& circle);
		bool AABBAndRay(const AABB& box, const Raycast2D& ray);
		bool AABBAndLine(const AABB& box, const Line2D& line);
		bool AABBAndAABB(const AABB& b1, const AABB& b2);
		bool AABBAndBox2D(const AABB& b1, const Box2D b2);

		bool Box2DAndCircle(const Box2D& box, const Circle& circle);
		bool Box2DAndRay(const Box2D& box, const Raycast2D& ray);
		bool Box2DAndLine(const Box2D& box, const Line2D& line);
		bool Box2DAndAABB(const Box2D& b1, const AABB& b2);
		bool Box2DAndBox2D(const Box2D& b1, const Box2D& b2);
	}
}
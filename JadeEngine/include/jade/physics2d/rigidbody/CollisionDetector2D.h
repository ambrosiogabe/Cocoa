#pragma once
#include "externalLibs.h"

#include "jade/renderer/Line2D.h"
#include "jade/physics2d/Physics2DSystem.h"
#include "jade/components/Transform.h"

namespace Jade
{
	namespace CollisionDetector2D
	{
		JADE bool PointOnLine(const glm::vec2& point, const Line2D& line);
		JADE bool PointInCircle(const glm::vec2& point, const Circle& circle);
		JADE bool PointInBox2D(const glm::vec2& point, const Box2D& box);
		JADE bool PointInAABB(const glm::vec2& point, const AABB& box);
		JADE bool PointOnRay(const glm::vec2& point, const Ray2D& ray);

		JADE bool LineAndCircle(const Line2D& line, const Circle& circle);
		JADE bool LineAndBox2D(const Line2D& line, const Box2D& box);
		JADE bool LineAndAABB(const Line2D& line, const AABB& box);

		JADE bool RayAndCircle(const Ray2D& ray, const Circle& circle);
		JADE bool RayAndBox2D(const Ray2D& ray, const Box2D& box);
		JADE bool RayAndAABB(const Ray2D& ray, const AABB& box);

		JADE bool CircleAndLine(const Circle& circle, const Line2D& line);
		JADE bool CircleAndRay(const Circle& circle, const Ray2D& ray);
		JADE bool CircleAndCircle(const Circle& c1, const Circle& c2);
		JADE bool CircleAndBox2D(const Circle& circle, const Box2D& box);
		JADE bool CircleAndAABB(const Circle& circle, const AABB& box);

		JADE bool AABBAndCircle(const AABB& box, const Circle& circle);
		JADE bool AABBAndRay(const AABB& box, const Ray2D& ray);
		JADE bool AABBAndLine(const AABB& box, const Line2D& line);
		JADE bool AABBAndAABB(const AABB& b1, const AABB& b2);
		JADE bool AABBAndBox2D(const AABB& b1, const Box2D b2);

		JADE bool Box2DAndCircle(const Box2D& box, const Circle& circle);
		JADE bool Box2DAndRay(const Box2D& box, const Ray2D& ray);
		JADE bool Box2DAndLine(const Box2D& box, const Line2D& line);
		JADE bool Box2DAndAABB(const Box2D& b1, const AABB& b2);
		JADE bool Box2DAndBox2D(const Box2D& b1, const Box2D& b2);
	}
}
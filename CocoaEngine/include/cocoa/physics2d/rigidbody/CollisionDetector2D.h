#pragma once
#include "externalLibs.h"

#include "cocoa/renderer/Line2D.h"
#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/components/Transform.h"

namespace Cocoa
{
	namespace CollisionDetector2D
	{
		COCOA bool PointOnLine(const glm::vec2& point, const Line2D& line);
		COCOA bool PointInCircle(const glm::vec2& point, const Circle& circle);
		COCOA bool PointInBox2D(const glm::vec2& point, const Box2D& box);
		COCOA bool PointInAABB(const glm::vec2& point, const AABB& box);
		COCOA bool PointOnRay(const glm::vec2& point, const Ray2D& ray);

		COCOA bool LineAndCircle(const Line2D& line, const Circle& circle);
		COCOA bool LineAndBox2D(const Line2D& line, const Box2D& box);
		COCOA bool LineAndAABB(const Line2D& line, const AABB& box);

		COCOA bool RayAndCircle(const Ray2D& ray, const Circle& circle);
		COCOA bool RayAndBox2D(const Ray2D& ray, const Box2D& box);
		COCOA bool RayAndAABB(const Ray2D& ray, const AABB& box);

		COCOA bool CircleAndLine(const Circle& circle, const Line2D& line);
		COCOA bool CircleAndRay(const Circle& circle, const Ray2D& ray);
		COCOA bool CircleAndCircle(const Circle& c1, const Circle& c2);
		COCOA bool CircleAndBox2D(const Circle& circle, const Box2D& box);
		COCOA bool CircleAndAABB(const Circle& circle, const AABB& box);

		COCOA bool AABBAndCircle(const AABB& box, const Circle& circle);
		COCOA bool AABBAndRay(const AABB& box, const Ray2D& ray);
		COCOA bool AABBAndLine(const AABB& box, const Line2D& line);
		COCOA bool AABBAndAABB(const AABB& b1, const AABB& b2);
		COCOA bool AABBAndBox2D(const AABB& b1, const Box2D b2);

		COCOA bool Box2DAndCircle(const Box2D& box, const Circle& circle);
		COCOA bool Box2DAndRay(const Box2D& box, const Ray2D& ray);
		COCOA bool Box2DAndLine(const Box2D& box, const Line2D& line);
		COCOA bool Box2DAndAABB(const Box2D& b1, const AABB& b2);
		COCOA bool Box2DAndBox2D(const Box2D& b1, const Box2D& b2);
	}
}
#include "externalLibs.h"

#include "jade/physics2d/rigidbody/CollisionDetector2D.h"
#include "jade/components/Transform.h"
#include "jade/util/JMath.h"

namespace Jade
{
	namespace CollisionDetector2D
	{
		using JP2 = Jade::Physics2DSystem;

		// Helper functions
		static glm::vec2 GetInterval(const Box2D& box, const glm::vec2& axis)
		{
			glm::vec2 normAxis = glm::normalize(axis);

			// Get the interval of the box's min and max projected onto the axis provided and 
			// store in result (result.x = min, result.y = max)
			glm::vec2 result{};
			std::array<glm::vec2, 4> vertices = JP2::GetVertices(box);

			// Project each vertex of the box onto the axis, and keep track of the smallest
			// and largest values
			result.x = result.y = glm::dot(axis, vertices[0]);
			for (int i = 1; i < vertices.size(); i++)
			{
				float projection = glm::dot(axis, vertices[i]);
				if (projection < result.x)
				{
					result.x = projection;
				}
				if (projection > result.y)
				{
					result.y = projection;
				}
			}

			return result;
		}

		static float OverlapOnAxis(const Box2D& b1, const Box2D& b2, const glm::vec2& axis, const glm::vec2& toCenter)
		{
			glm::vec2 interval1 = GetInterval(b1, axis);
			glm::vec2 interval2 = GetInterval(b2, axis);

			float proj1 = (interval1.y - interval1.x) / 2.0f;
			float proj2 = (interval2.y - interval2.x) / 2.0f;

			float distance = abs(glm::dot(toCenter, axis));

			return proj1 + proj2 - distance;
		}


		// Implementations
		bool PointOnLine(const glm::vec2& point, const Line2D& line)
		{
			// Find the slope
			glm::vec2 delta = line.GetEnd() - line.GetStart();
			float slope = delta.y / delta.x;

			// Find the y-intercept
			float yIntercept = line.GetStart().y - (slope * line.GetStart().x);

			// Check the line equation
			return JMath::Compare(point.y, slope * point.x + yIntercept);
		}

		bool PointInCircle(const glm::vec2& point, const Circle& circle)
		{
			glm::vec2 line = JP2::GetCenter(circle) - point;

			return glm::length2(line) <= circle.m_Radius * circle.m_Radius;
		}

		bool PointInBox2D(const glm::vec2& point, const Box2D& box)
		{
			// Translate the point into local space, then try to clip the point to the rectangle
			glm::vec2 pointLocalSpace = glm::vec2(point);
			JMath::Rotate(pointLocalSpace, JP2::GetRotation(box), JP2::GetCenter(box));

			glm::vec2 min = JP2::GetMin(box);
			glm::vec2 max = JP2::GetMax(box);

			return min.x <= pointLocalSpace.x && min.y <= pointLocalSpace.y &&
				pointLocalSpace.x <= max.x && pointLocalSpace.y <= max.y;
		}

		bool PointInAABB(const glm::vec2& point, const AABB& box)
		{
			glm::vec2 min = JP2::GetMin(box);
			glm::vec2 max = JP2::GetMax(box);

			return min.x <= point.x && min.y <= point.y && point.x <= max.x && point.y <= max.y;
		}

		bool PointOnRay(const glm::vec2& point, const Ray2D& ray)
		{
			return false;
		}


		bool LineAndCircle(const Line2D& line, const Circle& circle)
		{
			if (PointInCircle(line.GetStart(), circle) || PointInCircle(line.GetEnd(), circle))
			{
				return true;
			}

			glm::vec2 ab = line.GetEnd() - line.GetStart();

			// Project point (circle position) onto ab (line segment), computing the parameterized
			// position d(t) = a + t * (b - a)
			float t = glm::dot(JP2::GetCenter(circle) - line.GetStart(), ab) / glm::dot(ab, ab);

			// Clamp T to a 0-1 range. If t was < 0 or > 1
			// then the closest point was outside the segment
			if (t < 0.0f || t > 1.0f)
			{
				return false;
			}

			// Find teh closest point on the line segment
			glm::vec2 closestPoint = line.GetStart() + (ab * t);

			glm::vec2 circleToClosest = JP2::GetCenter(circle) - closestPoint;
			return glm::length2(circleToClosest) < circle.m_Radius * circle.m_Radius;
		}

		bool LineAndBox2D(const Line2D& line, const Box2D& box)
		{
			float theta = -JP2::GetRotation(box);
			glm::vec2 boxCenter = JP2::GetCenter(box);
			glm::vec2 localStart = line.GetStart();
			JMath::Rotate(localStart, theta, boxCenter);
			glm::vec2 localEnd = line.GetEnd();
			JMath::Rotate(localEnd, theta, boxCenter);
			Line2D localLine { localStart, localEnd };
			AABB localBox { JP2::GetMin(box), JP2::GetMin(box) };

			return LineAndAABB(localLine, localBox);
		}

		bool LineAndAABB(const Line2D& line, const AABB& box)
		{
			glm::vec2 normal = line.GetEnd() - line.GetStart();
			glm::normalize(normal);
			normal.x = (normal.x != 0.0f) ? 1.0f / normal.x : 0.0f;
			normal.y = (normal.y != 0.0f) ? 1.0f / normal.y : 0.0f;

			glm::vec2 min = JP2::GetMin(box) - (line.GetStart() * normal);
			glm::vec2 max = JP2::GetMax(box) - (line.GetStart() * normal);

			float tmin = std::max(std::min(min.x, max.x), std::min(min.y, max.y));
			float tmax = std::min(std::max(min.x, max.x), std::max(min.y, max.y));
			if (tmax < 0 || tmin > tmax)
			{
				return false;
			}
			float t = (tmin < 0.0f) ? tmax : tmin;
			return t > 0.0f && t * t < glm::length2(line.GetEnd() - line.GetStart());
		}


		bool RayAndCircle(const Ray2D& ray, const Circle& circle)
		{
			// TODO: IMPLEMENT ME!!
			return false;
		}

		bool RayAndBox2D(const Ray2D& ray, const Box2D& box)
		{
			// TODO: IMPLEMENT ME!!
			return false;
		}

		bool RayAndAABB(const Ray2D& ray, const AABB& box)
		{
			glm::vec2 min = JP2::GetMin(box);
			glm::vec2 max = JP2::GetMax(box);
			glm::vec2 origin = ray.m_Origin;
			glm::vec2 dir = ray.m_Direction;
			float maxDistance = ray.m_MaxDistance;

			float tMinX = (min.x - origin.x) / (JMath::Compare(dir.x, 0) ? 0.00001f : dir.x);
			float tMaxX = (max.x - origin.x) / (JMath::Compare(dir.x, 0) ? 0.00001f : dir.x);
			float tMinY = (min.y - origin.x) / (JMath::Compare(dir.y, 0) ? 0.00001f : dir.x);
			float tMaxY = (max.y - origin.x) / (JMath::Compare(dir.y, 0) ? 0.00001f : dir.x);

			float tMin = std::max(std::min(tMinX, tMaxX), std::min(tMinY, tMaxY));
			float tMax = std::min(std::max(tMinX, tMaxX), std::max(tMinY, tMaxY));

			if (tMax < 0)
			{
				return false;
			}
			else if (tMin > maxDistance)
			{
				return false;
			}
			else if (tMin > maxDistance)
			{
				return false;
			}

			return true;
		}


		bool CircleAndLine(const Circle& circle, const Line2D& line)
		{
			return LineAndCircle(line, circle);
		}

		bool CircleAndRay(const Circle& circle, const Ray2D& ray)
		{
			return RayAndCircle(ray, circle);
		}

		bool CircleAndCircle(const Circle& c1, const Circle& c2)
		{
			glm::vec2 lineBetweenCenters = JP2::GetCenter(c1) - JP2::GetCenter(c2);
			float radiiSum = c1.m_Radius + c2.m_Radius;
			return glm::length2(lineBetweenCenters) <= radiiSum * radiiSum;
		}

		bool CircleAndBox2D(const Circle& circle, const Box2D& box)
		{
			// Treat the box as if it was located at halfSize.x, halfSize.y
			glm::vec2 min{};
			glm::vec2 max = box.m_HalfSize * 2.0f;

			// Create a circle in box's local space
			glm::vec2 r = JP2::GetCenter(circle) - JP2::GetCenter(box);
			JMath::Rotate(r, -JP2::GetRotation(box), glm::vec2{});
			glm::vec2 localCirclePos = r + box.m_HalfSize;

			glm::vec2 closestPointToCircle = localCirclePos;
			if (closestPointToCircle.x < min.x)
			{
				closestPointToCircle.x = min.x;
			}
			else if (closestPointToCircle.x > max.x)
			{
				closestPointToCircle.x = max.x;
			}

			if (closestPointToCircle.y < min.y)
			{
				closestPointToCircle.y = min.y;
			}
			else if (closestPointToCircle.y > max.y)
			{
				closestPointToCircle.y = max.y;
			}

			glm::vec2 line = localCirclePos - closestPointToCircle;
			return glm::length2(line) <= circle.m_Radius * circle.m_Radius;
		}

		bool CircleAndAABB(const Circle& circle, const AABB& box)
		{
			// Treat the box as if it was located at halfSize.x, halfSize.y
			glm::vec2 min{};
			glm::vec2 max = box.m_HalfSize * 2.0f;

			// Create a circle in box's local space
			glm::vec2 localCirclePos = JP2::GetCenter(circle) - JP2::GetCenter(box);

			glm::vec2 closestPointToCircle = localCirclePos;
			if (closestPointToCircle.x < min.x)
			{
				closestPointToCircle.x = min.x;
			}
			else if (closestPointToCircle.x > max.x)
			{
				closestPointToCircle.x = max.x;
			}

			if (closestPointToCircle.y < min.y)
			{
				closestPointToCircle.y = min.y;
			}
			else if (closestPointToCircle.y > max.y)
			{
				closestPointToCircle.y = max.y;
			}

			glm::vec2 line = localCirclePos - closestPointToCircle;
			return glm::length2(line) <= circle.m_Radius * circle.m_Radius;
		}


		bool AABBAndCircle(const AABB& box, const Circle& circle)
		{
			return CircleAndAABB(circle, box);
		}

		bool AABBAndRay(const AABB& box, const Ray2D& ray)
		{
			return RayAndAABB(ray, box);
		}

		bool AABBAndLine(const AABB& box, const Line2D& line)
		{
			return LineAndAABB(line, box);
		}

		bool AABBAndAABB(const AABB& b1, const AABB& b2)
		{
			// TODO: IMPLEMENT ME
			return false;
		}

		bool AABBAndBox2D(const AABB& b1, const Box2D b2)
		{
			// TODO: IMPLEMENT ME
			return false;
		}


		bool Box2DAndCircle(const Box2D& box, const Circle& circle)
		{
			return CircleAndBox2D(circle, box);
		}

		bool Box2DAndRay(const Box2D& box, const Ray2D& ray)
		{
			return RayAndBox2D(ray, box);
		}

		bool Box2DAndLine(const Box2D& box, const Line2D& line)
		{
			return LineAndBox2D(line, box);
		}

		bool Box2DAndAABB(const Box2D& b1, const AABB& b2)
		{
			// TODO: IMPLEMENT ME
			return false;
		}

		bool Box2DAndBox2D(const Box2D& b1, const Box2D& b2)
		{
			// Generate all the axes to test from box one and box two
			glm::vec2 boxOneUp{ 0, b1.m_HalfSize.y };
			glm::vec2 boxOneRight{ b1.m_HalfSize.x, 0 };
			JMath::Rotate(boxOneUp, JP2::GetRotation(b1), glm::vec2{});
			JMath::Rotate(boxOneRight, JP2::GetRotation(b1), glm::vec2{});

			glm::vec2 boxTwoUp{ 0, b2.m_HalfSize.y };
			glm::vec2 boxTwoRight{ b2.m_HalfSize.x, 0 };
			JMath::Rotate(boxTwoUp, JP2::GetRotation(b2), glm::vec2{});
			JMath::Rotate(boxTwoRight, JP2::GetRotation(b2), glm::vec2{});

			// Test whether the boxes are intersecting on all axes (including global up and right axes)
			std::array<glm::vec2, 4> axisToTest { boxOneUp, boxOneRight, boxTwoUp, boxTwoRight };
			glm::vec2 toCenter = JP2::GetCenter(b2) - JP2::GetCenter(b1);
			int axis = -1;
			float overlap = std::numeric_limits<float>::max();
			for (int i = 0; i < axisToTest.size(); i++)
			{
				// Intervals don't overlap separating axis found
				float thisOverlap = OverlapOnAxis(b1, b2, axisToTest[i], toCenter);
				if (thisOverlap < 0)
				{
					return false;
				}
				else
				{
					if (thisOverlap < overlap)
					{
						overlap = thisOverlap;
						axis = i;
					}
				}
			}

			// All intervals overlapped, no separating axis found
			return true;
		}
	}
}
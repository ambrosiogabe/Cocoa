#include "jade/physics2d/Physics2D.h"
#include "jade/components/components.h"
#include "jade/components/Transform.h"
#include "jade/physics2d/rigidbody/CollisionDetector2D.h"

#include "jade/core/Application.h"

namespace Jade
{
	AABB Physics2D::GetBoundingBoxForPixels(uint8* pixels, int width, int height, int channels)
	{
		AABB result = Physics2DSystem::AABBFrom(glm::vec2{ 0, 0 }, glm::vec2{ width, height }, glm::vec2{ 0, 0 });

		if (channels == 4)
		{
			// Initialize minimum to the texture size, and max to 0
			int minX = width, minY = height;
			int maxX = 0, maxY = 0;

			uint8* pixelOffset = pixels;
			for (int y = 0; y < height; y++)
			{
				for (int x = 0; x < width; x++)
				{
					uint8 alpha = pixelOffset[3];
					// If we find non-transparent pixel, update the box bounds if required
					if (alpha > 10)
					{
						if (minX > x) minX = x;
						if (maxX < x) maxX = x;

						if (minY > y) minY = y;
						if (maxY < y) maxY = y;
					}

					pixelOffset += channels;
				}
			}

			// Prevent off by one error
			maxX++;
			maxY++;

			glm::vec2 center = { width / 2.0f, height / 2.0f };
			glm::vec2 subCenter = { maxX - ((maxX - minX) / 2.0f), maxY - ((maxY - minY) / 2.0f) };

			glm::vec2 offset = subCenter - center;
			offset.y *= -1;
			result = Physics2DSystem::AABBFrom(glm::vec2{minX, minY}, glm::vec2{maxX, maxY}, offset);
		}

		return result;
	}

	std::pair<entt::registry&, entt::entity> Physics2D::OverlapPoint(const glm::vec2& point)
	{
		Physics2D* physics = Get();
		auto group = physics->m_Registry.group<AABB>(entt::get<Transform>);
		for (auto entity : group) 
		{
			auto& aabb = group.get<AABB>(entity);
			auto& transform = group.get<Transform>(entity);
			if (CollisionDetector2D::PointInAABB(point, aabb))
			{
				return { physics->m_Registry, entity };
			}
		};

		return { physics->m_Registry, entt::null };
	}

	// -------------------------------------------------------------------------
	// Should be called once at beginning of engine initialization
	// -------------------------------------------------------------------------
	std::unique_ptr<Physics2D> Physics2D::s_Instance = nullptr;
	void Physics2D::Init(entt::registry& registry)
	{
		if (s_Instance == nullptr)
		{
			s_Instance = std::unique_ptr<Physics2D>(new Physics2D(registry));
		}
		else
		{
			Log::Error("Tried to reinitialize Physics2D.");
		}
	}

	Physics2D* Physics2D::Get()
	{
		Log::Assert(s_Instance != nullptr, "Physics2D was never initialized.");

		return s_Instance.get();
	}
}
#pragma once
#include "externalLibs.h"

#include "jade/physics2d/Physics2DSystem.h"
#include "jade/core/Core.h"
#include "jade/core/Entity.h"

namespace Jade
{
	class Scene;
	class JADE Physics2D
	{
	public:
		Physics2D(Scene* scene);

		static AABB GetBoundingBoxForPixels(uint8* pixels, int width, int height, int channels);

		static Entity OverlapPoint(const glm::vec2& point);
		static bool PointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees);

		static Physics2D* Get();
		static void Init();
		static void SetScene(Scene* scene);

		void AddEntity(Entity entity);
		void Update(float dt);
		void Destroy();

	private:
		Scene* m_Scene;

		// Box2D Stuff
		b2Vec2 m_Gravity { 0.0f, -10.0f };
		b2World m_World { m_Gravity };
		float m_PhysicsTime = 0.0f;

		static std::unique_ptr<Physics2D> s_Instance;
	};
}
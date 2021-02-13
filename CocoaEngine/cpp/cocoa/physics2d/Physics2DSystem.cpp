#include "cocoa/physics2d/Physics2DSystem.h"
#include "cocoa/components/components.h"
#include "cocoa/components/Transform.h"
#include "cocoa/core/Application.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	namespace Physics2DSystem
	{
		// Internal Variables
		static b2Vec2 m_Gravity;
		static b2World* m_World;
		static float m_PhysicsTime;

		void Init(const glm::vec2& gravity)
		{
			m_Gravity = { gravity.x, gravity.y };
			m_World = new b2World{ m_Gravity };
			m_PhysicsTime = 0.0f;
		}

		void Destroy(SceneData& scene)
		{
			auto view = scene.Registry.view<Rigidbody2D>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = { rawEntity, &scene };
				Rigidbody2D& rb = NEntity::GetComponent<Rigidbody2D>(entity);

				// Manually destroy all bodies, in case the physics system would like
				// to use this world again
				m_World->DestroyBody(static_cast<b2Body*>(rb.m_RawRigidbody));
				rb.m_RawRigidbody = nullptr;
			}
			delete m_World;
		}

		bool PointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees)
		{
			b2PolygonShape shape;
			shape.SetAsBox(halfSize.x, halfSize.y);
			b2Transform transform = b2Transform(b2Vec2(position.x, position.y), b2Rot(CMath::ToRadians(rotationDegrees)));
			return shape.TestPoint(transform, b2Vec2(point.x, point.y));
		}

		void AddEntity(Entity entity)
		{
			if (NEntity::HasComponent<TransformData, Rigidbody2D>(entity))
			{
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Rigidbody2D& rb = NEntity::GetComponent<Rigidbody2D>(entity);

				b2BodyDef bodyDef;
				bodyDef.position.Set(transform.Position.x, transform.Position.y);
				bodyDef.angle = CMath::ToRadians(transform.EulerRotation.z);
				bodyDef.angularDamping = rb.m_AngularDamping;
				bodyDef.linearDamping = rb.m_LinearDamping;
				bodyDef.fixedRotation = rb.m_FixedRotation;
				bodyDef.bullet = rb.m_ContinuousCollision;

				if (rb.m_BodyType == BodyType2D::Dynamic)
				{
					bodyDef.type = b2BodyType::b2_dynamicBody;
				}
				else if (rb.m_BodyType == BodyType2D::Static)
				{
					bodyDef.type = b2BodyType::b2_staticBody;
				}
				else
				{
					bodyDef.type = b2BodyType::b2_kinematicBody;
				}

				b2Body* body = m_World->CreateBody(&bodyDef);
				rb.m_RawRigidbody = body;

				b2PolygonShape shape;
				if (NEntity::HasComponent<Box2D>(entity))
				{
					Box2D& box = NEntity::GetComponent<Box2D>(entity);
					shape.SetAsBox(box.m_HalfSize.x * transform.Scale.x, box.m_HalfSize.y * transform.Scale.y);
					b2Vec2 pos = bodyDef.position;
					bodyDef.position.Set(pos.x - box.m_HalfSize.x * transform.Scale.x, pos.y - box.m_HalfSize.y * transform.Scale.y);
				}
				else if (NEntity::HasComponent<Circle>(entity))
				{
					// TODO: IMPLEMENT ME
					Circle& circle = NEntity::GetComponent<Circle>(entity);
				}

				body->CreateFixture(&shape, rb.m_Mass);
			}
		}

		void Update(SceneData& scene, float dt)
		{
			m_PhysicsTime += dt;
			while (m_PhysicsTime > 0.0f)
			{
				m_World->Step(Settings::Physics2D::s_Timestep, Settings::Physics2D::s_VelocityIterations, Settings::Physics2D::s_PositionIterations);
				m_PhysicsTime -= Settings::Physics2D::s_Timestep;
			}

			auto view = scene.Registry.view<TransformData, Rigidbody2D>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = { rawEntity, &scene };
				TransformData& transform = NEntity::GetComponent<TransformData>(entity);
				Rigidbody2D& rb = NEntity::GetComponent<Rigidbody2D>(entity);
				b2Body* body = static_cast<b2Body*>(rb.m_RawRigidbody);
				b2Vec2 position = body->GetPosition();
				transform.Position.x = position.x;
				transform.Position.y = position.y;
				transform.EulerRotation.z = CMath::ToDegrees(body->GetAngle());
			}
		}

		void Serialize(json& j, Entity entity, const AABB& box)
		{
			json halfSize = CMath::Serialize("HalfSize", box.m_HalfSize);
			json offset = CMath::Serialize("Offset", box.m_Offset);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"AABB", {
					{"Entity", NEntity::GetID(entity)},
					halfSize,
					offset
				}}
			};
		}

		void DeserializeAABB(json& j, Entity entity)
		{
			AABB box;
			box.m_HalfSize = CMath::DeserializeVec2(j["AABB"]["HalfSize"]);
			box.m_Size = box.m_HalfSize * 2.0f;
			box.m_Offset = CMath::DeserializeVec2(j["AABB"]["Offset"]);
			NEntity::AddComponent<AABB>(entity, box);
		}

		void Serialize(json& j, Entity entity, const Box2D& box)
		{
			json halfSize = CMath::Serialize("HalfSize", box.m_HalfSize);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Box2D", {
					{"Entity", NEntity::GetID(entity)},
					halfSize,
				}}
			};
		}

		void DeserializeBox2D(json& j, Entity entity)
		{
			Box2D box;
			box.m_HalfSize = CMath::DeserializeVec2(j["Box2D"]["HalfSize"]);
			box.m_Size = box.m_HalfSize * 2.0f;
			NEntity::AddComponent<Box2D>(entity, box);
		}

		void Serialize(json& j, Entity entity, const Rigidbody2D& rb)
		{
			json angularDamping = { "AngularDamping", rb.m_AngularDamping };
			json linearDamping = { "LinearDamping", rb.m_LinearDamping };
			json mass = { "Mass", rb.m_Mass };
			json velocity = CMath::Serialize("Velocity", rb.m_Velocity);
			json continousCollision = { "ContinousCollision", rb.m_ContinuousCollision };
			json fixedRotation = { "FixedRotation", rb.m_FixedRotation };
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Rigidbody2D", {
					{"Entity", NEntity::GetID(entity)},
					angularDamping,
					linearDamping,
					mass,
					velocity,
					continousCollision,
					fixedRotation
				}}
			};
		}

		void DeserializeRigidbody2D(json& j, Entity entity)
		{
			Rigidbody2D rb;
			rb.m_AngularDamping = j["Rigidbody2D"]["AngularDamping"];
			rb.m_LinearDamping = j["Rigidbody2D"]["LinearDamping"];
			rb.m_Mass = j["Rigidbody2D"]["Mass"];
			rb.m_Velocity = CMath::DeserializeVec2(j["Rigidbody2D"]["Velocity"]);
			rb.m_ContinuousCollision = j["Rigidbody2D"]["ContinousCollision"];
			rb.m_FixedRotation = j["Rigidbody2D"]["FixedRotation"];
			NEntity::AddComponent<Rigidbody2D>(entity, rb);
		}
	}
}
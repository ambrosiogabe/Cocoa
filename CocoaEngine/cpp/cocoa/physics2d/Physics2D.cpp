#include "cocoa/physics2d/Physics2D.h"
#include "cocoa/physics2d/ContactListener.h"
#include "cocoa/components/Transform.h"
#include "cocoa/core/Application.h"
#include "cocoa/util/CMath.h"
#include "cocoa/util/Settings.h"

namespace Cocoa
{
	namespace Physics2D
	{
		// Internal Variables
		static b2Vec2 m_Gravity;
		static b2World* m_World = nullptr;
		static ContactListener contactListener;
		static float m_PhysicsTime;

		void init(const glm::vec2& gravity)
		{
			m_Gravity = { gravity.x, gravity.y };
			m_World = new b2World{ m_Gravity };
			m_World->SetContactListener(&contactListener);
			m_PhysicsTime = 0.0f;
		}

		void destroy(SceneData& scene)
		{
			auto view = scene.registry.view<Rigidbody2D>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = { rawEntity };
				deleteEntity(entity);
			}

			if (m_World)
			{
				delete m_World;
				m_World = nullptr;
			}
		}

		bool pointInBox(const glm::vec2& point, const glm::vec2& halfSize, const glm::vec2& position, float rotationDegrees)
		{
			b2PolygonShape shape;
			shape.SetAsBox(halfSize.x, halfSize.y);
			b2Transform transform = b2Transform(b2Vec2(position.x, position.y), b2Rot(CMath::toRadians(rotationDegrees)));
			return shape.TestPoint(transform, b2Vec2(point.x, point.y));
		}

		void addEntity(Entity entity)
		{
			if (NEntity::hasComponent<TransformData, Rigidbody2D>(entity))
			{
				TransformData& transform = NEntity::getComponent<TransformData>(entity);
				Rigidbody2D& rb = NEntity::getComponent<Rigidbody2D>(entity);

				b2BodyDef bodyDef;
				bodyDef.position.Set(transform.position.x, transform.position.y);
				bodyDef.angle = CMath::toRadians(transform.eulerRotation.z);
				bodyDef.angularDamping = rb.angularDamping;
				bodyDef.linearDamping = rb.linearDamping;
				bodyDef.fixedRotation = rb.fixedRotation;
				bodyDef.bullet = rb.continuousCollision;

				if (rb.bodyType == BodyType2D::Dynamic)
				{
					bodyDef.type = b2BodyType::b2_dynamicBody;
				}
				else if (rb.bodyType == BodyType2D::Static)
				{
					bodyDef.type = b2BodyType::b2_staticBody;
				}
				else
				{
					bodyDef.type = b2BodyType::b2_kinematicBody;
				}

				entt::entity* staticRef = (entt::entity*)AllocMem(sizeof(entity.handle));
				*staticRef = entity.handle;
				bodyDef.userData = (void*)staticRef;

				b2Body* body = m_World->CreateBody(&bodyDef);
				rb.rawRigidbody = body;

				b2PolygonShape shape;
				if (NEntity::hasComponent<Box2D>(entity))
				{
					Box2D& box = NEntity::getComponent<Box2D>(entity);
					shape.SetAsBox(box.halfSize.x * transform.scale.x, box.halfSize.y * transform.scale.y);
					b2Vec2 pos = bodyDef.position;
					bodyDef.position.Set(pos.x - box.halfSize.x * transform.scale.x, pos.y - box.halfSize.y * transform.scale.y);
				}
				else if (NEntity::hasComponent<Circle>(entity))
				{
					// TODO: IMPLEMENT ME
					Circle& circle = NEntity::getComponent<Circle>(entity);
				}

				body->CreateFixture(&shape, rb.mass);
			}
		}

		void deleteEntity(Entity entity)
		{
			if (NEntity::hasComponent<Rigidbody2D>(entity))
			{
				Rigidbody2D& rb = NEntity::getComponent<Rigidbody2D>(entity);

				if (rb.rawRigidbody)
				{
					// Manually destroy all bodies, in case the physics system would like
					// to use this world again
					b2Body* body = static_cast<b2Body*>(rb.rawRigidbody);
					void* userData = body->GetUserData();
					if (userData)
					{
						FreeMem(userData);
						body->SetUserData(nullptr);
					}
					m_World->DestroyBody(static_cast<b2Body*>(rb.rawRigidbody));
					rb.rawRigidbody = nullptr;
				}
				else
				{
					Logger::Warning("Removed entity from physics engine that wasn't registered.");
				}
			}
		}

		void update(SceneData& scene, float dt)
		{
			m_PhysicsTime += dt;
			while (m_PhysicsTime > 0.0f)
			{
				m_World->Step(Settings::Physics2D::timestep, Settings::Physics2D::velocityIterations, Settings::Physics2D::positionIterations);
				m_PhysicsTime -= Settings::Physics2D::timestep;
			}

			auto view = scene.registry.view<TransformData, Rigidbody2D>();
			for (entt::entity rawEntity : view)
			{
				Entity entity = { rawEntity };
				TransformData& transform = NEntity::getComponent<TransformData>(entity);
				Rigidbody2D& rb = NEntity::getComponent<Rigidbody2D>(entity);
				b2Body* body = static_cast<b2Body*>(rb.rawRigidbody);
				b2Vec2 position = body->GetPosition();
				transform.position.x = position.x;
				transform.position.y = position.y;
				transform.eulerRotation.z = CMath::toDegrees(body->GetAngle());
			}
		}

		void applyForceToCenter(const Rigidbody2D& rigidbody, const glm::vec2& force)
		{
			b2Body* rawBody = static_cast<b2Body*>(rigidbody.rawRigidbody);
			Logger::Assert(rawBody != nullptr, "Invalid rigidbody. Cannot apply force to center.");
			rawBody->ApplyForceToCenter(b2Vec2(force.x, force.y), true);
		}

		void applyForce(const Rigidbody2D& rigidbody, const glm::vec2& force, const glm::vec2& point)
		{
			b2Body* rawBody = static_cast<b2Body*>(rigidbody.rawRigidbody);
			Logger::Assert(rawBody != nullptr, "Invalid rigidbody. Cannot apply force.");
			rawBody->ApplyForce(b2Vec2(force.x, force.y), b2Vec2(point.x, point.y), true);
		}

		void applyLinearImpulseToCenter(const Rigidbody2D& rigidbody, const glm::vec2& impulse)
		{
			b2Body* rawBody = static_cast<b2Body*>(rigidbody.rawRigidbody);
			Logger::Assert(rawBody != nullptr, "Invalid rigidbody. Cannot apply impulse to center.");
			rawBody->ApplyLinearImpulseToCenter(b2Vec2(impulse.x, impulse.y), true);
		}

		void applyLinearImpulse(const Rigidbody2D& rigidbody, const glm::vec2& impulse, const glm::vec2& point)
		{
			b2Body* rawBody = static_cast<b2Body*>(rigidbody.rawRigidbody);
			Logger::Assert(rawBody != nullptr, "Invalid rigidbody. Cannot apply force.");
			rawBody->ApplyLinearImpulse(b2Vec2(impulse.x, impulse.y), b2Vec2(point.x, point.y), true);
		}

		void serialize(json& j, Entity entity, const AABB& box)
		{
			json halfSize = CMath::serialize("HalfSize", box.halfSize);
			json offset = CMath::serialize("Offset", box.offset);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"AABB", {
					{"Entity", NEntity::getId(entity)},
					halfSize,
					offset
				}}
			};
		}

		void deserializeAabb(const json& j, Entity entity)
		{
			AABB box;
			box.halfSize = CMath::deserializeVec2(j["AABB"]["HalfSize"]);
			box.size = box.halfSize * 2.0f;
			box.offset = CMath::deserializeVec2(j["AABB"]["Offset"]);
			NEntity::addComponent<AABB>(entity, box);
		}

		void serialize(json& j, Entity entity, const Box2D& box)
		{
			json halfSize = CMath::serialize("HalfSize", box.halfSize);
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Box2D", {
					{"Entity", NEntity::getId(entity)},
					halfSize,
				}}
			};
		}

		void deserializeBox2D(const json& j, Entity entity)
		{
			Box2D box;
			box.halfSize = CMath::deserializeVec2(j["Box2D"]["HalfSize"]);
			box.size = box.halfSize * 2.0f;
			NEntity::addComponent<Box2D>(entity, box);
		}

		void serialize(json& j, Entity entity, const Rigidbody2D& rb)
		{
			json angularDamping = { "AngularDamping", rb.angularDamping };
			json linearDamping = { "LinearDamping", rb.linearDamping };
			json mass = { "Mass", rb.mass };
			json velocity = CMath::serialize("Velocity", rb.velocity);
			json continousCollision = { "ContinousCollision", rb.continuousCollision };
			json fixedRotation = { "FixedRotation", rb.fixedRotation };
			int size = j["Components"].size();
			j["Components"][size] = {
				{"Rigidbody2D", {
					{"Entity", NEntity::getId(entity)},
					angularDamping,
					linearDamping,
					mass,
					velocity,
					continousCollision,
					fixedRotation
				}}
			};
		}

		void deserializeRigidbody2D(const json& j, Entity entity)
		{
			Rigidbody2D rb;
			rb.angularDamping = j["Rigidbody2D"]["AngularDamping"];
			rb.linearDamping = j["Rigidbody2D"]["LinearDamping"];
			rb.mass = j["Rigidbody2D"]["Mass"];
			rb.velocity = CMath::deserializeVec2(j["Rigidbody2D"]["Velocity"]);
			rb.continuousCollision = j["Rigidbody2D"]["ContinousCollision"];
			rb.fixedRotation = j["Rigidbody2D"]["FixedRotation"];
			NEntity::addComponent<Rigidbody2D>(entity, rb);
		}
	}
}
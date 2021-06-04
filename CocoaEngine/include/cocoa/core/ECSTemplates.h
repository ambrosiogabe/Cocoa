#pragma once
#include "cocoa/core/ECS.h"

#include "cocoa/components/TransformStruct.h"
#include "cocoa/components/Tag.h"
#include "cocoa/components/SpriteRenderer.h"
#include "cocoa/components/FontRenderer.h"
#include "cocoa/physics2d/PhysicsComponents.h"

namespace Cocoa
{
	namespace ECSInternal
	{
		template<typename T>
		T DefaultConstructor()
		{
			return T{};
		}

		// ----------------------------------------------------------------------------------------------------------------------
		// Default Ctors Template specializations
		template<>
		TransformData DefaultConstructor<TransformData>()
		{
			return Transform::CreateTransform();
		}

		template<>
		Tag DefaultConstructor<Tag>()
		{
			return NTag::createTag("Default");
		}
	}

	namespace ECS
	{
		template<typename T>
		T& GetComponent(EntityEx entity)
		{
			Log::Assert(false, "Cannot get unknown component type '%s'. Add template specialization.", typeid(T).name());
			return T{};
		}

		template<typename T>
		void AddComponent(EntityEx entity, T& component = ECSInternal::DefaultConstructor<T>())
		{
			Log::Assert(false, "Cannot add unknown component type '%s'. Add template specialization", typeid(T).name());
		}

		// ----------------------------------------------------------------------------------------------------------------------
		// GetComponent Template specializations
		template<>
		TransformData& GetComponent<TransformData>(EntityEx entity)
		{
			return *(TransformData*)ECSInternal::GetComponentInternal(EngineComponent::_Transform, entity);
		}

		template<>
		Tag& GetComponent<Tag>(EntityEx entity)
		{
			return *(Tag*)ECSInternal::GetComponentInternal(EngineComponent::_Tag, entity);
		}

		template<>
		SpriteRenderer& GetComponent<SpriteRenderer>(EntityEx entity)
		{
			return *(SpriteRenderer*)ECSInternal::GetComponentInternal(EngineComponent::_SpriteRenderer, entity);
		}

		template<>
		FontRenderer& GetComponent<FontRenderer>(EntityEx entity)
		{
			return *(FontRenderer*)ECSInternal::GetComponentInternal(EngineComponent::_FontRenderer, entity);
		}

		template<>
		Box2D& GetComponent<Box2D>(EntityEx entity)
		{
			return *(Box2D*)ECSInternal::GetComponentInternal(EngineComponent::_Box2D, entity);
		}

		template<>
		AABB& GetComponent<AABB>(EntityEx entity)
		{
			return *(AABB*)ECSInternal::GetComponentInternal(EngineComponent::_AABB, entity);
		}

		template<>
		Circle& GetComponent<Circle>(EntityEx entity)
		{
			return *(Circle*)ECSInternal::GetComponentInternal(EngineComponent::_Circle, entity);
		}

		template<>
		Rigidbody2D& GetComponent<Rigidbody2D>(EntityEx entity)
		{
			return *(Rigidbody2D*)ECSInternal::GetComponentInternal(EngineComponent::_Rigidbody2D, entity);
		}

		// ----------------------------------------------------------------------------------------------------------------------
		// AddComponent Template specializations
		template<>
		void AddComponent<TransformData>(EntityEx entity, TransformData& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_Transform, entity, (void*)&component);
		}

		template<>
		void AddComponent<Tag>(EntityEx entity, Tag& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_Tag, entity, (void*)&component);
		}

		template<>
		void AddComponent<SpriteRenderer>(EntityEx entity, SpriteRenderer& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_SpriteRenderer, entity, (void*)&component);
		}

		template<>
		void AddComponent<FontRenderer>(EntityEx entity, FontRenderer& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_FontRenderer, entity, (void*)&component);
		}

		template<>
		void AddComponent<Box2D>(EntityEx entity, Box2D& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_Box2D, entity, (void*)&component);
		}

		template<>
		void AddComponent<AABB>(EntityEx entity, AABB& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_AABB, entity, (void*)&component);
		}

		template<>
		void AddComponent<Circle>(EntityEx entity, Circle& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_Circle, entity, (void*)&component);
		}

		template<>
		void AddComponent<Rigidbody2D>(EntityEx entity, Rigidbody2D& component)
		{
			ECSInternal::AddComponentInternal(EngineComponent::_Rigidbody2D, entity, (void*)&component);
		}
	}
}
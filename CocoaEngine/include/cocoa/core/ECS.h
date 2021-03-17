//#pragma once
//#include "cocoa/core/Core.h"
//
//namespace Cocoa
//{
//	enum EngineComponent
//	{
//		_Transform,
//		_Tag,
//		_SpriteRenderer,
//		_FontRenderer,
//		_Box2D,
//		_AABB,
//		_Circle,
//		_Rigidbody2D,
//		_Size
//	};
//
//	struct EntityEx
//	{
//		int Handle;
//	};
//
//	namespace ECSInternal
//	{
//		COCOA void AddComponentInternal(EngineComponent componentType, EntityEx entity, void* componentData = nullptr);
//		COCOA void* GetComponentInternal(EngineComponent componentType, EntityEx entity);
//		COCOA size_t GetSizeOf(EngineComponent componentType);
//	}
//
//	namespace ECS
//	{
//		COCOA void Init();
//		COCOA void Destroy();
//
//		COCOA EntityEx CreateEntity();
//	}
//}
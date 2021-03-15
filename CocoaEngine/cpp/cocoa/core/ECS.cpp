//#include "cocoa/core/ECS.h"
//#include "cocoa/core/Memory.h"
//#include "cocoa/util/Log.h"
//#include "cocoa/util/DynamicArray.h"
//
//#include "cocoa/components/TransformStruct.h"
//#include "cocoa/components/Tag.h"
//#include "cocoa/components/SpriteRenderer.h"
//#include "cocoa/components/FontRenderer.h"
//#include "cocoa/physics2d/PhysicsComponents.h"
//
//namespace Cocoa
//{
//	// Constant variables
//	static const int MemoryPoolLength = 10;
//	static const int InitialArrayLength = 16;
//
//	// Internal Structures
//	struct MemoryBlob
//	{
//		size_t ElementSize;
//		int32 NumElements;
//		int32 MaxSize;
//		char* Data;
//	};
//
//	struct ComponentIndex
//	{
//		int16 ThisList;
//		int16 NextList;
//		int32 ThisEntity;
//		int32 NextComponentIndex;
//	};
//
//	struct EntityIndex
//	{
//		int32 FirstComponentIndex; // If this is negative one, the entity is dead
//		union
//		{
//			int32 NextFreeEntity;
//			int16 FirstList; // If this is negative one, the entity has no components
//		};
//	};
//
//	struct Registry
//	{
//		int32 FirstFreeEntity;
//		int32 MaxSize;
//		int32 NumEntities;
//		MemoryBlob ComponentBlobs[EngineComponent::_Size];
//		EntityIndex Entities[];
//	};
//
//	namespace ECS
//	{
//		// Forward declarations
//		static void FreeComponentMetadata(EngineComponent component);
//		static void Register(EngineComponent component, size_t sizeOfComponent);
//
//		// Internal Variables
//		static Registry* EntityRegistry;
//
//		// Public methods
//		void Init()
//		{
//			EntityRegistry = (Registry*)AllocMem(sizeof(Registry) + InitialArrayLength * sizeof(EntityIndex));
//			EntityRegistry->MaxSize = InitialArrayLength;
//			EntityRegistry->NumEntities = 0;
//
//			Register(EngineComponent::_Transform, sizeof(TransformData));
//			Register(EngineComponent::_Tag, sizeof(Tag));
//			Register(EngineComponent::_SpriteRenderer, sizeof(SpriteRenderer));
//			Register(EngineComponent::_FontRenderer, sizeof(FontRenderer));
//			Register(EngineComponent::_Box2D, sizeof(Box2D));
//			Register(EngineComponent::_AABB, sizeof(AABB));
//			Register(EngineComponent::_Circle, sizeof(Circle));
//			Register(EngineComponent::_Rigidbody2D, sizeof(Rigidbody2D));
//		}
//
//		void Destroy()
//		{
//			for (int i = 0; i < EngineComponent::_Size; i++)
//			{
//				FreeComponentMetadata((EngineComponent)i);
//			}
//
//			NDynamicArray::Free<int>(EntityRegistry.EntityFreeList);
//		}
//
//		EntityEx CreateEntity()
//		{
//			int nextAvailableIndex = EntityRegistry->FirstFreeEntity;
//			EntityRegistry->Entities[nextAvailableIndex].FirstComponentIndex = 0;
//			EntityRegistry->Entities[nextAvailableIndex].FirstList = -1;
//			EntityRegistry->NumEntities++;
//
//			if (EntityRegistry->Entities[nextAvailableIndex].NextFreeEntity != -1)
//			{
//				EntityRegistry->FirstFreeEntity = EntityRegistry->Entities[nextAvailableIndex].NextFreeEntity;
//			}
//			else
//			{
//				EntityRegistry->FirstFreeEntity = EntityRegistry->NumEntities;
//			}
//
//			return EntityEx{ nextAvailableIndex };
//		}
//
//		static void Register(EngineComponent component, size_t sizeOfComponent)
//		{
//			MemoryBlob componentBlob = EntityRegistry->ComponentBlobs[component];
//			Log::Assert(componentBlob.Data == nullptr, "Tried to register component '%d' twice.", component);
//			componentBlob.Data = (char*)AllocMem(sizeOfComponent * InitialArrayLength);
//			componentBlob.ElementSize = sizeOfComponent;
//			componentBlob.NumElements = 0;
//			componentBlob.MaxSize = InitialArrayLength;
//		}
//
//		static void FreeComponentMetadata(EngineComponent component)
//		{
//			for (int i = 0; i < MemoryPoolLength; i++)
//			{
//				ComponentBatch memory = ComponentRegistry[component].ComponentBatches[i];
//				if (memory.ArrayStart != nullptr)
//				{
//					FreeMem(memory.ArrayStart);
//				}
//			}
//		}
//	}
//
//	namespace ECSInternal
//	{
//		void AddComponentInternal(EngineComponent componentType, EntityEx entity, void* componentData)
//		{
//			ComponentMetadata& pool = ECS::ComponentRegistry[componentType];
//			int batchIndex = entity.Handle / ComponentBatchSize;
//			int batchInternalIndex = entity.Handle % ComponentBatchSize;
//
//			Log::Assert(batchIndex >= 0, "Invalid entity.");
//			Log::Assert(batchIndex < MemoryPoolLength, "Ran out of memory for component type '%d'. Add dynamic memory pool", componentType);
//			if (pool.ComponentBatches[batchIndex].ArrayStart == nullptr)
//			{
//				pool.ComponentBatches[batchIndex].ArrayStart = AllocMem(pool.Size * ComponentBatchSize);
//			}
//
//			void* newComponentAddress = (void*)((char*)pool.ComponentBatches[batchIndex].ArrayStart + (pool.Size * batchInternalIndex));
//			memcpy(newComponentAddress, componentData, pool.Size);
//		}
//
//		void* GetComponentInternal(EngineComponent componentType, EntityEx entity)
//		{
//			ComponentMetadata& pool = ECS::ComponentRegistry[componentType];
//			int batchIndex = entity.Handle / ComponentBatchSize;
//			int batchInternalIndex = entity.Handle % ComponentBatchSize;
//
//			Log::Assert(batchIndex >= 0, "Invalid entity.");
//			Log::Assert(batchIndex < MemoryPoolLength&& pool.ComponentBatches[batchIndex].ArrayStart != nullptr, "Invalid Entity.");
//
//			void* componentAddress = (void*)((char*)pool.ComponentBatches[batchIndex].ArrayStart + (pool.Size * batchInternalIndex));
//			Log::Assert(ECS::EntityRegistry.EntitiyStatus.m_Data[entity.Handle] == 1, "Entity does not exist.");
//			return componentAddress;
//		}
//
//		size_t GetSizeOf(EngineComponent componentType)
//		{
//			Log::Assert(componentType < EngineComponent::_Size, "Invalid component.");
//			return ECS::ComponentRegistry[componentType].Size;
//		}
//	}
//}
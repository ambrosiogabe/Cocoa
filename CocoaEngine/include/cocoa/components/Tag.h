#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
	struct Tag
	{
		const char* Name;
		int Size;
		bool IsHeapAllocated;
	};

	namespace NTag
	{
		COCOA Tag CreateTag(const char* name, bool isHeapAllocated = false);
		COCOA void Destroy(Tag& tag);

		COCOA void Serialize(json& j, Entity entity, const Tag& transform);
		COCOA void Deserialize(const json& j, Entity entity);
	}
}
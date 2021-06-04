#ifndef COCOA_ENGINE_TAG_H
#define COCOA_ENGINE_TAG_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
	struct Tag
	{
		const char* name;
		int size;
		bool isHeapAllocated;
	};

	namespace NTag
	{
		COCOA Tag createTag(const char* name, bool isHeapAllocated = false);
		COCOA void destroy(Tag& tag);

		COCOA void serialize(json& j, Entity entity, const Tag& transform);
		COCOA void deserialize(const json& j, Entity entity);
	}
}

#endif

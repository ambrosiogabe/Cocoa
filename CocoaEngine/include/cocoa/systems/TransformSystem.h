#ifndef COCOA_ENGINE_TRANSFORM_SYSTEM_H
#define COCOA_ENGINE_TRANSFORM_SYSTEM_H
#include "externalLibs.h"
#include "cocoa/scenes/SceneData.h"

#include "cocoa/core/EntityStruct.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		COCOA void update(SceneData& scene, float dt);
		COCOA void destroy(SceneData& scene);
		COCOA void deleteEntity(Entity entity);
	}
}

#endif

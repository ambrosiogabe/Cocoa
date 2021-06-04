#pragma once
#include "externalLibs.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		COCOA void update(SceneData& scene, float dt);
		COCOA void Destroy(SceneData& scene);
		COCOA void DeleteEntity(Entity entity);
	}
}
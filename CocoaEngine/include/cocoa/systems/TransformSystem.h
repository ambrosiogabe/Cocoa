#pragma once
#include "externalLibs.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace TransformSystem
	{
		COCOA void Update(SceneData& scene, float dt);
		COCOA void Destroy(SceneData& scene);
		COCOA void DeleteEntity(Entity entity);
	}
}
#ifndef COCOA_ENGINE_SCENE_DATA_H
#define COCOA_ENGINE_SCENE_DATA_H
#include "cocoa/renderer/Shader.h"

namespace Cocoa
{
	class SceneInitializer;
	struct SceneData
	{
		bool isPlaying;
		entt::registry registry;
		json saveDataJson;

		SceneInitializer* currentSceneInitializer = nullptr;
	};
}

#endif

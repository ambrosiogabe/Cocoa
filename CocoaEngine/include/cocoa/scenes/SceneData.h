#pragma once
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"
#include "cocoa/renderer/CameraStruct.h"

namespace Cocoa
{
	class SceneInitializer;
	struct SceneData
	{
		bool IsPlaying;
		entt::registry Registry;
		json SaveDataJson;

		SceneInitializer* CurrentSceneInitializer = nullptr;
	};
}
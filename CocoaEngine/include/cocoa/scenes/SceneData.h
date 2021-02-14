#pragma once
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"

namespace Cocoa
{
	class Camera;
	class SceneInitializer;
	struct SceneData
	{
		bool IsPlaying;

		entt::registry Registry;
		json SaveDataJson;

		Camera* SceneCamera = nullptr;
		SceneInitializer* CurrentSceneInitializer = nullptr;
	};
}
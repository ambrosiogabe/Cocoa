#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/events/KeyEvent.h"
#include "cocoa/events/MouseEvent.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace LevelEditorSystem
	{
		void EditorUpdate(SceneData& scene, float dt);
		void Init(SceneData& scene);
		void OnEvent(SceneData& scene, Event& e);
		void Destroy(SceneData& scene);

		Camera& GetCamera();

		void Serialize(json& j);
		void Deserialize(const json& j, SceneData& scene);
	};
}
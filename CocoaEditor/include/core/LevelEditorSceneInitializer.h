#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneInitializer.h"

#include "cocoa/systems/System.h"

namespace Cocoa
{
	class Scene;
	class LevelEditorSceneInitializer : public SceneInitializer
	{
	public:
		LevelEditorSceneInitializer() {}

		virtual void Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems) override;
		virtual void Start(Scene* scene) override;
	};
}
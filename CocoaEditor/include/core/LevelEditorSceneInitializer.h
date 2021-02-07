#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneInitializer.h"

namespace Cocoa
{
	class Scene;
	class LevelEditorSceneInitializer : public SceneInitializer
	{
	public:
		LevelEditorSceneInitializer() {}

		virtual void Init(Scene* scene) override;
		virtual void Start(Scene* scene) override;
	};
}
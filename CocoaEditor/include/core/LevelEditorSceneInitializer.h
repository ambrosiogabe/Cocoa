#pragma once
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneInitializer.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class LevelEditorSceneInitializer : public SceneInitializer
	{
	public:
		LevelEditorSceneInitializer() {}

		virtual void Init(SceneData& scene) override;
		virtual void Start(SceneData& scene) override;
		virtual void Destroy(SceneData& scene) override;
		virtual void Save(SceneData& scene) override;
		virtual void Load(SceneData& scene) override;
	};
}
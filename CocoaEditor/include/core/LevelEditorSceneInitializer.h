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

		virtual void init(SceneData& scene) override;
		virtual void start(SceneData& scene) override;
		virtual void destroy(SceneData& scene) override;
		virtual void save(SceneData& scene) override;
		virtual void load(SceneData& scene) override;
	};
}
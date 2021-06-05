#ifndef COCOA_EDITOR_LEVEL_EDITOR_SCENE_INITIALIZER_H
#define COCOA_EDITOR_LEVEL_EDITOR_SCENE_INITIALIZER_H
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

		void init(SceneData& scene) override;
		void start(SceneData& scene) override;
		void destroy(SceneData& scene) override;
		void save(SceneData& scene) override;
		void load(SceneData& scene) override;
	};
}

#endif

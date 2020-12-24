#pragma once
#include "jade/core/Core.h"
#include "jade/scenes/SceneInitializer.h"

#include "jade/systems/System.h"

namespace Jade
{
	class Scene;
	class LevelEditorSceneInitializer : public SceneInitializer
	{
	public:
		LevelEditorSceneInitializer() {}

		virtual void Init(Scene* scene, std::vector<std::unique_ptr<System>>& systems) override;
	};
}
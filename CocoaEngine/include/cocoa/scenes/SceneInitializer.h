#ifndef COCOA_ENGINE_SCENE_INITIALIZER_H
#define COCOA_ENGINE_SCENE_INITIALIZER_H
#include "cocoa/core/Core.h"
#include "externalLibs.h"

#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	class COCOA SceneInitializer
	{
	public:
		SceneInitializer() {}

		virtual void init(SceneData& scene) = 0;
		virtual void start(SceneData& scene) = 0;
		virtual void destroy(SceneData& scene) = 0;
		virtual void save(SceneData& scene) = 0;
		virtual void load(SceneData& scene) = 0;
	};
}

#endif

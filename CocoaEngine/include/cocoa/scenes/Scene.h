#ifndef COCOA_ENGINE_SCENE_H
#define COCOA_ENGINE_SCENE_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/CPath.h"
#include "cocoa/events/Event.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
	namespace Scene
	{
		COCOA SceneData create(SceneInitializer* sceneInitializer);

		COCOA void init(SceneData& data);
		COCOA void start(SceneData& data);
		COCOA void update(SceneData& data, float dt);
		COCOA void editorUpdate(SceneData& data, float dt);
		COCOA void onEvent(SceneData& data, const Event& e);
		COCOA void render(SceneData& data);
		COCOA void freeResources(SceneData& data);

		COCOA void play(SceneData& data);
		COCOA void stop(SceneData& data);
		COCOA void save(SceneData& data, const CPath& filename);
		COCOA void load(SceneData& data, const CPath& filename, bool setAsCurrentScene = true);
		COCOA void loadScriptsOnly(SceneData& data, const CPath& filename);

		COCOA void serializeEntity(json* j, Entity entity);
		COCOA void deserializeEntities(const json& j, SceneData& scene);
		COCOA Entity createEntity(SceneData& data);
		COCOA Entity duplicateEntity(SceneData& data, Entity entity);
		COCOA Entity getEntity(SceneData& data, uint32 id);
		COCOA bool isValid(SceneData& scene, uint32 entityId);
		COCOA bool isValid(SceneData& scene, Entity entity);
		COCOA void deleteEntity(SceneData& data, Entity entity);
	};
}

#endif

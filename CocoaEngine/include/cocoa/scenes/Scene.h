#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/file/CPath.h"
#include "cocoa/scenes/SceneInitializer.h"
#include "cocoa/renderer/PickingTexture.h"
#include "cocoa/renderer/Shader.h"
#include "cocoa/core/Handle.h"
#include "cocoa/events/Event.h"
#include "cocoa/scenes/SceneData.h"
#include "cocoa/core/Entity.h"

#include <entt/entt.hpp>

namespace Cocoa
{
	namespace Scene
	{
		COCOA SceneData Create(SceneInitializer* sceneInitializer);

		COCOA void Init(SceneData& data);
		COCOA void Start(SceneData& data);
		COCOA void Update(SceneData& data, float dt);
		COCOA void EditorUpdate(SceneData& data, float dt);
		COCOA void OnEvent(SceneData& data, const Event& e);
		COCOA void Render(SceneData& data);
		COCOA void Destroy(SceneData& data);

		COCOA void Play(SceneData& data);
		COCOA void Stop(SceneData& data);
		COCOA void Save(SceneData& data, const CPath& filename);
		COCOA void Load(SceneData& data, const CPath& filename);
		COCOA void LoadScriptsOnly(SceneData& data, const CPath& filename);
		COCOA void Reset(SceneData& data);

		COCOA Entity CreateEntity(SceneData& data);
		COCOA Entity DuplicateEntity(SceneData& data, Entity entity);
		COCOA Entity GetEntity(SceneData& data, uint32 id);
	};
}
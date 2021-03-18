#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
    typedef void (*AddComponentFn)(entt::registry&, std::string, entt::entity);
    typedef void (*UpdateScriptFn)(entt::registry&, float);
    typedef void (*EditorUpdateScriptFn)(entt::registry&, float);
    typedef void (*SaveScriptsFn)(entt::registry&, json&, SceneData*);
    typedef void (*LoadScriptFn)(entt::registry&, json&, Entity);
    typedef void (*InitScriptsFn)(SceneData*);
    typedef void (*InitImGuiFn)(void*);
    typedef void (*ImGuiFn)(entt::registry&, Entity);
    typedef void (*DeleteScriptsFn)();
    typedef void (*NotifyBeginContactFn)(Entity, Entity);
    typedef void (*NotifyEndContactFn)(Entity, Entity);

    namespace ScriptSystem
    {
        COCOA void Init(SceneData& scene);
        COCOA void Update(SceneData& scene, float dt);
        COCOA void EditorUpdate(SceneData& scene, float dt);

        COCOA void ImGui(SceneData& scene, Entity entity);
        COCOA void InitImGui(void* context);

        COCOA void NotifyBeginContact(Entity entityA, Entity entityB);
        COCOA void NotifyEndContact(Entity entityA, Entity entityB);

        COCOA void Reload(SceneData& scene, bool deleteScriptComponents = false);
        COCOA void SaveScripts(SceneData& scene, json& j);
        COCOA void Deserialize(SceneData& scene, json& j, Entity entity);

        COCOA bool FreeScriptLibrary(SceneData& scene, bool deleteScriptComponents = false);
        COCOA void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry);
    };
}
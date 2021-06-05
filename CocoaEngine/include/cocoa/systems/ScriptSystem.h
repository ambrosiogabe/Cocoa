#ifndef COCOA_ENGINE_SCRIPT_SYSTEM_H
#define COCOA_ENGINE_SCRIPT_SYSTEM_H
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
    typedef void (*AddComponentFn)(entt::registry&, std::string, entt::entity);
    typedef void (*UpdateScriptFn)(entt::registry&, float);
    typedef void (*EditorUpdateScriptFn)(entt::registry&, float);
    typedef void (*SaveScriptsFn)(entt::registry&, json&, SceneData*);
    typedef void (*LoadScriptFn)(entt::registry&, const json&, Entity);
    typedef void (*InitScriptsFn)(SceneData*);
    typedef void (*InitImGuiFn)(void*);
    typedef void (*ImGuiFn)(entt::registry&, Entity);
    typedef void (*DeleteScriptsFn)();
    typedef void (*NotifyBeginContactFn)(Entity, Entity);
    typedef void (*NotifyEndContactFn)(Entity, Entity);

    namespace ScriptSystem
    {
        COCOA void init(SceneData& scene);
        COCOA void update(SceneData& scene, float dt);
        COCOA void editorUpdate(SceneData& scene, float dt);

        COCOA void imGui(SceneData& scene, Entity entity);
        COCOA void initImGui(void* context);

        COCOA void notifyBeginContact(Entity entityA, Entity entityB);
        COCOA void notifyEndContact(Entity entityA, Entity entityB);

        COCOA void reload(SceneData& scene, bool deleteScriptComponents = false);
        COCOA void saveScripts(SceneData& scene, json& j);
        COCOA void deserialize(SceneData& scene, const json& j, Entity entity);

        COCOA bool freeScriptLibrary(SceneData& scene, bool deleteScriptComponents = false);
        COCOA void addComponentFromString(std::string className, entt::entity entity, entt::registry& registry);
    };
}

#endif

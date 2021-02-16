#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"
#include "cocoa/scenes/SceneData.h"

namespace Cocoa
{
    typedef void (*DeleteScriptFn)();
    typedef void (*InitScriptsFn)();
    typedef void (*InitImGuiFn)(void*);
    typedef void (*SaveScriptFn)(json&);
    typedef void (*LoadScriptFn)(json&, Entity);
    typedef void (*ImGuiFn)(Entity);
    typedef void (*UpdateScriptFn)(float, SceneData*);
    typedef void (*EditorUpdateScriptFn)(float, SceneData*);
    typedef void (*AddComponentFromStringFn)(std::string, entt::entity, entt::registry&);

    namespace ScriptSystem
    {
        COCOA void Init();
        COCOA void Update(SceneData& scene, float dt);
        COCOA void EditorUpdate(SceneData& scene, float dt);

        COCOA void ImGui(Entity entity);
        COCOA void InitImGui(void* context);

        COCOA void Reload();
        COCOA void SaveScripts(json& j);
        COCOA void Deserialize(json& j, Entity entity);

        COCOA bool FreeScriptLibrary();
        COCOA void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry);
    };
}
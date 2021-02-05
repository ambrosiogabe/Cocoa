#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"

namespace Cocoa
{
    typedef void (*DeleteScriptFn)();
    typedef void (*InitScriptsFn)();
    typedef void (*InitImGuiFn)(void*);
    typedef void (*SaveScriptFn)(json&);
    typedef void (*LoadScriptFn)(json&, Entity);
    typedef void (*ImGuiFn)(Entity);
    typedef void (*UpdateScriptFn)(float, Scene*);
    typedef void (*EditorUpdateScriptFn)(float, Scene*);
    typedef void (*AddComponentFromStringFn)(std::string, entt::entity, entt::registry&);

    namespace ScriptSystem
    {
        COCOA void Start();
        // TODO: Update these to const pointers, no reason to pass a non-const pointer here
        COCOA void Update(Scene* scene, float dt);
        COCOA void EditorUpdate(Scene* scene, float dt);

        COCOA void ImGui(Entity entity);
        COCOA void InitImGui(void* context);

        COCOA void Reload();
        COCOA void SaveScripts(json& j);
        COCOA void Deserialize(json& j, Entity entity);

        COCOA bool FreeScriptLibrary();
        COCOA void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry);
    };
}
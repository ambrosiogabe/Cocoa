#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/systems/System.h"
#include "jade/scenes/Scene.h"
#include "jade/core/Entity.h"

#include <Windows.h>
#include <libloaderapi.h>

class ImGuiContext;
namespace Jade
{
    typedef void (*DeleteScriptFn)();
    typedef void (*InitScriptsFn)();
    typedef void (*InitImGuiFn)(ImGuiContext*);
    typedef void (*SaveScriptFn)(json&);
    typedef void (*LoadScriptFn)(json&, Entity);
    typedef void (*ImGuiFn)(Entity);
    typedef void (*UpdateScriptFn)(float, Scene*);
    typedef void (*EditorUpdateScriptFn)(float, Scene*);
    typedef void (*AddComponentFromStringFn)(std::string, entt::entity, entt::registry&);

    class JADE ScriptSystem : public System
    {
    public:
        ScriptSystem(const char* name, Scene* scene)
            : System(name, scene)
        {
            m_Module = NULL;
        }
        ~ScriptSystem();

        virtual void Start() override;
        virtual void Update(float dt) override;
        virtual void EditorUpdate(float dt) override;
        void ImGui(Entity entity);
        void Reload();
        void SaveScripts(json& j);
        void Deserialize(json& j, Entity entity);

        bool FreeScriptLibrary();
        void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry);

    public:
        InitImGuiFn m_InitImGui = nullptr;

    private:
        SaveScriptFn m_SaveScripts = nullptr;
        LoadScriptFn m_LoadScript = nullptr;
        DeleteScriptFn m_DeleteScripts = nullptr;
        UpdateScriptFn m_UpdateScripts = nullptr;
        InitScriptsFn m_InitScripts = nullptr;
        EditorUpdateScriptFn m_EditorUpdateScripts = nullptr;
        AddComponentFromStringFn m_AddComponentFromString = nullptr;
        ImGuiFn m_ImGui = nullptr;

        bool m_IsLoaded = false;
        HMODULE m_Module;
    };
}
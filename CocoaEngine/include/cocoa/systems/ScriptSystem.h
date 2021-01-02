#pragma once
#include "externalLibs.h"
#include "cocoa/core/Core.h"

#include "cocoa/systems/System.h"
#include "cocoa/scenes/Scene.h"
#include "cocoa/core/Entity.h"

#ifdef _WIN32
#include <Windows.h>
#include <libloaderapi.h>
#endif

class ImGuiContext;
namespace Cocoa
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

    class COCOA ScriptSystem : public System
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
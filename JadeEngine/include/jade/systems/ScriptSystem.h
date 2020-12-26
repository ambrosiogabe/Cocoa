#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/systems/System.h"
#include "jade/scenes/Scene.h"

#include <Windows.h>
#include <libloaderapi.h>

namespace Jade
{
    typedef void(*SaveScriptFn)();
    typedef void(*DeleteScriptFn)();
    typedef void(*LoadScriptFn)();
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

        virtual void Start() override;
        virtual void Update(float dt) override;
        void Reload();
        void SaveScriptState();

        void FreeScriptLibrary();
        void AddComponentFromString(std::string className, entt::entity entity, entt::registry& registry);

    private:
        SaveScriptFn m_SaveScripts = nullptr;
        LoadScriptFn m_LoadScripts = nullptr;
        DeleteScriptFn m_DeleteScripts = nullptr;
        UpdateScriptFn m_UpdateScripts = nullptr;
        EditorUpdateScriptFn m_EditorUpdateScripts = nullptr;
        AddComponentFromStringFn m_AddComponentFromString = nullptr;

        bool m_IsLoaded = false;
        HMODULE m_Module;
    };
}
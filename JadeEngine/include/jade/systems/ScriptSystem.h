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

    class JADE ScriptSystem : public System
    {
    public:
        ScriptSystem(const char* name, Scene* scene)
            : System(name, scene)
        {
        }

        virtual void Start() override;
        virtual void Update(float dt) override;
        void Reload();
        void SaveScriptState();

    private:
        SaveScriptFn m_SaveScripts = nullptr;
        LoadScriptFn m_LoadScripts = nullptr;
        DeleteScriptFn m_DeleteScripts = nullptr;
        UpdateScriptFn m_UpdateScripts = nullptr;
        EditorUpdateScriptFn m_EditorUpdateScripts = nullptr;

        bool m_IsLoaded = false;
        HMODULE m_Module;
    };
}
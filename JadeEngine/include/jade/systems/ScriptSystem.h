#pragma once
#include "externalLibs.h"
#include "jade/core/Core.h"

#include "jade/systems/System.h"

#include <Windows.h>
#include <libloaderapi.h>

namespace Jade
{
    class JADE ScriptSystem : public System
    {
    public:
        ScriptSystem(const char* name, Scene* scene)
            : System(name, scene)
        {
        }

        virtual void Update(float dt) override;

    };
}
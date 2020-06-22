#pragma once

#include "core/Core.h"

#include <string>

namespace Jade {
    class JWindow {
        virtual ~JWindow() {}

        virtual void OnUpdate() = 0;

        virtual uint32 GetWidth() const = 0;
        virtual uint32 GetHeight() const = 0;

        // Window Attributes
        virtual void SetVSync(bool enabled) = 0;
        virtual bool IsVSync() const = 0;

        virtual void* GetNativeWindow() const = 0;

        static JWindow* Create(uint32 width, uint32 height, const std::string& name);
    };
}
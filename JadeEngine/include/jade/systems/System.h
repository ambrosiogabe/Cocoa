#pragma once

#include "jade/events/Event.h"

#include <entt.h>
#include <string>

namespace Jade {
    class System {
    public:
        System(const char* name) 
            : m_Name(name) {}


        virtual void Start(entt::registry& registry) {

        }

        virtual void Update(entt::registry& registry, float dt) {

        }

        virtual void Render(entt::registry& registry) {

        }

        virtual void ImGui(entt::registry& registry) {
            
        }

        virtual void OnEvent(Event& e) {

        }

        const char* GetName() {
            return m_Name;
        }

    private:
        const char* m_Name = "";
    };
}
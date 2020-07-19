#pragma once
#include "externalLibs.h"

#include "jade/events/Event.h"


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
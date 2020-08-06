#pragma once
#include "externalLibs.h"

#include "jade/events/Event.h"

namespace Jade {

    class Scene;

    class System {
    public:
        System(const char* name, Scene* scene) 
            : m_Name(name), m_Scene(scene) {}


        virtual void Start() {

        }

        virtual void Update(float dt) {

        }

        virtual void Render() {

        }

        virtual void ImGui() {
            
        }

        virtual void OnEvent(Event& e) {

        }

        const char* GetName() {
            return m_Name;
        }

    private:
        const char* m_Name = "";

    protected:
        Scene* m_Scene = nullptr;
    };
}
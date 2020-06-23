#include "Jade.h"
#include "LevelEditorScene.cpp"
#include "LevelEditorSystem.cpp"

namespace Jade {
    class EditorLayer : public Layer {
    public:
        EditorLayer() {
            Application::Get()->ChangeScene(new LevelEditorScene());
        }

        virtual void OnAttach() override {

        }

        virtual void OnUpdate(float dt) override {
            Application::Get()->GetScene()->Update(dt);
        }

        virtual void OnImGuiRender() override {
            Application::Get()->GetScene()->ImGui();
        }

        virtual void OnRender() override {
            Application::Get()->GetScene()->Render();
        }
    };

    class JadeEditor : public Application {
    public:
        JadeEditor() {
            Log::Info("Running app!");
        }

        ~JadeEditor() {

        }
    };


    Application* CreateApplication() {
        JadeEditor* editor = new JadeEditor();
        editor->PushLayer(new EditorLayer());
        return editor;
    }
}
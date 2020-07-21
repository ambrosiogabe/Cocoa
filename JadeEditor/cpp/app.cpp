#include "Jade.h"

#include "LevelEditorSystem.h"
#include "LevelEditorScene.h"

#include <glad/glad.h>

namespace Jade {
    class EditorLayer : public Layer {
    public:
        EditorLayer() {
            Application::Get()->ChangeScene(new LevelEditorScene());
        }

        virtual void OnAttach() override {

        }

        virtual void OnUpdate(float dt) override {
            DebugDraw::BeginFrame();
            Application::Get()->GetScene()->Update(dt);
        }

        virtual void OnImGuiRender() override {
            Application::Get()->GetScene()->ImGui();
        }

        virtual void OnRender() override {
            glBindFramebuffer(GL_FRAMEBUFFER, Application::Get()->GetFramebuffer()->GetId());
            glViewport(0, 0, 3840, 2160);
            glClearColor(0.45f, 0.55f, 0.6f, 1.0f);
            glClear(GL_COLOR_BUFFER_BIT);

            DebugDraw::DrawBottomBatches();
            Application::Get()->GetScene()->Render();
            DebugDraw::DrawTopBatches();
        }

        virtual void OnEvent(Event& e) override {
            const auto& systems = Application::Get()->GetScene()->GetSystems();

            for (const auto& system : systems) {
                system->OnEvent(e);
            }
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
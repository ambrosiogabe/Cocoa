#include "Jade.h"

namespace Jade {
    class EditorLayer : public Layer {
        virtual void OnUpdate(float dt) override {
            Log::Info("Updating!");
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
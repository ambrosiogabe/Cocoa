#include "Jade.h"

class JadeEditor : public Jade::Application {
public:
    JadeEditor() {
        Log::Info("Running app!");
    }

    ~JadeEditor() {

    }
};


Jade::Application* Jade::CreateApplication() {
    return new JadeEditor();
}
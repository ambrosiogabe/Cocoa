#include "Jade.h"

namespace Jade {
    class JadeEditor : public Application {
    public:
        JadeEditor() {
            Log::Info("Running app!");
        }

        ~JadeEditor() {

        }
    };


    Application* CreateApplication() {
        return new JadeEditor();
    }
}
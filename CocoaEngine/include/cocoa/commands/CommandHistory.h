#ifndef COCOA_ENGINE_COMMAND_HISTORY
#define COCOA_ENGINE_COMMAND_HISTORY
#include "cocoa/commands/ICommand.h"

namespace Cocoa {
    class COCOA CommandHistory {
    public:
        static void addCommand(ICommand* cmd);
        static void setNoMergeMostRecent();
        static void undo();
        static void redo();

    private:
        static ICommand* mCommands[1000];
        static int mCommandSize;
        static int mCommandPtr;
    };
}

#endif

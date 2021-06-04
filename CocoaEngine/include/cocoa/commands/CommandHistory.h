#pragma once
#include "cocoa/commands/ICommand.h"

namespace Cocoa {
    class COCOA CommandHistory {
    public:
        static void AddCommand(ICommand* cmd);
        static void SetNoMergeMostRecent();
        static void Undo();
        static void Redo();

    private:
        static ICommand* m_Commands[1000];
        static int m_CommandSize;
        static int m_CommandPtr;
    };
}
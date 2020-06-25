#pragma once

#include "jade/commands/ICommand.h"
#include "jade/util/Log.h"

namespace Jade {
    class CommandHistory {
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
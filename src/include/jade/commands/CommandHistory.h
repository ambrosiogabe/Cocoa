#pragma once

#include "jade/commands/ICommand.h"
#include "jade/util/Log.h"

class CommandHistory {
public:
    static void AddCommand(ICommand* cmd) {
        cmd->execute();

        if (m_CommandPtr < m_CommandSize - 1) {
            for (int i=m_CommandSize - 1; i > m_CommandPtr; i--) {
                delete m_Commands[i];
            }
            m_CommandSize = m_CommandPtr + 1;
        }

        m_Commands[m_CommandSize] = cmd;
        m_CommandSize++;

        if (m_CommandSize > 1 && m_Commands[m_CommandSize - 1]->CanMerge() && m_Commands[m_CommandSize - 2]->CanMerge()) {
            if (m_Commands[m_CommandSize - 1]->mergeWith(m_Commands[m_CommandSize - 2])) {
                delete m_Commands[m_CommandSize - 1];
                m_CommandSize--;
            }
        }

        m_CommandPtr = m_CommandSize - 1;
    }

    static void SetNoMergeMostRecent() {
        m_Commands[m_CommandSize - 1]->SetNoMerge();
    }

    static void Undo() {
        if (m_CommandPtr >= 0) {
            m_Commands[m_CommandPtr]->undo();
            m_CommandPtr--;
        }
    }

    static void Redo() {
        int redoCommand = m_CommandPtr + 1;
        if (redoCommand < m_CommandSize && redoCommand >= 0) {
            m_Commands[redoCommand]->execute();
            m_CommandPtr++;
        }
    }

private:
    static ICommand* m_Commands[1000];
    static int m_CommandSize;
    static int m_CommandPtr;
};


ICommand* CommandHistory::m_Commands[1000] = {};
int CommandHistory::m_CommandSize = 0;
int CommandHistory::m_CommandPtr = 0;
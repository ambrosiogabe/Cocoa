#pragma once
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeIntCommand : public ICommand
    {
    public:
        ChangeIntCommand(int& originalInt, int newInt)
            : m_Int(originalInt), m_NewInt(newInt), m_OldInt(0)
        {
        }

        virtual void execute() override
        {
            m_OldInt = m_Int;
            m_Int = m_NewInt;
        }

        virtual void undo() override
        {
            m_Int = m_OldInt;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeIntCommand* changeIntCommand = dynamic_cast<ChangeIntCommand*>(other);
            if (changeIntCommand != nullptr)
            {
                if (&changeIntCommand->m_Int == &this->m_Int)
                {
                    changeIntCommand->m_NewInt = this->m_NewInt;
                    return true;
                }
            }

            return false;
        }


    private:
        int& m_Int;
        int m_NewInt;
        int m_OldInt;
    };
}
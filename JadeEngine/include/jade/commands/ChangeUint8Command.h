#pragma once

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

namespace Jade
{
    class JADE ChangeUint8Command : public ICommand
    {
    public:
        ChangeUint8Command(uint8& originalUint8, uint8 newUint8)
            : m_Uint8(originalUint8), m_NewUint8(newUint8), m_OldUint8(0)
        {
        }

        virtual void execute() override
        {
            m_OldUint8 = m_Uint8;
            m_Uint8 = m_NewUint8;
        }

        virtual void undo() override
        {
            m_Uint8 = m_OldUint8;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeUint8Command* changeUint8Command = dynamic_cast<ChangeUint8Command*>(other);
            if (changeUint8Command != nullptr)
            {
                if (&changeUint8Command->m_Uint8 == &this->m_Uint8)
                {
                    changeUint8Command->m_NewUint8 = this->m_NewUint8;
                    return true;
                }
            }

            return false;
        }


    private:
        uint8& m_Uint8;
        uint8 m_NewUint8;
        uint8 m_OldUint8;
    };
}
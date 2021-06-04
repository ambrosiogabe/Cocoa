#ifndef COCOA_ENGINE_CHANGE_UINT8_COMMAND_H
#define COCOA_ENGINE_CHANGE_UINT8_COMMAND_H
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeUint8Command final : public ICommand
    {
    public:
        ChangeUint8Command(uint8& originalUint8, uint8 newUint8)
            : mUint8(originalUint8), mNewUint8(newUint8), mOldUint8(0)
        {
        }

        void execute() override
        {
            mOldUint8 = mUint8;
            mUint8 = mNewUint8;
        }

        void undo() override
        {
            mUint8 = mOldUint8;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeUint8Command* changeUint8Command = dynamic_cast<ChangeUint8Command*>(other);
            if (changeUint8Command != nullptr)
            {
                if (&changeUint8Command->mUint8 == &this->mUint8)
                {
                    changeUint8Command->mNewUint8 = this->mNewUint8;
                    return true;
                }
            }

            return false;
        }


    private:
        uint8& mUint8;
        uint8 mNewUint8;
        uint8 mOldUint8;
    };
}

#endif

#ifndef COCOA_ENGINE_CHANGE_INT_COMMAND_H
#define COCOA_ENGINE_CHANGE_INT_COMMAND_H
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeIntCommand final : public ICommand
    {
    public:
        ChangeIntCommand(int& originalInt, const int newInt)
            : mInt(originalInt), mNewInt(newInt), mOldInt(0)
        {
        }

        void execute() override
        {
            mOldInt = mInt;
            mInt = mNewInt;
        }

        void undo() override
        {
            mInt = mOldInt;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeIntCommand* changeIntCommand = dynamic_cast<ChangeIntCommand*>(other);
            if (changeIntCommand != nullptr)
            {
                if (&changeIntCommand->mInt == &this->mInt)
                {
                    changeIntCommand->mNewInt = this->mNewInt;
                    return true;
                }
            }

            return false;
        }


    private:
        int& mInt;
        int mNewInt;
        int mOldInt;
    };
}

#endif

#ifndef COCOA_ENGINE_CHANGE_FLOAT_COMMAND_H
#define COCOA_ENGINE_CHANGE_FLOAT_COMMAND_H
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeFloatCommand final : public ICommand
    {
    public:
        ChangeFloatCommand(float& originalFloat, float newFloat)
            : mFloat(originalFloat), mNewFloat(newFloat), mOldFloat(0)
        {
        }

        void execute() override
        {
            mOldFloat = mFloat;
            mFloat = mNewFloat;
        }

        void undo() override
        {
            mFloat = mOldFloat;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeFloatCommand* changeFloatCommand = dynamic_cast<ChangeFloatCommand*>(other);
            if (changeFloatCommand != nullptr)
            {
                if (&changeFloatCommand->mFloat == &this->mFloat)
                {
                    changeFloatCommand->mNewFloat = this->mNewFloat;
                    return true;
                }
            }

            return false;
        }


    private:
        float& mFloat;
        float mNewFloat;
        float mOldFloat;
    };
}

#endif

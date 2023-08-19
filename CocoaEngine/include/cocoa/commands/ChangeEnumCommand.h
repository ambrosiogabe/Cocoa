#ifndef COCOA_ENGINE_CHANGE_ENUM_COMMAND_H
#define COCOA_ENGINE_CHANGE_ENUM_COMMAND_H
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    template<typename T>
    class ChangeEnumCommand final : public ICommand
    {
    public:
        ChangeEnumCommand(T& originalEnum, T newEnum)
            : mEnum(originalEnum), mNewEnum(newEnum), mOldEnum(static_cast<T>(0))
        {
        }

        void execute() override
        {
            mOldEnum = mEnum;
            mEnum = mNewEnum;
        }

        void undo() override
        {
            mEnum = mOldEnum;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeEnumCommand<T>* changeEnumCommand = dynamic_cast<ChangeEnumCommand<T>*>(other);
            if (changeEnumCommand != nullptr)
            {
                if (&changeEnumCommand->mEnum == &this->mEnum)
                {
                    changeEnumCommand->mNewEnum = this->mNewEnum;
                    return true;
                }
            }

            return false;
        }


    private:
        T& mEnum;
        T mNewEnum;
        T mOldEnum;
    };
}

#endif

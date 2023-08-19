#ifndef COCOA_ENGINE_CHANGE_INT_COMMAND_2_H
#define COCOA_ENGINE_CHANGE_INT_COMMAND_2_H
#include "externalLibs.h"
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeInt2Command final : public ICommand
    {
    public:
        ChangeInt2Command(glm::ivec2& originalVector, glm::ivec2& newVector)
            : mVector(originalVector), mNewVector(newVector), mOldVector(glm::vec2())
        {
        }

        void execute() override
        {
            mOldVector = glm::ivec2(mVector);
            mVector.x = mNewVector.x;
            mVector.y = mNewVector.y;
        }

        void undo() override
        {
            mVector.x = mOldVector.x;
            mVector.y = mOldVector.y;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeInt2Command* changeInt2Command = dynamic_cast<ChangeInt2Command*>(other);
            if (changeInt2Command != nullptr)
            {
                if (&changeInt2Command->mVector == &this->mVector)
                {
                    changeInt2Command->mNewVector = this->mNewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::ivec2& mVector;
        glm::ivec2 mNewVector;
        glm::ivec2 mOldVector;
    };
}

#endif

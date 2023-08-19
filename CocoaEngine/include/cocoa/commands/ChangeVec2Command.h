#ifndef COCOA_ENGINE_CHANGE_VEC2_COMMAND_H
#define COCOA_ENGINE_CHANGE_VEC2_COMMAND_H
#include "externalLibs.h"
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeVec2Command final : public ICommand
    {
    public:
        ChangeVec2Command(glm::vec2& originalVector, glm::vec2& newVector)
            : mVector(originalVector), mNewVector(newVector), mOldVector(glm::vec2())
        {
        }

        void execute() override
        {
            mOldVector = glm::vec2(mVector);
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
            ChangeVec2Command* changeVec2Command = dynamic_cast<ChangeVec2Command*>(other);
            if (changeVec2Command != nullptr)
            {
                if (&changeVec2Command->mVector == &this->mVector)
                {
                    changeVec2Command->mNewVector = this->mNewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec2& mVector;
        glm::vec2 mNewVector;
        glm::vec2 mOldVector;
    };
}

#endif

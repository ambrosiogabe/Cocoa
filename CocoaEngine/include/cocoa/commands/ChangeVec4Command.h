#ifndef COCOA_ENGINE_CHANGE_VEC4_COMMAND_H
#define COCOA_ENGINE_CHANGE_VEC4_COMMAND_H
#include "externalLibs.h"
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeVec4Command final : public ICommand
    {
    public:
        ChangeVec4Command(glm::vec4& originalVector, glm::vec4& newVector)
            : mVector(originalVector), mNewVector(newVector), mOldVector(glm::vec4())
        {
        }

        void execute() override
        {
            mOldVector = glm::vec4(mVector);
            mVector.x = mNewVector.x;
            mVector.y = mNewVector.y;
            mVector.z = mNewVector.z;
            mVector.w = mNewVector.w;
        }

        void undo() override
        {
            mVector.x = mOldVector.x;
            mVector.y = mOldVector.y;
            mVector.z = mOldVector.z;
            mVector.w = mOldVector.w;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeVec4Command* changeVec4Command = dynamic_cast<ChangeVec4Command*>(other);
            if (changeVec4Command != nullptr)
            {
                if (&changeVec4Command->mVector == &this->mVector)
                {
                    changeVec4Command->mNewVector = this->mNewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec4& mVector;
        glm::vec4 mNewVector;
        glm::vec4 mOldVector;
    };
}

#endif

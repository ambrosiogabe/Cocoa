#ifndef COCOA_ENGINE_CHANGE_VEC3_COMMAND_H
#define COCOA_ENGINE_CHANGE_VEC3_COMMAND_H
#include "externalLibs.h"
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeVec3Command final : public ICommand
    {
    public:
        ChangeVec3Command(glm::vec3& originalVector, glm::vec3& newVector)
            : mVector(originalVector), mNewVector(newVector), mOldVector(glm::vec3())
        {
        }

        void execute() override
        {
            mOldVector = glm::vec3(mVector);
            mVector.x = mNewVector.x;
            mVector.y = mNewVector.y;
            mVector.z = mNewVector.z;
        }

        void undo() override
        {
            mVector.x = mOldVector.x;
            mVector.y = mOldVector.y;
            mVector.z = mOldVector.z;
        }

        bool mergeWith(ICommand* other) override
        {
            ChangeVec3Command* changeVec3Command = dynamic_cast<ChangeVec3Command*>(other);
            if (changeVec3Command != nullptr)
            {
                if (&changeVec3Command->mVector == &this->mVector)
                {
                    changeVec3Command->mNewVector = this->mNewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec3& mVector;
        glm::vec3 mNewVector;
        glm::vec3 mOldVector;
    };
}

#endif

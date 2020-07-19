#pragma once
#include "externalLibs.h"

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

namespace Jade
{
    class ChangeVec3Command : public ICommand
    {
    public:
        ChangeVec3Command(glm::vec3& originalVector, glm::vec3& newVector)
            : m_Vector(originalVector), m_NewVector(newVector), m_OldVector(glm::vec3())
        {
        }

        virtual void execute() override
        {
            m_OldVector = glm::vec3(m_Vector);
            m_Vector.x = m_NewVector.x;
            m_Vector.y = m_NewVector.y;
            m_Vector.z = m_NewVector.z;
        }

        virtual void undo() override
        {
            m_Vector.x = m_OldVector.x;
            m_Vector.y = m_OldVector.y;
            m_Vector.z = m_OldVector.z;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeVec3Command* changeVec3Command = dynamic_cast<ChangeVec3Command*>(other);
            if (changeVec3Command != nullptr)
            {
                if (&changeVec3Command->m_Vector == &this->m_Vector)
                {
                    changeVec3Command->m_NewVector = this->m_NewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec3& m_Vector;
        glm::vec3 m_NewVector;
        glm::vec3 m_OldVector;
    };
}
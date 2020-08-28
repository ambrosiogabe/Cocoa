#pragma once
#include "externalLibs.h"

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

namespace Jade
{
    class JADE ChangeVec4Command : public ICommand
    {
    public:
        ChangeVec4Command(glm::vec4& originalVector, glm::vec4& newVector)
            : m_Vector(originalVector), m_NewVector(newVector), m_OldVector(glm::vec4())
        {
        }

        virtual void execute() override
        {
            m_OldVector = glm::vec4(m_Vector);
            m_Vector.x = m_NewVector.x;
            m_Vector.y = m_NewVector.y;
            m_Vector.z = m_NewVector.z;
            m_Vector.w = m_NewVector.w;
        }

        virtual void undo() override
        {
            m_Vector.x = m_OldVector.x;
            m_Vector.y = m_OldVector.y;
            m_Vector.z = m_OldVector.z;
            m_Vector.w = m_OldVector.w;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeVec4Command* changeVec4Command = dynamic_cast<ChangeVec4Command*>(other);
            if (changeVec4Command != nullptr)
            {
                if (&changeVec4Command->m_Vector == &this->m_Vector)
                {
                    changeVec4Command->m_NewVector = this->m_NewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec4& m_Vector;
        glm::vec4 m_NewVector;
        glm::vec4 m_OldVector;
    };
}
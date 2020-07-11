#pragma once

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

#include <glm/vec2.hpp>

namespace Jade
{
    class ChangeVec2Command : public ICommand
    {
    public:
        ChangeVec2Command(glm::vec2& originalVector, glm::vec2& newVector)
            : m_Vector(originalVector), m_NewVector(newVector), m_OldVector(glm::vec2())
        {
        }

        virtual void execute() override
        {
            m_OldVector = glm::vec2(m_Vector);
            m_Vector.x = m_NewVector.x;
            m_Vector.y = m_NewVector.y;
        }

        virtual void undo() override
        {
            m_Vector.x = m_OldVector.x;
            m_Vector.y = m_OldVector.y;
        }

        virtual bool mergeWith(ICommand* other) override
        {
            ChangeVec2Command* changeVec2Command = dynamic_cast<ChangeVec2Command*>(other);
            if (changeVec2Command != nullptr)
            {
                if (&changeVec2Command->m_Vector == &this->m_Vector)
                {
                    changeVec2Command->m_NewVector = this->m_NewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::vec2& m_Vector;
        glm::vec2 m_NewVector;
        glm::vec2 m_OldVector;
    };
}
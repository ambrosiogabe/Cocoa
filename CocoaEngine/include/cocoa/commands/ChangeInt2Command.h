#pragma once
#include "externalLibs.h"
#include "cocoa/commands/ICommand.h"

namespace Cocoa
{
    class COCOA ChangeInt2Command : public ICommand
    {
    public:
        ChangeInt2Command(glm::ivec2& originalVector, glm::ivec2& newVector)
            : m_Vector(originalVector), m_NewVector(newVector), m_OldVector(glm::vec2())
        {
        }

        virtual void execute() override
        {
            m_OldVector = glm::ivec2(m_Vector);
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
            ChangeInt2Command* changeInt2Command = dynamic_cast<ChangeInt2Command*>(other);
            if (changeInt2Command != nullptr)
            {
                if (&changeInt2Command->m_Vector == &this->m_Vector)
                {
                    changeInt2Command->m_NewVector = this->m_NewVector;
                    return true;
                }
            }

            return false;
        }


    private:
        glm::ivec2& m_Vector;
        glm::ivec2 m_NewVector;
        glm::ivec2 m_OldVector;
    };
}
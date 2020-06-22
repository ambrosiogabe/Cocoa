#pragma once

#include "commands/ICommand.h"
#include "components/components.h"

#include <glm/vec3.hpp>

namespace Jade {
    class MoveTransformCommand : public ICommand {
    public:
        MoveTransformCommand(Transform& transform, glm::vec3& newPos) 
            : m_Transform(transform), m_NewPos(newPos), m_OldPos(glm::vec3()) {}
        
        virtual void execute() override {
            m_OldPos = glm::vec3(m_Transform.m_Position);
            m_Transform.m_Position = m_NewPos;
        }

        virtual void undo() override {
            m_Transform.m_Position = m_OldPos;
        }

        virtual bool mergeWith(ICommand* other) override {        
            MoveTransformCommand* moveTransformCmd = dynamic_cast<MoveTransformCommand*>(other);
            if (moveTransformCmd != nullptr) {
                if (&moveTransformCmd->m_Transform == &this->m_Transform) {
                    moveTransformCmd->m_NewPos = this->m_NewPos;
                    return true;
                }
            }

            return false;
        }


    private:
        Transform& m_Transform;
        glm::vec3 m_NewPos;
        glm::vec3 m_OldPos;
    };
}
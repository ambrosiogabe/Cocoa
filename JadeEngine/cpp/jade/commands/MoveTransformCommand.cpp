#include "jade/commands/MoveTransformCommand.h"

namespace Jade {
    MoveTransformCommand::MoveTransformCommand(Transform& transform, glm::vec3& newPos) 
    : m_Transform(transform), m_NewPos(newPos), m_OldPos(glm::vec3()) {}
        
    void MoveTransformCommand::execute() {
        m_OldPos = glm::vec3(m_Transform.m_Position);
        m_Transform.m_Position = m_NewPos;
    }

    void MoveTransformCommand::undo() {
        m_Transform.m_Position = m_OldPos;
    }

    bool MoveTransformCommand::mergeWith(ICommand* other) {        
        MoveTransformCommand* moveTransformCmd = dynamic_cast<MoveTransformCommand*>(other);
        if (moveTransformCmd != nullptr) {
            if (&moveTransformCmd->m_Transform == &this->m_Transform) {
                moveTransformCmd->m_NewPos = this->m_NewPos;
                return true;
            }
        }

        return false;
    }
}
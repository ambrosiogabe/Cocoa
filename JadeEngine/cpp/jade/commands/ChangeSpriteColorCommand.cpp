#include "jade/commands/ChangeSpriteColorCommand.h"

namespace Jade {
    ChangeSpriteColorCommand::ChangeSpriteColorCommand(SpriteRenderer& renderer, glm::vec4& newColor) 
    : m_SpriteRenderer(renderer), m_NewColor(newColor), m_OldColor(glm::vec4()) {}
        
    void ChangeSpriteColorCommand::execute() {
        m_OldColor = glm::vec4(m_SpriteRenderer.m_Color);
        m_SpriteRenderer.m_Color = m_NewColor;
    }

    void ChangeSpriteColorCommand::undo() {
        m_SpriteRenderer.m_Color = m_OldColor;
    }

    bool ChangeSpriteColorCommand::mergeWith(ICommand* other) {        
        ChangeSpriteColorCommand* changeColorCmd = dynamic_cast<ChangeSpriteColorCommand*>(other);
        if (changeColorCmd != nullptr) {
            if (&changeColorCmd->m_SpriteRenderer == &this->m_SpriteRenderer) {
                changeColorCmd->m_NewColor = this->m_NewColor;
                return true;
            }
        }

        return false;
    }
}
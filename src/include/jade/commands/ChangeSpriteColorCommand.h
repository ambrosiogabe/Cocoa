#pragma once

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

#include <glm/vec3.hpp>

class ChangeSpriteColorCommand : public ICommand {
public:
    ChangeSpriteColorCommand(SpriteRenderer& renderer, glm::vec4& newColor) 
        : m_SpriteRenderer(renderer), m_NewColor(newColor), m_OldColor(glm::vec4()) {}
    
    virtual void execute() override {
        m_OldColor = glm::vec4(m_SpriteRenderer.m_Color);
        m_SpriteRenderer.m_Color = m_NewColor;
    }

    virtual void undo() override {
        m_SpriteRenderer.m_Color = m_OldColor;
    }

    virtual bool mergeWith(ICommand* other) override {        
        ChangeSpriteColorCommand* changeColorCmd = dynamic_cast<ChangeSpriteColorCommand*>(other);
        if (changeColorCmd != nullptr) {
            if (&changeColorCmd->m_SpriteRenderer == &this->m_SpriteRenderer) {
                changeColorCmd->m_NewColor = this->m_NewColor;
                return true;
            }
        }

        return false;
    }


private:
    SpriteRenderer& m_SpriteRenderer;
    glm::vec4 m_NewColor;
    glm::vec4 m_OldColor;
};
#pragma once

#include "commands/ICommand.h"
#include "components/components.h"

#include <glm/vec3.hpp>

namespace Jade {
    class ChangeSpriteColorCommand : public ICommand {
    public:
        ChangeSpriteColorCommand(SpriteRenderer& renderer, glm::vec4& newColor);
        
        virtual void execute() override;
        virtual void undo() override;
        virtual bool mergeWith(ICommand* other) override;


    private:
        SpriteRenderer& m_SpriteRenderer;
        glm::vec4 m_NewColor;
        glm::vec4 m_OldColor;
    };
}
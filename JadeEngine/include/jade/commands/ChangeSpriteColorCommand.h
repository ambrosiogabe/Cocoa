#pragma once

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

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
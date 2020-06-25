#pragma once

#include "jade/commands/ICommand.h"
#include "jade/components/components.h"

#include <glm/vec3.hpp>

namespace Jade {
    class MoveTransformCommand : public ICommand {
    public:
        MoveTransformCommand(Transform& transform, glm::vec3& newPos);
        
        virtual void execute() override;
        virtual void undo() override;
        virtual bool mergeWith(ICommand* other) override;


    private:
        Transform& m_Transform;
        glm::vec3 m_NewPos;
        glm::vec3 m_OldPos;
    };
}
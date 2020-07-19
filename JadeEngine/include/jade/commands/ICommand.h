#pragma once
#include "externalLibs.h"

#include "jade/core/Core.h"

namespace Jade {
    class ICommand {
    public:
        virtual ~ICommand() {}
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual bool mergeWith(ICommand* other) = 0;

        void SetNoMerge() { m_CanMerge = false; }
        bool CanMerge() const { return m_CanMerge; }

    private:
        static int64 m_Id;

    protected:
        bool m_CanMerge = true;
    };
}

// These includes are for ease of use from other classes
// If you wish to have all commands simply include ICommand.h
#include "jade/commands/CommandHistory.h"
#include "jade/commands/ChangeFloatCommand.h"
#include "jade/commands/ChangeVec2Command.h"
#include "jade/commands/ChangeVec3Command.h"
#include "jade/commands/ChangeVec4Command.h"
#include "jade/commands/ChangeUint8Command.h"
#include "jade/commands/ChangeEnumCommand.h"
#ifndef COCOA_ENGINE_ICOMMAND_H
#define COCOA_ENGINE_ICOMMAND_H
#include "externalLibs.h"

#include "cocoa/core/Core.h"

namespace Cocoa {
    class COCOA ICommand {
    public:
        virtual ~ICommand() {}
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual bool mergeWith(ICommand* other) = 0;

        void setNoMerge() { mCanMerge = false; }
        bool canMerge() const { return mCanMerge; }

    private:
        static int64 mId;

    protected:
        bool mCanMerge = true;
    };
}

// These includes are for ease of use from other classes
// If you wish to have all commands simply include ICommand.h
#include "cocoa/commands/CommandHistory.h"
#include "cocoa/commands/ChangeFloatCommand.h"
#include "cocoa/commands/ChangeVec2Command.h"
#include "cocoa/commands/ChangeVec3Command.h"
#include "cocoa/commands/ChangeVec4Command.h"
#include "cocoa/commands/ChangeInt2Command.h"
#include "cocoa/commands/ChangeIntCommand.h"
#include "cocoa/commands/ChangeUint8Command.h"
#include "cocoa/commands/ChangeEnumCommand.h"

#endif

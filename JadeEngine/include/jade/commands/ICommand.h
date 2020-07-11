#pragma once

#include "jade/core/Core.h"

#include <typeinfo>

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
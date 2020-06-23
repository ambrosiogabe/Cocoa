#pragma once

#include "core/Core.h"

#include <typeinfo>

namespace Jade {
    class ICommand {
    public:
        virtual ~ICommand() {}
        virtual void execute() = 0;
        virtual void undo() = 0;
        virtual bool mergeWith(ICommand* other) = 0;

        void SetNoMerge();
        bool CanMerge() const;

        template<class T>
        static int64 ID();

        template<typename T>
        static int64 ID(T object);

    private:
        static int64 m_Id;

    protected:
        bool m_CanMerge = true;
    };
}
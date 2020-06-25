#include "jade/commands/ICommand.h"

namespace Jade {
    int64 ICommand::m_Id = 0;

    void ICommand::SetNoMerge() {
        m_CanMerge = false;
    }

    bool ICommand::CanMerge() const {
        return m_CanMerge;
    }

    template<class T>
    static int64 ICommand::ID() {
        static bool generatedId = false;
        if (!generatedId) {
            m_Id = typeid(T).hash_code();
        }   
        return m_Id;
    }

    template<typename T>
    static int64 ICommand::ID(T object) {
        return ICommand::m_Id;
    }
}
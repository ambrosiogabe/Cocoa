#pragma once

#include "core/Core.h"

#include <typeinfo>

class ICommand {
public:
    virtual ~ICommand() {}
    virtual void execute() = 0;
    virtual void undo() = 0;
    virtual bool mergeWith(ICommand* other) = 0;

    void SetNoMerge() {
        m_CanMerge = false;
    }

    bool CanMerge() const {
        return m_CanMerge;
    }

    template<class T>
    static int64 ID() {
        static bool generatedId = false;
        if (!generatedId) {
            m_Id = typeid(T).hash_code();
        }   
        return m_Id;
    }

    template<typename T>
    static int64 ID(T object) {
        return ICommand::m_Id;
    }

private:
    static int64 m_Id;

protected:
    bool m_CanMerge = true;
};

int64 ICommand::m_Id = 0;
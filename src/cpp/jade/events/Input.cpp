#include "jade/events/Input.h"
#include "jade/Jade.h"

Input* Input::m_Instance = nullptr;

void Input::KeyCallback(int key, int scancode, int action, int mods) {
    Input* input = Input::Get();
    input->m_KeyPressed[key] = (action == JADE_PRESS || action == JADE_REPEAT) ? true : false;
}

bool Input::KeyPressed(int keyCode) {
    return Input::Get()->m_KeyPressed[keyCode];
}

Input* Input::Get() {
    if (Input::m_Instance == nullptr) {
        Input::m_Instance = new Input();
    }

    return Input::m_Instance;
}
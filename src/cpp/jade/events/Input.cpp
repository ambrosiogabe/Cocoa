#include "jade/events/Input.h"
#include "jade/Jade.h"

Input* Input::m_Instance = nullptr;

void Input::KeyCallback(int key, int scancode, int action, int mods) {
    Input* input = Input::Get();
    input->m_KeyPressed[key] = (action == JADE_PRESS || action == JADE_REPEAT) ? true : false;
}

bool Input::KeyPressed(int keyCode) {
    if (keyCode >= 0 && keyCode < 350) {
        return Input::Get()->m_KeyPressed[keyCode];
    }
    return false;
}

void Input::MouseButtonCallback(int button, int action, int mods) {
    Input* input = Input::Get();
    input->m_MouseButtonPressed[button] = action == JADE_PRESS ? true : false;
}

bool Input::MouseButtonPressed(int mouseButton) {
    if (mouseButton >= 0 && mouseButton < 3) {
        return Input::Get()->m_MouseButtonPressed[mouseButton];
    }
    return false;
}

Input* Input::Get() {
    if (Input::m_Instance == nullptr) {
        Input::m_Instance = new Input();
    }

    return Input::m_Instance;
}
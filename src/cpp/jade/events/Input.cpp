#include "jade/events/Input.h"
#include "jade/core/Jade.h"

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

void Input::CursorCallback(double xpos, double ypos) {
    Input* input = Input::Get();
    input->m_XPos = (float)xpos;
    input->m_YPos = (float)ypos;
}

float Input::MouseX() {
    return Input::Get()->m_XPos;
}

float Input::MouseY() {
    return Input::Get()->m_YPos;
}

void Input::ScrollCallback(double xpos, double ypos) {
    Input* input = Input::Get();
    input->m_ScrollX = (float)xpos;
    input->m_ScrollY = -(float)ypos;
}

float Input::ScrollX() {
    return Input::Get()->m_ScrollX;
}

float Input::ScrollY() {
    return Input::Get()->m_ScrollY;
}

void Input::EndFrame() {
    Input* input = Input::Get();
    input->m_ScrollX = 0;
    input->m_ScrollY = 0;
}

Input* Input::Get() {
    if (Input::m_Instance == nullptr) {
        Input::m_Instance = new Input();
    }

    return Input::m_Instance;
}
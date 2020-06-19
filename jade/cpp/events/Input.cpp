#include "events/Input.h"
#include "core/Core.h"
#include "platform/JWindow.h"

#include <glm/vec4.hpp>

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

float Input::OrthoMouseX() {
    const glm::vec2& gameviewPos = JWindow::GetScene()->GetGameviewPos();
    const glm::vec2& gameviewSize = JWindow::GetScene()->GetGameviewSize();

    float currentX = MouseX() - gameviewPos.x;
    currentX = (currentX / gameviewSize.x) * 2.0f - 1.0f;
    glm::vec4 tmp = glm::vec4(currentX, 0.0f, 0.0f, 1.0f);
    tmp = JWindow::GetScene()->GetCamera()->GetOrthoInverseView() * JWindow::GetScene()->GetCamera()->GetOrthoInverseProjection() * tmp;

    return tmp.x;
}

float Input::OrthoMouseY() {
    const glm::vec2& gameviewPos = JWindow::GetScene()->GetGameviewPos();
    const glm::vec2& gameviewSize = JWindow::GetScene()->GetGameviewSize();

    float currentY = gameviewPos.y - MouseY();
    currentY = (currentY / gameviewSize.y) * 2.0f - 1.0f;
    glm::vec4 tmp = glm::vec4(0.0f, currentY, 0.0f, 1.0f);
    tmp = JWindow::GetScene()->GetCamera()->GetOrthoInverseView() * JWindow::GetScene()->GetCamera()->GetOrthoInverseProjection() * tmp;

    return tmp.y;
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
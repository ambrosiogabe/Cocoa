#include "jade/events/Input.h"
#include "jade/core/Core.h"
#include "jade/core/Application.h"

#include <glm/vec4.hpp>

namespace Jade {
    bool Input::s_KeyPressed[349] { };
    bool Input::s_MouseButtonPressed[3] { };
    float Input::s_XPos = 0.0f;
    float Input::s_YPos = 0.0f;
    float Input::s_ScrollX = 0.0f;
    float Input::s_ScrollY = 0.0f;

    void Input::KeyCallback(int key, int scancode, int action, int mods) {
        s_KeyPressed[key] = (action == JADE_PRESS || action == JADE_REPEAT) ? true : false;
    }

    bool Input::KeyPressed(int keyCode) {
        if (keyCode >= 0 && keyCode < 350) {
            return s_KeyPressed[keyCode];
        }
        return false;
    }

    void Input::MouseButtonCallback(int button, int action, int mods) {
        s_MouseButtonPressed[button] = action == JADE_PRESS ? true : false;
    }

    bool Input::MouseButtonPressed(int mouseButton) {
        if (mouseButton >= 0 && mouseButton < 3) {
            return s_MouseButtonPressed[mouseButton];
        }
        return false;
    }

    void Input::CursorCallback(double xpos, double ypos) {
        s_XPos = (float)xpos;
        s_YPos = (float)ypos;
    }

    float Input::MouseX() {
        return s_XPos;
    }

    float Input::MouseY() {
        return s_YPos;
    }

    float Input::OrthoMouseX() {
        const glm::vec2& gameviewPos = Application::Get()->GetGameViewPos();
        const glm::vec2& gameviewSize = Application::Get()->GetGameViewSize();

        float currentX = MouseX() - gameviewPos.x;
        currentX = (currentX / gameviewSize.x) * 2.0f - 1.0f;
        glm::vec4 tmp = glm::vec4(currentX, 0.0f, 0.0f, 1.0f);
        tmp = Application::Get()->GetScene()->GetCamera()->GetOrthoInverseView() * Application::Get()->GetScene()->GetCamera()->GetOrthoInverseProjection() * tmp;

        return tmp.x;
    }

    float Input::OrthoMouseY() {
        const glm::vec2& gameviewPos = Application::Get()->GetGameViewPos();
        const glm::vec2& gameviewSize = Application::Get()->GetGameViewSize();

        float currentY = gameviewPos.y - MouseY();
        currentY = (currentY / gameviewSize.y) * 2.0f - 1.0f;
        glm::vec4 tmp = glm::vec4(0.0f, currentY, 0.0f, 1.0f);
        tmp = Application::Get()->GetScene()->GetCamera()->GetOrthoInverseView() * Application::Get()->GetScene()->GetCamera()->GetOrthoInverseProjection() * tmp;

        return tmp.y;
    }

    void Input::ScrollCallback(double xpos, double ypos) {
        s_ScrollX = (float)xpos;
        s_ScrollY = -(float)ypos;
    }

    float Input::ScrollX() {
        return s_ScrollX;
    }

    float Input::ScrollY() {
        return s_ScrollY;
    }

    void Input::EndFrame() {
        s_ScrollX = 0;
        s_ScrollY = 0;
    }

    glm::vec2 Input::MousePos()
    {
        return glm::vec2{ s_XPos, s_YPos };
    }
}
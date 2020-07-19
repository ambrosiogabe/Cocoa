#pragma once
#include "externalLibs.h"

namespace Jade {
    class Input {
    public:
        static void KeyCallback(int key, int scancode, int action, int mods);
        static void CursorCallback(double xpos, double ypos);
        static void MouseButtonCallback(int button, int action, int mods);
        static void ScrollCallback(double xoffset, double yoffset);

        static bool KeyPressed(int keyCode);
        static bool MouseButtonPressed(int mouseButton);
        static float MouseX();
        static float MouseY();
        static float ScrollX();
        static float ScrollY();
        static void EndFrame();
        static glm::vec2 MousePos();

        static float OrthoMouseX();
        static float OrthoMouseY();

    private:
        Input::Input() {}

    private:
        static bool s_KeyPressed[349];
        static bool s_MouseButtonPressed[3];
        static float s_XPos, s_YPos;
        static float s_ScrollX, s_ScrollY;
    };
}
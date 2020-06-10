#pragma once

class Input {
public:
    static void KeyCallback(int key, int scancode, int action, int mods);
    static void CursorCallback(double xpos, double ypos);
    static void MouseButtonCallback(int button, int action, int mods);
    static void ScrollCallback(double xoffset, double yoffset);

    static bool KeyPressed(int keyCode);

private:
    Input::Input() {
        for (int i=0; i < 349; i++) {
            m_KeyPressed[i] = false;
        }
    }
    static Input* Input::Get();

private:
    bool m_KeyPressed[349];

    static Input* m_Instance;
};
#pragma once

#include "platform/JWindow.h"

class Win32Window : public JWindow {
public:
    Win32Window() {}
    virtual void Destroy() override;
    virtual void PollEvents() override;

    static void ShowMessage(LPCSTR message);
    static void InitWin32(HINSTANCE hInstance);

    virtual void Show() override;
    virtual void Hide() override;
    virtual void Close() override;

protected:
    virtual JWindow* _CreateWindow(int width, int height, const char* title) override;

    virtual void _Render() override;

private:
    bool m_Initialized = false;

    HINSTANCE m_HINSTANCE;
    HGLRC RC;
    HDC DC;
    HWND WND;
};
#pragma once

#include "jade/platform/JWindow.h"

class Win32Window : public JWindow {
public:
    Win32Window() {}
    virtual void Destroy() override;

    static int Create(HINSTANCE hInstance, int nCmdShow);
    static void ShowMessage(LPCSTR message);

protected:
    virtual JWindow* _Create() override;
    virtual void _Hide() override;
    virtual void _Close() override;
    virtual void _Stop() override;
    virtual void _Render() override;

private:
    HGLRC RC;
    HDC DC;
    HWND WND;
};
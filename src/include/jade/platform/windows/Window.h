#include <windows.h>

class Window {
public:
    int Create(HINSTANCE hInstance, int nCmdShow);
    void ShowMessage(LPCSTR message);
    void Render();
    void Destroy();

private:
    HGLRC RC;
    HDC DC;
    HWND WND;
};
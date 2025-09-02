// Torchade.cpp : Defines the entry point for the application.
//

#include "Window.h"
#include <sstream>

int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);
    
    try
    {
        Window wnd(800, 300, L"Torchade");

        MSG msg;
        BOOL gResult;

        int scroll = 0;

        while ((gResult = GetMessage(&msg, nullptr, 0U, 0U)) > 0)
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        if (gResult == -1)
        {
            return -1;
        }

        return msg.wParam;
    }
    catch (const TorchadeException& e)
    {
        MessageBoxA(nullptr, e.what(), e.GetType(), MB_OK | MB_ICONEXCLAMATION);
    }
    catch (const std::exception& e)
    {
        MessageBoxA(nullptr, e.what(), "Standard Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    catch (...)
    {
        MessageBoxA(nullptr, "No Details Available", "Unkown Exception", MB_OK | MB_ICONEXCLAMATION);
    }
    return -1;
}

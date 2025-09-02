#include "Window.h"
#include <sstream>
#include "Resource.h"

Window::WindowClass Window::WindowClass::wndClass;

Window::WindowClass::WindowClass() noexcept : hInst(GetModuleHandle(nullptr))
{
    WNDCLASSEXW wcex;

    wcex.cbSize = sizeof(WNDCLASSEX);

    wcex.style = CS_HREDRAW | CS_VREDRAW | CS_DBLCLKS;
    wcex.lpfnWndProc = HandleMsgSetup;
    wcex.cbClsExtra = 0;
    wcex.cbWndExtra = 0;
    wcex.hInstance = GetInstance();
    wcex.hIcon = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_TORCHADE), IMAGE_ICON, 32, 32, 0));
    wcex.hCursor = nullptr;
    wcex.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);
    wcex.lpszMenuName = nullptr;
    wcex.lpszClassName = GetName();
    wcex.hIconSm = static_cast<HICON>(LoadImage(hInst, MAKEINTRESOURCE(IDI_TORCHADE), IMAGE_ICON, 16, 16, 0));;

    RegisterClassExW(&wcex);
}

Window::WindowClass::~WindowClass()
{
    UnregisterClass(wndClassName, GetInstance());
}

const WCHAR* Window::WindowClass::GetName() noexcept
{
    return wndClassName;
}

HINSTANCE Window::WindowClass::GetInstance() noexcept
{
    return wndClass.hInst;
}

Window::Window(int width, int height, const WCHAR* name) noexcept
    :
    width(width),
    height(height)
{
    // Calculate window size
    RECT wr;
    wr.left = 100;
    wr.right = width + wr.left;
    wr.top = 100;
    wr.bottom = height + wr.top;
    if (AdjustWindowRect(&wr, WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU, FALSE) == 0)
    {
        throw TORCHWND_LAST_EXCEPT();
    }

    // Create window and get window handle
    hWnd = CreateWindow(
        WindowClass::GetName(), name,
        WS_CAPTION | WS_MINIMIZEBOX | WS_SYSMENU,
        CW_USEDEFAULT, CW_USEDEFAULT, wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, WindowClass::GetInstance(), this
    );

    if (hWnd == nullptr)
    {
        throw TORCHWND_LAST_EXCEPT();
    }

    ShowWindow(hWnd, SW_SHOWDEFAULT);
}

Window::~Window()
{
    DestroyWindow(hWnd);
}

void Window::SetTitle(const std::string title) {
    if (SetWindowTextA(hWnd, title.c_str()) == 0) {
        throw TORCHWND_LAST_EXCEPT();
    }
}

LRESULT CALLBACK Window::HandleMsgSetup(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    // Checks to see if a window is to be created so we can store a window class pointer
    if (message == WM_NCCREATE)
    {
        const CREATESTRUCTW* const pCreate = reinterpret_cast<CREATESTRUCTW*>(lParam);
        Window* const pWnd = static_cast<Window*>(pCreate->lpCreateParams);

        // Set WinAPI-managed user data to store ptr to window class
        SetWindowLongPtr(hWnd, GWLP_USERDATA, reinterpret_cast<LONG_PTR>(pWnd));
        // Set message proc to normal (non-setup) handler now that setup is finished
        SetWindowLongPtr(hWnd, GWLP_WNDPROC, reinterpret_cast<LONG_PTR>(&Window::HandleMsgThunk));
        // Forward message to window class handler
        return pWnd->WndProc(hWnd, message, wParam, lParam);
    }
    return DefWindowProc(hWnd, message, wParam, lParam);
}

LRESULT CALLBACK Window::HandleMsgThunk(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    Window* const pWnd = reinterpret_cast<Window*>(GetWindowLongPtr(hWnd, GWLP_USERDATA));
    return pWnd->WndProc(hWnd, message, wParam, lParam);
}

LRESULT Window::WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) noexcept
{
    switch (message) {
        case WM_CLOSE:
        {
            PostQuitMessage(0);
            return 0; // We destroy our window from our destructor
        }

        // Keyboard Messages
        case WM_KEYDOWN:
        case WM_SYSKEYDOWN:
        {
            if (!(lParam & 0x40000000) || kbd.AutorepeatIsEnabled()) {
                kbd.OnKeyPressed(static_cast<unsigned char>(wParam));
            }
            break;
        }
        case WM_KEYUP:
        case WM_SYSKEYUP:
        {
            kbd.OnKeyReleased(static_cast<unsigned char>(wParam));
            break;
        }
        case WM_CHAR:
        {
            kbd.OnChar(static_cast<unsigned char>(wParam));
            break;
        }
        case WM_KILLFOCUS:
        {
            kbd.ClearState();
            break;
        }

        // Mouse Messages
        case WM_MOUSEMOVE:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            if (pt.x >= 0 && pt.x < width && pt.y >= 0 && pt.y < height) {
                mouse.OnMouseMove(pt.x, pt.y);
                if (!mouse.IsInWindow()) {
                    SetCapture(hWnd);
                    mouse.OnMouseEnter();
                }
            }
            else {
                if (mouse.LeftIsPressed() || mouse.RightIsPressed()) {
                    mouse.OnMouseMove(pt.x, pt.y);
                }
                else {
                    ReleaseCapture();
                    mouse.OnMouseLeave();
                }
            }
            break;
        }
        case WM_LBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            mouse.OnLeftPressed(pt.x, pt.y);
            break;
        }
        case WM_LBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            mouse.OnLeftReleased(pt.x, pt.y);
            break;
        }
        case WM_RBUTTONDOWN:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            mouse.OnRightPressed(pt.x, pt.y);
            break;
        }
        case WM_RBUTTONUP:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            mouse.OnRightReleased(pt.x, pt.y);
            break;
        }
        case WM_MOUSEWHEEL:
        {
            const POINTS pt = MAKEPOINTS(lParam);
            const int delta = GET_WHEEL_DELTA_WPARAM(wParam);
            mouse.OnWheelDelta(pt.x, pt.y, delta);
            break;
        }
    }

    return DefWindowProc(hWnd, message, wParam, lParam);
}

Window::Exception::Exception(int line, const char* file, HRESULT hr) noexcept
    :
    TorchadeException(line, file),
    hr(hr)
{}

const char* Window::Exception::what() const noexcept
{
    std::ostringstream oss;
    oss << GetType() << std::endl
        << "[Error Code] " << GetErrorCode() << std::endl
        << "[Description " << GetErrorString() << std::endl
        << GetOriginString();
    whatBuffer = oss.str();
    return whatBuffer.c_str();
}

const char* Window::Exception::GetType() const noexcept
{
    return "Torchade Window Exception";
}

std::string Window::Exception::TranslateErrorCode(HRESULT hr) noexcept
{
    char* pMsgBuf = nullptr;
    DWORD nMsgLen = FormatMessageA(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS,
        nullptr, hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
        reinterpret_cast<LPSTR>(&pMsgBuf), 0, nullptr
    );
    if (nMsgLen == 0)
    {
        return "Unidentified error code";
    }
    std::string errorString = pMsgBuf;
    LocalFree(pMsgBuf);
    return errorString;
}

HRESULT Window::Exception::GetErrorCode() const noexcept
{
    return hr;
}

std::string Window::Exception::GetErrorString() const noexcept
{
    return TranslateErrorCode(hr);
}
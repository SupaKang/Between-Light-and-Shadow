#include "window.hpp"
#include "renderer.hpp"
#include "input.hpp"

namespace JoseonRPG {

Window::Window(std::string_view title, int scale)
    : m_title(title), m_scale(scale) {}

Window::~Window() {
    if (m_hdc && m_hwnd) {
        ReleaseDC(m_hwnd, m_hdc);
    }
    if (m_hwnd) {
        DestroyWindow(m_hwnd);
    }
}

bool Window::init() {
    HINSTANCE hInstance = GetModuleHandle(nullptr);

    WNDCLASSEX wc = {};
    wc.cbSize = sizeof(WNDCLASSEX);
    wc.style = CS_HREDRAW | CS_VREDRAW;
    wc.lpfnWndProc = Window::WndProc;
    wc.hInstance = hInstance;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.lpszClassName = "JoseonYokaiRPGWindow";

    RegisterClassEx(&wc);

    int clientW = SCREEN_WIDTH * m_scale;
    int clientH = SCREEN_HEIGHT * m_scale;

    RECT wr = {0, 0, clientW, clientH};
    AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);

    m_hwnd = CreateWindowEx(
        0,
        wc.lpszClassName,
        m_title.c_str(),
        WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX | WS_VISIBLE,
        CW_USEDEFAULT, CW_USEDEFAULT,
        wr.right - wr.left, wr.bottom - wr.top,
        nullptr, nullptr, hInstance, this
    );

    if (!m_hwnd) return false;

    m_hdc = GetDC(m_hwnd);

    // Bitmap info for StretchDIBits (Top-down 32-bit ARGB DIB)
    m_bmi.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
    m_bmi.bmiHeader.biWidth = SCREEN_WIDTH;
    m_bmi.bmiHeader.biHeight = -SCREEN_HEIGHT; // Negative for top-down
    m_bmi.bmiHeader.biPlanes = 1;
    m_bmi.bmiHeader.biBitCount = 32;
    m_bmi.bmiHeader.biCompression = BI_RGB;

    m_isOpen = true;
    return true;
}

bool Window::processMessages() {
    MSG msg;
    while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_QUIT) {
            m_isOpen = false;
            return false;
        }
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
    return m_isOpen;
}

void Window::present(const Renderer& renderer) {
    if (!m_hdc) return;

    RECT clientRect;
    GetClientRect(m_hwnd, &clientRect);
    int clientW = clientRect.right - clientRect.left;
    int clientH = clientRect.bottom - clientRect.top;

    StretchDIBits(
        m_hdc,
        0, 0, clientW, clientH,
        0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,
        renderer.getFramebuffer(),
        &m_bmi,
        DIB_RGB_COLORS,
        SRCCOPY
    );
}

LRESULT CALLBACK Window::WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
    switch (msg) {
        case WM_KEYDOWN:
        case WM_KEYUP: {
            bool down = (msg == WM_KEYDOWN);
            switch (wParam) {
                case VK_UP:
                case 'W':
                    Input::setKeyState(Key::Up, down);
                    break;
                case VK_DOWN:
                case 'S':
                    Input::setKeyState(Key::Down, down);
                    break;
                case VK_LEFT:
                case 'A':
                    Input::setKeyState(Key::Left, down);
                    break;
                case VK_RIGHT:
                case 'D':
                    Input::setKeyState(Key::Right, down);
                    break;
                case 'Z':
                case VK_SPACE:
                case VK_RETURN:
                    Input::setKeyState(Key::ActionA, down);
                    break;
                case 'X':
                case VK_ESCAPE:
                    Input::setKeyState(Key::ActionB, down);
                    break;
                case 'C':
                case VK_TAB:
                    Input::setKeyState(Key::ActionC, down);
                    break;
                case VK_F1:
                    Input::setKeyState(Key::Debug, down);
                    break;
            }
            return 0;
        }
        case WM_DESTROY:
            PostQuitMessage(0);
            return 0;
    }
    return DefWindowProc(hwnd, msg, wParam, lParam);
}

} // namespace JoseonRPG

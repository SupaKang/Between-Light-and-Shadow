#include "window.hpp"
#include "renderer.hpp"
#include "input.hpp"
#include <algorithm>

namespace JoseonRPG {

Window* Window::s_instance = nullptr;

Window::Window(std::string_view title, int scale)
    : m_title(title), m_scale(scale) {
    s_instance = this;
}

Window::~Window() {
    if (s_instance == this) {
        s_instance = nullptr;
    }
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

void Window::setScale(int scale) {
    if (scale < 1 || scale > 6) return;
    m_scale = scale;
    if (m_isFullscreen) {
        setFullscreen(false);
    }
    if (m_hwnd) {
        int clientW = SCREEN_WIDTH * m_scale;
        int clientH = SCREEN_HEIGHT * m_scale;
        RECT wr = {0, 0, clientW, clientH};
        AdjustWindowRect(&wr, WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX, FALSE);
        SetWindowPos(m_hwnd, nullptr, 0, 0, wr.right - wr.left, wr.bottom - wr.top,
                     SWP_NOMOVE | SWP_NOZORDER | SWP_NOACTIVATE | SWP_FRAMECHANGED);
    }
}

void Window::toggleFullscreen() {
    setFullscreen(!m_isFullscreen);
}

void Window::setFullscreen(bool enable) {
    if (m_isFullscreen == enable || !m_hwnd) return;
    m_isFullscreen = enable;

    DWORD style = GetWindowLong(m_hwnd, GWL_STYLE);

    if (m_isFullscreen) {
        GetWindowPlacement(m_hwnd, &m_prevPlacement);
        SetWindowLong(m_hwnd, GWL_STYLE, style & ~(WS_OVERLAPPEDWINDOW | WS_CAPTION | WS_THICKFRAME));

        HMONITOR hMon = MonitorFromWindow(m_hwnd, MONITOR_DEFAULTTOPRIMARY);
        MONITORINFO mi = {sizeof(mi)};
        GetMonitorInfo(hMon, &mi);

        SetWindowPos(m_hwnd, HWND_TOP,
                     mi.rcMonitor.left, mi.rcMonitor.top,
                     mi.rcMonitor.right - mi.rcMonitor.left,
                     mi.rcMonitor.bottom - mi.rcMonitor.top,
                     SWP_NOOWNERZORDER | SWP_FRAMECHANGED | SWP_SHOWWINDOW);
    } else {
        SetWindowLong(m_hwnd, GWL_STYLE, style | WS_OVERLAPPED | WS_CAPTION | WS_SYSMENU | WS_MINIMIZEBOX);
        SetWindowPlacement(m_hwnd, &m_prevPlacement);
        SetWindowPos(m_hwnd, nullptr, 0, 0, 0, 0,
                     SWP_NOMOVE | SWP_NOSIZE | SWP_NOZORDER | SWP_NOOWNERZORDER | SWP_FRAMECHANGED);
    }
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

    if (clientW <= 0 || clientH <= 0) return;

    // Pixel-Perfect Integer Scaling
    int scaleX = clientW / SCREEN_WIDTH;
    int scaleY = clientH / SCREEN_HEIGHT;
    int scale = std::max(1, std::min(scaleX, scaleY));

    int dstW = SCREEN_WIDTH * scale;
    int dstH = SCREEN_HEIGHT * scale;
    int dstX = (clientW - dstW) / 2;
    int dstY = (clientH - dstH) / 2;

    // Clear pillarbox / letterbox borders if present
    if (dstX > 0) {
        RECT leftBar = {0, 0, dstX, clientH};
        RECT rightBar = {dstX + dstW, 0, clientW, clientH};
        FillRect(m_hdc, &leftBar, (HBRUSH)GetStockObject(BLACK_BRUSH));
        FillRect(m_hdc, &rightBar, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }
    if (dstY > 0) {
        RECT topBar = {0, 0, clientW, dstY};
        RECT bottomBar = {0, dstY + dstH, clientW, clientH};
        FillRect(m_hdc, &topBar, (HBRUSH)GetStockObject(BLACK_BRUSH));
        FillRect(m_hdc, &bottomBar, (HBRUSH)GetStockObject(BLACK_BRUSH));
    }

    StretchDIBits(
        m_hdc,
        dstX, dstY, dstW, dstH,
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
                    Input::setKeyState(Key::ActionA, down);
                    break;
                case VK_RETURN:
                    if (down && (GetKeyState(VK_MENU) & 0x8000)) {
                        if (Window::s_instance) Window::s_instance->toggleFullscreen();
                    } else {
                        Input::setKeyState(Key::ActionA, down);
                    }
                    break;
                case 'X':
                case VK_ESCAPE:
                    Input::setKeyState(Key::ActionB, down);
                    break;
                case 'C':
                case VK_TAB:
                    Input::setKeyState(Key::ActionC, down);
                    break;
                case 'V':
                case 'P':
                    Input::setKeyState(Key::ActionD, down);
                    break;
                case 'Q':
                case 'L':
                    Input::setKeyState(Key::ActionE, down);
                    break;
                case 'O':
                case 'M':
                case VK_F2:
                    Input::setKeyState(Key::Menu, down);
                    break;
                case VK_F5:
                    Input::setKeyState(Key::Save, down);
                    break;
                case VK_F11:
                    if (down && Window::s_instance) {
                        Window::s_instance->toggleFullscreen();
                    }
                    break;
                case VK_F1:
                    Input::setKeyState(Key::Debug, down);
                    break;
                case VK_SHIFT:
                case VK_LSHIFT:
                case VK_RSHIFT:
                case 'B':
                    Input::setKeyState(Key::Dash, down);
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

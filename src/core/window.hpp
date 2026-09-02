#pragma once
#include "types.hpp"
#include <string_view>

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#include <windows.h>

namespace JoseonRPG {

class Renderer;

class Window {
public:
    Window(std::string_view title, int scale = 3);
    ~Window();

    bool init();
    bool processMessages();
    void present(const Renderer& renderer);
    
    bool isOpen() const { return m_isOpen; }
    void close() { m_isOpen = false; }

    HWND getHWND() const { return m_hwnd; }
    int getScale() const { return m_scale; }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    std::string m_title;
    int m_scale;
    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    BITMAPINFO m_bmi = {};
    bool m_isOpen = false;
};

} // namespace JoseonRPG

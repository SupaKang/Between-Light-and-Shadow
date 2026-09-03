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
    void setScale(int scale);

    void toggleFullscreen();
    void setFullscreen(bool enable);
    bool isFullscreen() const { return m_isFullscreen; }

    static Window* getInstance() { return s_instance; }

private:
    static LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

    std::string m_title;
    int m_scale = 3;
    HWND m_hwnd = nullptr;
    HDC m_hdc = nullptr;
    BITMAPINFO m_bmi = {};
    bool m_isOpen = false;
    bool m_isFullscreen = false;
    WINDOWPLACEMENT m_prevPlacement = {sizeof(WINDOWPLACEMENT)};

    static Window* s_instance;
};

} // namespace JoseonRPG

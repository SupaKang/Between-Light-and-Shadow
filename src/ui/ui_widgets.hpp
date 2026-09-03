#pragma once
#include "../core/types.hpp"
#include "../core/renderer.hpp"
#include <string>
#include <vector>

namespace JoseonRPG {

class UISmoothBar {
public:
    UISmoothBar(int current = 100, int max = 100);

    void setTarget(int current, int max, bool instant = false);
    void update(float dt);
    void render(Renderer& renderer, int x, int y, int w, int h, Color fillColor, Color bgColor = Palette::DarkGray) const;

    int getCurrentValue() const { return static_cast<int>(m_displayVal); }
    int getTargetValue() const { return m_targetVal; }
    int getMaxValue() const { return m_maxVal; }
    bool isAnimating() const;

private:
    float m_displayVal = 100.0f;
    int m_targetVal = 100;
    int m_maxVal = 100;
    float m_slideSpeed = 60.0f; // Value points per second
};

class UIMenuList {
public:
    UIMenuList() = default;

    void setItems(const std::vector<std::string>& items);
    void addItem(const std::string& item);
    void clear();

    bool handleInput(); // Returns true if cursor moved or confirmed/cancelled
    int getSelectedIndex() const { return m_selectedIndex; }
    void setSelectedIndex(int idx);

    void render(Renderer& renderer, int x, int y, int w, int itemHeight = 12, int maxVisibleItems = 4) const;

    size_t size() const { return m_items.size(); }
    const std::string& getSelectedItem() const;

private:
    std::vector<std::string> m_items;
    int m_selectedIndex = 0;
};

class UIBanner {
public:
    static void show(const std::string& message, float duration = 2.5f, Color textColor = Palette::Yellow);
    static void update(float dt);
    static void render(Renderer& renderer);

private:
    static std::string s_message;
    static float s_timer;
    static Color s_color;
};

} // namespace JoseonRPG

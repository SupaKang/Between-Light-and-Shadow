#include "ui_widgets.hpp"
#include "font_renderer.hpp"
#include "../core/input.hpp"
#include <algorithm>
#include <cmath>

namespace JoseonRPG {

// --- UISmoothBar Implementation ---
UISmoothBar::UISmoothBar(int current, int max)
    : m_displayVal(static_cast<float>(current)), m_targetVal(current), m_maxVal(max) {}

void UISmoothBar::setTarget(int current, int max, bool instant) {
    m_targetVal = current;
    m_maxVal = std::max(1, max);
    if (instant) {
        m_displayVal = static_cast<float>(current);
    }
}

void UISmoothBar::update(float dt) {
    float diff = static_cast<float>(m_targetVal) - m_displayVal;
    if (std::abs(diff) < 0.05f) {
        m_displayVal = static_cast<float>(m_targetVal);
        return;
    }

    float step = m_slideSpeed * dt;
    if (std::abs(diff) <= step) {
        m_displayVal = static_cast<float>(m_targetVal);
    } else {
        if (diff > 0.0f) m_displayVal += step;
        else m_displayVal -= step;
    }
}

bool UISmoothBar::isAnimating() const {
    return std::abs(static_cast<float>(m_targetVal) - m_displayVal) > 0.1f;
}

void UISmoothBar::render(Renderer& renderer, int x, int y, int w, int h, Color fillColor, Color bgColor) const {
    int cur = std::clamp(static_cast<int>(std::round(m_displayVal)), 0, m_maxVal);
    renderer.drawHealthBar(x, y, w, h, cur, m_maxVal, fillColor, bgColor);
}

// --- UIMenuList Implementation ---
void UIMenuList::setItems(const std::vector<std::string>& items) {
    m_items = items;
    m_selectedIndex = 0;
}

void UIMenuList::addItem(const std::string& item) {
    m_items.push_back(item);
}

void UIMenuList::clear() {
    m_items.clear();
    m_selectedIndex = 0;
}

void UIMenuList::setSelectedIndex(int idx) {
    if (m_items.empty()) {
        m_selectedIndex = 0;
    } else {
        m_selectedIndex = std::clamp(idx, 0, static_cast<int>(m_items.size()) - 1);
    }
}

const std::string& UIMenuList::getSelectedItem() const {
    static const std::string empty = "";
    if (m_items.empty() || m_selectedIndex < 0 || m_selectedIndex >= static_cast<int>(m_items.size())) {
        return empty;
    }
    return m_items[m_selectedIndex];
}

bool UIMenuList::handleInput() {
    if (m_items.empty()) return false;

    if (Input::isRepeated(Key::Up)) {
        if (m_selectedIndex > 0) {
            m_selectedIndex--;
            return true;
        }
    }
    if (Input::isRepeated(Key::Down)) {
        if (m_selectedIndex + 1 < static_cast<int>(m_items.size())) {
            m_selectedIndex++;
            return true;
        }
    }
    return false;
}

void UIMenuList::render(Renderer& renderer, int x, int y, int w, int itemHeight, int maxVisibleItems) const {
    if (m_items.empty()) return;

    int total = static_cast<int>(m_items.size());
    int visibleCount = std::min(total, maxVisibleItems);

    // Scroll calculation to keep selected index visible
    int scrollOffset = 0;
    if (m_selectedIndex >= visibleCount) {
        scrollOffset = m_selectedIndex - visibleCount + 1;
    }
    if (scrollOffset + visibleCount > total) {
        scrollOffset = total - visibleCount;
    }
    scrollOffset = std::max(0, scrollOffset);

    for (int i = 0; i < visibleCount; ++i) {
        int itemIdx = scrollOffset + i;
        if (itemIdx >= total) break;

        int iy = y + i * itemHeight;
        bool isSelected = (itemIdx == m_selectedIndex);

        if (isSelected) {
            renderer.fillRect(x, iy - 1, w, itemHeight - 1, Color(40, 48, 64));
            FontRenderer::drawText(renderer, x + 2, iy, ">", Palette::Yellow);
        }

        Color txtColor = isSelected ? Palette::Yellow : Palette::White;
        FontRenderer::drawText(renderer, x + 10, iy, m_items[itemIdx], txtColor);
    }
}

// --- UIBanner Implementation ---
std::string UIBanner::s_message = "";
float UIBanner::s_timer = 0.0f;
Color UIBanner::s_color = Palette::Yellow;

void UIBanner::show(const std::string& message, float duration, Color textColor) {
    s_message = message;
    s_timer = duration;
    s_color = textColor;
}

void UIBanner::update(float dt) {
    if (s_timer > 0.0f) {
        s_timer -= dt;
        if (s_timer <= 0.0f) {
            s_message.clear();
        }
    }
}

void UIBanner::render(Renderer& renderer) {
    if (s_message.empty()) return;

    renderer.fillRect(0, SCREEN_HEIGHT - 13, SCREEN_WIDTH, 13, Color(16, 20, 26, 235));
    renderer.drawLine(0, SCREEN_HEIGHT - 13, SCREEN_WIDTH, SCREEN_HEIGHT - 13, Palette::MidGray);
    FontRenderer::drawText(renderer, 4, SCREEN_HEIGHT - 10, s_message, s_color);
}

} // namespace JoseonRPG

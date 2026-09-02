#include "input.hpp"

namespace JoseonRPG {

uint32_t Input::s_currentKeys = 0;
uint32_t Input::s_previousKeys = 0;

void Input::update() {
    s_previousKeys = s_currentKeys;
}

void Input::setKeyState(Key key, bool down) {
    uint32_t bit = static_cast<uint32_t>(key);
    if (down) {
        s_currentKeys |= bit;
    } else {
        s_currentKeys &= ~bit;
    }
}

bool Input::isDown(Key key) {
    return (s_currentKeys & static_cast<uint32_t>(key)) != 0;
}

bool Input::isPressed(Key key) {
    uint32_t bit = static_cast<uint32_t>(key);
    return ((s_currentKeys & bit) != 0) && ((s_previousKeys & bit) == 0);
}

bool Input::isReleased(Key key) {
    uint32_t bit = static_cast<uint32_t>(key);
    return ((s_currentKeys & bit) == 0) && ((s_previousKeys & bit) != 0);
}

} // namespace JoseonRPG

#include "input.hpp"

namespace JoseonRPG {

uint32_t Input::s_currentKeys = 0;
uint32_t Input::s_previousKeys = 0;
std::array<float, 16> Input::s_keyHoldTimers = {};
std::array<float, 16> Input::s_keyRepeatTimers = {};

int Input::getKeyIndex(Key key) {
    uint32_t val = static_cast<uint32_t>(key);
    for (int i = 0; i < 16; ++i) {
        if ((val & (1u << i)) != 0) {
            return i;
        }
    }
    return -1;
}

void Input::update(float dt) {
    s_previousKeys = s_currentKeys;

    for (int i = 0; i < 16; ++i) {
        uint32_t mask = (1u << i);
        if ((s_currentKeys & mask) != 0) {
            s_keyHoldTimers[i] += dt;
            s_keyRepeatTimers[i] += dt;
        } else {
            s_keyHoldTimers[i] = 0.0f;
            s_keyRepeatTimers[i] = 0.0f;
        }
    }
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

bool Input::isRepeated(Key key, float initialDelay, float repeatInterval) {
    if (isPressed(key)) {
        int idx = getKeyIndex(key);
        if (idx >= 0) {
            s_keyRepeatTimers[idx] = 0.0f;
        }
        return true;
    }

    if (isDown(key)) {
        int idx = getKeyIndex(key);
        if (idx >= 0) {
            float hold = s_keyHoldTimers[idx];
            if (hold >= initialDelay) {
                float repeat = s_keyRepeatTimers[idx];
                if (repeat >= repeatInterval) {
                    s_keyRepeatTimers[idx] = 0.0f;
                    return true;
                }
            }
        }
    }

    return false;
}

float Input::getHoldDuration(Key key) {
    int idx = getKeyIndex(key);
    if (idx >= 0) {
        return s_keyHoldTimers[idx];
    }
    return 0.0f;
}

void Input::reset() {
    s_currentKeys = 0;
    s_previousKeys = 0;
    s_keyHoldTimers.fill(0.0f);
    s_keyRepeatTimers.fill(0.0f);
}

} // namespace JoseonRPG

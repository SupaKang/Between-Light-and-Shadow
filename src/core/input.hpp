#pragma once
#include <cstdint>

namespace JoseonRPG {

enum class Key : uint32_t {
    None    = 0,
    Up      = 1 << 0,
    Down    = 1 << 1,
    Left    = 1 << 2,
    Right   = 1 << 3,
    ActionA = 1 << 4, // Z, Space, Enter (Confirm / Talk)
    ActionB = 1 << 5, // X, Escape (Cancel / Codex)
    ActionC = 1 << 6, // C, Tab (Artifact Inventory)
    ActionD = 1 << 7, // V, P (Party Management)
    ActionE = 1 << 8, // Q, L (Quest Log)
    Debug   = 1 << 9  // F1, D (Tech Info toggle)
};

inline Key operator|(Key a, Key b) {
    return static_cast<Key>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Key operator&(Key a, Key b) {
    return static_cast<Key>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

class Input {
public:
    static void update();
    static bool isDown(Key key);
    static bool isPressed(Key key);
    static bool isReleased(Key key);

    static void setKeyState(Key key, bool down);

private:
    static uint32_t s_currentKeys;
    static uint32_t s_previousKeys;
};

} // namespace JoseonRPG

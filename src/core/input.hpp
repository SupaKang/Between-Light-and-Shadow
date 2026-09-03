#pragma once
#include <cstdint>
#include <array>

namespace JoseonRPG {

enum class Key : uint32_t {
    None    = 0,
    Up      = 1 << 0,
    Down    = 1 << 1,
    Left    = 1 << 2,
    Right   = 1 << 3,
    ActionA = 1 << 4, // Z, Space, Enter (Confirm / Talk)
    ActionB = 1 << 5, // X, Escape (Cancel / Back)
    ActionC = 1 << 6, // C, Tab (Artifact Inventory)
    ActionD = 1 << 7, // V, P (Party Management)
    ActionE = 1 << 8, // Q, L (Quest Log)
    Save    = 1 << 9, // S (Quick Save)
    Debug   = 1 << 10, // F1, D (Tech Info toggle)
    Menu    = 1 << 11  // ESC, O, M (Settings Menu)
};

inline Key operator|(Key a, Key b) {
    return static_cast<Key>(static_cast<uint32_t>(a) | static_cast<uint32_t>(b));
}

inline Key operator&(Key a, Key b) {
    return static_cast<Key>(static_cast<uint32_t>(a) & static_cast<uint32_t>(b));
}

class Input {
public:
    static void update(float dt = 0.01667f);
    static bool isDown(Key key);
    static bool isPressed(Key key);
    static bool isReleased(Key key);
    
    // Key repeat for UI lists and menus (initial delay then fast repeat)
    static bool isRepeated(Key key, float initialDelay = 0.25f, float repeatInterval = 0.07f);
    static float getHoldDuration(Key key);

    static void setKeyState(Key key, bool down);
    static void reset();

private:
    static uint32_t s_currentKeys;
    static uint32_t s_previousKeys;
    static std::array<float, 16> s_keyHoldTimers;
    static std::array<float, 16> s_keyRepeatTimers;

    static int getKeyIndex(Key key);
};

} // namespace JoseonRPG

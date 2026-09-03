#pragma once
#include <cstdint>
#include <string>

namespace JoseonRPG {

enum class BgmTrack {
    None,
    Title,
    Village,
    WildBattle,
    BossBattle,
    Victory
};

enum class SfxId {
    MenuCursor,
    MenuSelect,
    MenuCancel,
    HitPhysical,
    HitMagic,
    StatusAfflict,
    CaptureThrow,
    CaptureSuccess,
    LevelUp,
    ChestOpen,
    DialogueBeep
};

class AudioEngine {
public:
    static void init();
    static void shutdown();
    static void update(float dt);

    static void playBgm(BgmTrack track);
    static void stopBgm();
    static void playSfx(SfxId sfx);

    static void setMasterVolume(float vol); // 0.0f to 1.0f
    static float getMasterVolume();
    static void setBgmVolume(float vol);
    static float getBgmVolume();
    static void setSfxVolume(float vol);
    static float getSfxVolume();

    static void toggleMute();
    static bool isMuted();

private:
    static void fillAudioBuffer(int16_t* buffer, int numSamples);
};

} // namespace JoseonRPG

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
    Victory,
    HanyangCourt,   // 한양 도성 궁중 아악
    SobaekMountain, // 소백산맥 험로 굿거리
    NamhaeReeds,    // 남해안 갈대밭 수제천 변주
    JirisanMystic,  // 지리산 신비 대나무 숲 살풀이
    EumyangSanctum  // 음양당 본거지 결전 휘모리
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
    DialogueBeep,
    ArtifactDestroy,
    TavernHeal,
    MapWarp,
    FreezeShatter,
    CapturePulse,
    CaptureBreak,
    BattleEncounter,
    Jing,           // 징: 크리티컬 및 대형 타격음
    Kkwaenggwari,   // 꽹과리: 합격기 및 공명 발동음
    Taepyeongso,    // 태평소: 승리 및 도감 등록 팡파레
    TalismanBurst   // 부적 파열음
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

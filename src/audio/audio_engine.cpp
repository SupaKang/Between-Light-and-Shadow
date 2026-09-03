#include "audio_engine.hpp"

#ifdef _WIN32
#include <windows.h>
#include <mmsystem.h>
#endif

#include <cmath>
#include <vector>
#include <thread>
#include <atomic>
#include <algorithm>

namespace JoseonRPG {

namespace {

constexpr int SAMPLE_RATE = 22050;
constexpr int BUFFER_SIZE = 1024;
constexpr int NUM_BUFFERS = 3;

// Synthesizer State
std::atomic<bool> s_running{false};
std::atomic<bool> s_muted{false};
std::atomic<float> s_masterVolume{0.6f};
std::thread s_audioThread;

#ifdef _WIN32
HWAVEOUT s_hWaveOut = nullptr;
WAVEHDR s_waveHeaders[NUM_BUFFERS];
int16_t s_pcmBuffers[NUM_BUFFERS][BUFFER_SIZE];
#endif

BgmTrack s_currentBgm = BgmTrack::None;
float s_bgmTime = 0.0f;
int s_bgmStep = 0;

// SFX State
struct ActiveSfx {
    SfxId id;
    float time = 0.0f;
    float duration = 0.0f;
    float freqStart = 440.0f;
    float freqEnd = 440.0f;
    float volume = 1.0f;
    bool active = false;
};
ActiveSfx s_activeSfx[4];

// Frequency calculation helper (MIDI note to Hz)
float midiToFreq(int note) {
    if (note <= 0) return 0.0f;
    return 440.0f * std::pow(2.0f, (note - 69) / 12.0f);
}

// Korean Pentatonic Scale (궁·상·각·치·우: C, D, E, G, A)
// MIDI Notes: C4=60, D4=62, E4=64, G4=67, A4=69, C5=72, D5=74, E5=76, G5=79, A5=81
const int kTitleBgmLead[] = {
    60, 62, 64, 67, 69, 72, 69, 67, 64, 67, 69, 72, 76, 74, 72, 69,
    64, 67, 69, 72, 69, 67, 64, 62, 60, 62, 64, 67, 64, 62, 60, 0
};
const int kTitleBgmBass[] = {
    36, 0, 36, 0, 43, 0, 43, 0, 45, 0, 45, 0, 40, 0, 40, 0,
    36, 0, 36, 0, 43, 0, 43, 0, 38, 0, 38, 0, 36, 0, 36, 0
};

const int kVillageBgmLead[] = {
    64, 67, 69, 67, 64, 62, 60, 62, 64, 64, 67, 69, 72, 69, 67, 0,
    69, 72, 74, 72, 69, 67, 64, 67, 64, 62, 60, 62, 60, 0, 60, 0
};
const int kVillageBgmBass[] = {
    36, 36, 43, 43, 36, 36, 43, 43, 45, 45, 40, 40, 36, 36, 43, 43,
    45, 45, 43, 43, 40, 40, 36, 36, 38, 38, 43, 43, 36, 0, 36, 0
};

const int kBattleBgmLead[] = {
    60, 60, 72, 60, 63, 60, 70, 60, 60, 60, 72, 60, 67, 65, 63, 62,
    60, 60, 72, 60, 63, 60, 70, 60, 67, 70, 72, 75, 72, 70, 67, 63
};
const int kBattleBgmBass[] = {
    36, 36, 36, 36, 39, 39, 41, 41, 36, 36, 36, 36, 43, 43, 41, 41,
    36, 36, 36, 36, 39, 39, 41, 41, 43, 43, 46, 46, 48, 48, 43, 39
};

const int kBossBgmLead[] = {
    57, 57, 60, 64, 69, 67, 64, 60, 57, 57, 60, 64, 72, 71, 69, 64,
    57, 60, 64, 69, 72, 76, 72, 69, 67, 64, 60, 57, 55, 57, 60, 57
};
const int kBossBgmBass[] = {
    33, 33, 33, 33, 33, 33, 33, 33, 36, 36, 36, 36, 40, 40, 40, 40,
    33, 33, 33, 33, 36, 36, 36, 36, 38, 38, 38, 38, 33, 33, 33, 33
};

// 4-Channel Synthesizer Audio Stream Generator
float s_phaseLead = 0.0f;
float s_phaseHarmony = 0.0f;
float s_phaseBass = 0.0f;
uint32_t s_noiseLfsr = 0xACE1u;

float generateNoise() {
    uint32_t bit = ((s_noiseLfsr >> 0) ^ (s_noiseLfsr >> 2) ^ (s_noiseLfsr >> 3) ^ (s_noiseLfsr >> 5)) & 1u;
    s_noiseLfsr = (s_noiseLfsr >> 1) | (bit << 15);
    return (s_noiseLfsr & 1u) ? 0.4f : -0.4f;
}

} // namespace

void AudioEngine::init() {
    if (s_running.load()) return;

#ifdef _WIN32
    WAVEFORMATEX wfx;
    wfx.wFormatTag = WAVE_FORMAT_PCM;
    wfx.nChannels = 1; // Mono
    wfx.nSamplesPerSec = SAMPLE_RATE;
    wfx.wBitsPerSample = 16;
    wfx.nBlockAlign = wfx.nChannels * (wfx.wBitsPerSample / 8);
    wfx.nAvgBytesPerSec = wfx.nSamplesPerSec * wfx.nBlockAlign;
    wfx.cbSize = 0;

    if (waveOutOpen(&s_hWaveOut, WAVE_MAPPER, &wfx, 0, 0, CALLBACK_NULL) != MMSYSERR_NOERROR) {
        s_hWaveOut = nullptr;
        return;
    }

    for (int i = 0; i < NUM_BUFFERS; ++i) {
        std::memset(&s_waveHeaders[i], 0, sizeof(WAVEHDR));
        s_waveHeaders[i].lpData = reinterpret_cast<LPSTR>(s_pcmBuffers[i]);
        s_waveHeaders[i].dwBufferLength = BUFFER_SIZE * sizeof(int16_t);
        waveOutPrepareHeader(s_hWaveOut, &s_waveHeaders[i], sizeof(WAVEHDR));
    }
#endif

    s_running.store(true);

    s_audioThread = std::thread([]() {
        int currentBuffer = 0;
        while (s_running.load()) {
#ifdef _WIN32
            if (s_hWaveOut) {
                WAVEHDR& hdr = s_waveHeaders[currentBuffer];
                if ((hdr.dwFlags & WHDR_DONE) || !(hdr.dwFlags & WHDR_PREPARED)) {
                    fillAudioBuffer(s_pcmBuffers[currentBuffer], BUFFER_SIZE);
                    hdr.dwBufferLength = BUFFER_SIZE * sizeof(int16_t);
                    waveOutWrite(s_hWaveOut, &hdr, sizeof(WAVEHDR));
                    currentBuffer = (currentBuffer + 1) % NUM_BUFFERS;
                }
            }
#endif
            std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
    });
}

void AudioEngine::shutdown() {
    if (!s_running.load()) return;
    s_running.store(false);

    if (s_audioThread.joinable()) {
        s_audioThread.join();
    }

#ifdef _WIN32
    if (s_hWaveOut) {
        waveOutReset(s_hWaveOut);
        for (int i = 0; i < NUM_BUFFERS; ++i) {
            waveOutUnprepareHeader(s_hWaveOut, &s_waveHeaders[i], sizeof(WAVEHDR));
        }
        waveOutClose(s_hWaveOut);
        s_hWaveOut = nullptr;
    }
#endif
}

void AudioEngine::playBgm(BgmTrack track) {
    if (s_currentBgm == track) return;
    s_currentBgm = track;
    s_bgmTime = 0.0f;
    s_bgmStep = 0;
}

void AudioEngine::stopBgm() {
    s_currentBgm = BgmTrack::None;
}

void AudioEngine::playSfx(SfxId sfx) {
    for (int i = 0; i < 4; ++i) {
        if (!s_activeSfx[i].active) {
            s_activeSfx[i].id = sfx;
            s_activeSfx[i].time = 0.0f;
            s_activeSfx[i].active = true;

            switch (sfx) {
                case SfxId::MenuCursor:
                    s_activeSfx[i].duration = 0.04f;
                    s_activeSfx[i].freqStart = 880.0f;
                    s_activeSfx[i].freqEnd = 880.0f;
                    s_activeSfx[i].volume = 0.4f;
                    break;
                case SfxId::MenuSelect:
                    s_activeSfx[i].duration = 0.10f;
                    s_activeSfx[i].freqStart = 587.33f; // D5
                    s_activeSfx[i].freqEnd = 880.0f;    // A5
                    s_activeSfx[i].volume = 0.6f;
                    break;
                case SfxId::MenuCancel:
                    s_activeSfx[i].duration = 0.08f;
                    s_activeSfx[i].freqStart = 440.0f;
                    s_activeSfx[i].freqEnd = 220.0f;
                    s_activeSfx[i].volume = 0.5f;
                    break;
                case SfxId::HitPhysical:
                    s_activeSfx[i].duration = 0.15f;
                    s_activeSfx[i].freqStart = 200.0f;
                    s_activeSfx[i].freqEnd = 60.0f;
                    s_activeSfx[i].volume = 0.8f;
                    break;
                case SfxId::HitMagic:
                    s_activeSfx[i].duration = 0.22f;
                    s_activeSfx[i].freqStart = 1200.0f;
                    s_activeSfx[i].freqEnd = 400.0f;
                    s_activeSfx[i].volume = 0.7f;
                    break;
                case SfxId::StatusAfflict:
                    s_activeSfx[i].duration = 0.25f;
                    s_activeSfx[i].freqStart = 300.0f;
                    s_activeSfx[i].freqEnd = 900.0f;
                    s_activeSfx[i].volume = 0.6f;
                    break;
                case SfxId::CaptureThrow:
                    s_activeSfx[i].duration = 0.20f;
                    s_activeSfx[i].freqStart = 300.0f;
                    s_activeSfx[i].freqEnd = 1200.0f;
                    s_activeSfx[i].volume = 0.7f;
                    break;
                case SfxId::CaptureSuccess:
                    s_activeSfx[i].duration = 0.40f;
                    s_activeSfx[i].freqStart = 523.25f;
                    s_activeSfx[i].freqEnd = 1046.50f;
                    s_activeSfx[i].volume = 0.8f;
                    break;
                case SfxId::LevelUp:
                    s_activeSfx[i].duration = 0.50f;
                    s_activeSfx[i].freqStart = 440.0f;
                    s_activeSfx[i].freqEnd = 880.0f;
                    s_activeSfx[i].volume = 0.85f;
                    break;
                case SfxId::ChestOpen:
                    s_activeSfx[i].duration = 0.35f;
                    s_activeSfx[i].freqStart = 659.25f; // E5
                    s_activeSfx[i].freqEnd = 1318.5f;   // E6
                    s_activeSfx[i].volume = 0.75f;
                    break;
                case SfxId::DialogueBeep:
                    s_activeSfx[i].duration = 0.02f;
                    s_activeSfx[i].freqStart = 600.0f;
                    s_activeSfx[i].freqEnd = 600.0f;
                    s_activeSfx[i].volume = 0.25f;
                    break;
            }
            break;
        }
    }
}

void AudioEngine::setMasterVolume(float vol) {
    s_masterVolume.store(std::clamp(vol, 0.0f, 1.0f));
}

void AudioEngine::toggleMute() {
    s_muted.store(!s_muted.load());
}

bool AudioEngine::isMuted() {
    return s_muted.load();
}

void AudioEngine::fillAudioBuffer(int16_t* buffer, int numSamples) {
    if (s_muted.load() || s_masterVolume.load() <= 0.001f) {
        std::memset(buffer, 0, numSamples * sizeof(int16_t));
        return;
    }

    float masterVol = s_masterVolume.load();
    float stepDuration = 0.14f; // ~107 BPM 16th note step

    for (int i = 0; i < numSamples; ++i) {
        float sample = 0.0f;

        // 1. Synthesize BGM
        if (s_currentBgm != BgmTrack::None) {
            s_bgmTime += 1.0f / SAMPLE_RATE;
            s_bgmStep = static_cast<int>(s_bgmTime / stepDuration) % 32;

            const int* leadNotes = nullptr;
            const int* bassNotes = nullptr;

            switch (s_currentBgm) {
                case BgmTrack::Title:
                    leadNotes = kTitleBgmLead;
                    bassNotes = kTitleBgmBass;
                    break;
                case BgmTrack::Village:
                    leadNotes = kVillageBgmLead;
                    bassNotes = kVillageBgmBass;
                    break;
                case BgmTrack::WildBattle:
                    leadNotes = kBattleBgmLead;
                    bassNotes = kBattleBgmBass;
                    break;
                case BgmTrack::BossBattle:
                    leadNotes = kBossBgmLead;
                    bassNotes = kBossBgmBass;
                    break;
                default:
                    break;
            }

            if (leadNotes && bassNotes) {
                int leadNote = leadNotes[s_bgmStep];
                int bassNote = bassNotes[s_bgmStep];

                float leadFreq = midiToFreq(leadNote);
                float bassFreq = midiToFreq(bassNote);

                // Lead Square Wave (Duty 25%)
                if (leadFreq > 0.0f) {
                    s_phaseLead += leadFreq / SAMPLE_RATE;
                    if (s_phaseLead >= 1.0f) s_phaseLead -= 1.0f;
                    float leadSample = (s_phaseLead < 0.25f) ? 0.35f : -0.35f;
                    sample += leadSample * 0.40f;
                }

                // Bass Triangle Wave
                if (bassFreq > 0.0f) {
                    s_phaseBass += bassFreq / SAMPLE_RATE;
                    if (s_phaseBass >= 1.0f) s_phaseBass -= 1.0f;
                    float bassSample = (s_phaseBass < 0.5f) ? (4.0f * s_phaseBass - 1.0f) : (3.0f - 4.0f * s_phaseBass);
                    sample += bassSample * 0.45f;
                }

                // Drum / Hi-hat Noise on beat
                if ((s_bgmStep % 2 == 0) && (s_currentBgm == BgmTrack::WildBattle || s_currentBgm == BgmTrack::BossBattle)) {
                    sample += generateNoise() * 0.20f;
                }
            }
        }

        // 2. Synthesize Active SFX
        for (int s = 0; s < 4; ++s) {
            if (s_activeSfx[s].active) {
                float tRatio = s_activeSfx[s].time / s_activeSfx[s].duration;
                float curFreq = s_activeSfx[s].freqStart + (s_activeSfx[s].freqEnd - s_activeSfx[s].freqStart) * tRatio;
                float sfxSample = 0.0f;

                if (s_activeSfx[s].id == SfxId::HitPhysical) {
                    sfxSample = generateNoise() * 0.6f + std::sin(s_activeSfx[s].time * curFreq * 6.283185f) * 0.4f;
                } else {
                    sfxSample = (std::sin(s_activeSfx[s].time * curFreq * 6.283185f) > 0.0f) ? 0.5f : -0.5f;
                }

                float env = 1.0f - tRatio; // Linear decay
                sample += sfxSample * s_activeSfx[s].volume * env;

                s_activeSfx[s].time += 1.0f / SAMPLE_RATE;
                if (s_activeSfx[s].time >= s_activeSfx[s].duration) {
                    s_activeSfx[s].active = false;
                }
            }
        }

        // Final Master Clamping & Output
        sample = std::clamp(sample * masterVol, -1.0f, 1.0f);
        buffer[i] = static_cast<int16_t>(sample * 32767.0f);
    }
}

void AudioEngine::update(float dt) {
    (void)dt;
}

} // namespace JoseonRPG

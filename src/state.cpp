#include "state.h"

Phase phase_of(float m) {
    int h = (int)m / 60;
    if (h >= 5 && h < 7) return Phase::Dawn;
    if (h >= 7 && h < 17) return Phase::Day;
    if (h >= 17 && h < 19) return Phase::Dusk;
    return Phase::Night;
}

void rest(Clock& c) {
    c.day += 1;
    c.minute = 6 * 60;
}

namespace yy {

Game g;

// ---------------------------------------------------------------- helpers
void say(std::vector<std::string> texts, std::string speaker, std::function<void(int)> done,
         std::vector<std::string> choices) {
    Dialog d;
    d.speaker = std::move(speaker);
    for (auto& t : texts) {
        auto lines = wrap(t, 570);
        for (size_t i = 0; i < lines.size(); i += 2)
            d.pages.push_back(lines[i] + (i + 1 < lines.size() ? "\n" + lines[i + 1] : ""));
    }
    d.choices = std::move(choices);
    d.done = std::move(done);
    g.dq.push_back(std::move(d));
}

void show_toast(const std::string& s) { g.toast = s; g.toast_t = 180; }

void transition(std::function<void()> mid) {
    if (g.trans_t >= 0) return;
    g.trans_t = 0;
    g.trans_mid = std::move(mid);
}

int rnd(int lo, int hi) { return std::uniform_int_distribution<int>(lo, hi)(g.rng); }

unsigned hash3(int a, int b, int c) {
    unsigned h = (unsigned)a * 73856093u ^ (unsigned)b * 19349663u ^ (unsigned)c * 83492791u;
    h ^= h >> 13;
    h *= 0x5bd1e995u;
    return h ^ (h >> 15);
}

std::string clock_str() {
    int m = (int)g.clock.minute;
    char buf[48];
    const char* names[] = {"새벽", "낮", "저녁", "밤"};
    snprintf(buf, sizeof buf, "%d일째 · %s %02d:%02d", g.clock.day, names[(int)phase_of(g.clock.minute)], m / 60, m % 60);
    return buf;
}

}  // namespace yy

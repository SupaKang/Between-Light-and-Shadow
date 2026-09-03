#pragma once
#include "../core/types.hpp"
#include <vector>

namespace JoseonRPG {

class Renderer;

enum class WeatherType {
    None,
    EerieMist,     // Ch.1: 붉은 요기 안개 & 도깨비불
    Blizzard,      // Ch.2: 소백산 휘몰아치는 눈보라
    Rainstorm,     // Ch.3: 남해안 거친 빗줄기
    BambooFog,     // Ch.4: 지리산 대나무 숲 짙은 안개
    SolarEclipse   // Ch.5: 일식의 성채 암실 & 보라색 영기 오라
};

struct WeatherParticle {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float life = 0.0f;
    float maxLife = 1.0f;
    Color color;
    int size = 1;
};

class WeatherSystem {
public:
    WeatherSystem();

    void setWeatherForMap(int mapId);
    void setWeather(WeatherType type);
    void update(float dt);
    void render(Renderer& renderer) const;

    WeatherType getWeather() const { return m_currentWeather; }

private:
    void initParticles();

    WeatherType m_currentWeather = WeatherType::None;
    float m_animTimer = 0.0f;
    std::vector<WeatherParticle> m_particles;
};

} // namespace JoseonRPG

#include "weather_system.hpp"
#include "../core/renderer.hpp"
#include <random>
#include <cmath>
#include <algorithm>

namespace JoseonRPG {

namespace {
static std::mt19937 s_weatherRng(2026);
}

WeatherSystem::WeatherSystem() {
    setWeather(WeatherType::None);
}

void WeatherSystem::setWeatherForMap(int mapId) {
    // Determine region weather based on 26-map layout
    if (mapId == 1 || mapId == 2 || mapId == 8 || mapId == 13 || mapId == 18) {
        // Indoor maps (Tavern, Bureau, Simmani Hut, Haenyeo Shelter, Hermitage)
        setWeather(WeatherType::None);
    } else if (mapId >= 0 && mapId <= 5) {
        // Chapter 1: 한양 북악산 & 도선사
        setWeather(WeatherType::EerieMist);
    } else if (mapId >= 6 && mapId <= 10) {
        // Chapter 2: 소백산맥 & 죽령 험로
        setWeather(WeatherType::Blizzard);
    } else if (mapId >= 11 && mapId <= 15) {
        // Chapter 3: 남해안 & 다도해
        setWeather(WeatherType::Rainstorm);
    } else if (mapId >= 16 && mapId <= 20) {
        // Chapter 4: 지리산 & 천왕봉
        setWeather(WeatherType::BambooFog);
    } else if (mapId >= 21 && mapId <= 25) {
        // Chapter 5: 일식의 성채 & 음양당 본거지
        setWeather(WeatherType::SolarEclipse);
    } else {
        setWeather(WeatherType::None);
    }
}

void WeatherSystem::setWeather(WeatherType type) {
    if (m_currentWeather == type) return;
    m_currentWeather = type;
    m_animTimer = 0.0f;
    initParticles();
}

void WeatherSystem::initParticles() {
    m_particles.clear();
    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(SCREEN_WIDTH));
    std::uniform_real_distribution<float> distY(0.0f, static_cast<float>(SCREEN_HEIGHT));

    int count = 0;
    switch (m_currentWeather) {
        case WeatherType::EerieMist: count = 25; break;
        case WeatherType::Blizzard: count = 45; break;
        case WeatherType::Rainstorm: count = 55; break;
        case WeatherType::BambooFog: count = 30; break;
        case WeatherType::SolarEclipse: count = 30; break;
        default: count = 0; break;
    }

    m_particles.resize(count);
    for (int i = 0; i < count; ++i) {
        WeatherParticle& p = m_particles[i];
        p.x = distX(s_weatherRng);
        p.y = distY(s_weatherRng);
        p.life = 0.0f;
        p.maxLife = 2.0f + std::uniform_real_distribution<float>(0.0f, 2.0f)(s_weatherRng);

        switch (m_currentWeather) {
            case WeatherType::EerieMist:
                p.vx = std::uniform_real_distribution<float>(-15.0f, 15.0f)(s_weatherRng);
                p.vy = -std::uniform_real_distribution<float>(10.0f, 30.0f)(s_weatherRng);
                p.color = (i % 2 == 0) ? Color(255, 80, 50, 180) : Color(255, 180, 40, 160);
                p.size = (i % 3 == 0) ? 2 : 1;
                break;
            case WeatherType::Blizzard:
                p.vx = -std::uniform_real_distribution<float>(40.0f, 80.0f)(s_weatherRng);
                p.vy = std::uniform_real_distribution<float>(100.0f, 160.0f)(s_weatherRng);
                p.color = Color(240, 248, 255, 220);
                p.size = (i % 2 == 0) ? 2 : 1;
                break;
            case WeatherType::Rainstorm:
                p.vx = -std::uniform_real_distribution<float>(20.0f, 40.0f)(s_weatherRng);
                p.vy = std::uniform_real_distribution<float>(300.0f, 450.0f)(s_weatherRng);
                p.color = Color(120, 180, 255, 190);
                p.size = 1;
                break;
            case WeatherType::BambooFog:
                p.vx = std::uniform_real_distribution<float>(15.0f, 35.0f)(s_weatherRng);
                p.vy = std::uniform_real_distribution<float>(-3.0f, 3.0f)(s_weatherRng);
                p.color = Color(220, 235, 230, 100);
                p.size = 3;
                break;
            case WeatherType::SolarEclipse:
                p.vx = std::uniform_real_distribution<float>(-10.0f, 10.0f)(s_weatherRng);
                p.vy = -std::uniform_real_distribution<float>(15.0f, 35.0f)(s_weatherRng);
                p.color = (i % 2 == 0) ? Color(180, 60, 240, 180) : Color(120, 20, 180, 150);
                p.size = 2;
                break;
            default:
                break;
        }
    }
}

void WeatherSystem::update(float dt) {
    if (m_currentWeather == WeatherType::None) return;
    m_animTimer += dt;

    std::uniform_real_distribution<float> distX(0.0f, static_cast<float>(SCREEN_WIDTH));

    for (auto& p : m_particles) {
        p.x += p.vx * dt;
        p.y += p.vy * dt;
        p.life += dt;

        // Wrap around boundaries
        if (m_currentWeather == WeatherType::EerieMist || m_currentWeather == WeatherType::SolarEclipse) {
            if (p.y < -5.0f || p.life >= p.maxLife) {
                p.x = distX(s_weatherRng);
                p.y = static_cast<float>(SCREEN_HEIGHT + 5);
                p.life = 0.0f;
            }
        } else if (m_currentWeather == WeatherType::Blizzard || m_currentWeather == WeatherType::Rainstorm) {
            if (p.y > static_cast<float>(SCREEN_HEIGHT + 5)) {
                p.y = -5.0f;
                p.x = distX(s_weatherRng);
                p.life = 0.0f;
            }
            if (p.x < -5.0f) {
                p.x = static_cast<float>(SCREEN_WIDTH + 5);
            }
        } else if (m_currentWeather == WeatherType::BambooFog) {
            if (p.x > static_cast<float>(SCREEN_WIDTH + 10)) {
                p.x = -10.0f;
                p.y = std::uniform_real_distribution<float>(0.0f, static_cast<float>(SCREEN_HEIGHT))(s_weatherRng);
                p.life = 0.0f;
            }
        }
    }
}

void WeatherSystem::render(Renderer& renderer) const {
    if (m_currentWeather == WeatherType::None) return;

    // Ambient Region Atmosphere Lighting Tints
    if (m_currentWeather == WeatherType::EerieMist) {
        // Subtle red mist overlay
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(40, 10, 10, 35));
    } else if (m_currentWeather == WeatherType::Blizzard) {
        // Cold frost tint
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(20, 30, 50, 30));
    } else if (m_currentWeather == WeatherType::Rainstorm) {
        // Dark storm tint
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(10, 20, 40, 45));
    } else if (m_currentWeather == WeatherType::BambooFog) {
        // Thick mountain fog tint
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(200, 210, 205, 30));
    } else if (m_currentWeather == WeatherType::SolarEclipse) {
        // Menacing solar eclipse purple-black darkness
        renderer.fillRect(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT, Color(30, 10, 40, 75));
    }

    // Render Weather Particles
    for (const auto& p : m_particles) {
        int ix = static_cast<int>(p.x);
        int iy = static_cast<int>(p.y);

        if (ix < 0 || ix >= SCREEN_WIDTH || iy < 0 || iy >= SCREEN_HEIGHT) continue;

        if (m_currentWeather == WeatherType::Rainstorm) {
            // Draw rain streak line
            renderer.drawLine(ix, iy, ix - 1, iy + 4, p.color);
        } else if (p.size == 1) {
            renderer.setPixel(ix, iy, p.color);
        } else if (p.size == 2) {
            renderer.fillRect(ix, iy, 2, 2, p.color);
        } else if (p.size >= 3) {
            renderer.fillRect(ix - 1, iy - 1, 4, 3, p.color);
        }
    }
}

void WeatherSystem::setTimeOfDay(TimeOfDay tod) {
    m_timeOfDay = tod;
}

void WeatherSystem::applyLighting(Renderer& renderer, int playerScreenX, int playerScreenY, bool hasLantern) const {
    switch (m_timeOfDay) {
        case TimeOfDay::Day:
            // Normal daylight, no extra tint
            break;
        case TimeOfDay::Dusk:
            // Sunset orange-red tint
            renderer.applyColorTint(1.08f, 0.92f, 0.82f);
            break;
        case TimeOfDay::Night:
            // Night blue-black tint with radial torch/lantern light
            renderer.applyColorTint(0.70f, 0.75f, 0.95f);
            if (hasLantern) {
                renderer.applyRadialLighting(playerScreenX, playerScreenY, 68, 0.40f);
            } else {
                renderer.applyRadialLighting(playerScreenX, playerScreenY, 32, 0.25f);
            }
            break;
        case TimeOfDay::Midnight:
            // Deep midnight darkness
            renderer.applyColorTint(0.55f, 0.60f, 0.85f);
            if (hasLantern) {
                renderer.applyRadialLighting(playerScreenX, playerScreenY, 52, 0.28f);
            } else {
                renderer.applyRadialLighting(playerScreenX, playerScreenY, 22, 0.15f);
            }
            break;
    }
}

} // namespace JoseonRPG

#include "skill_fx_system.hpp"
#include "../core/renderer.hpp"
#include <random>
#include <cmath>

namespace JoseonRPG {

namespace {
static std::mt19937 s_fxRng(2026);
}

SkillFxSystem::SkillFxSystem() {}

void SkillFxSystem::triggerSkillFx(Element elem, int targetX, int targetY) {
    int count = 24;
    m_particles.reserve(m_particles.size() + count);

    for (int i = 0; i < count; ++i) {
        SkillParticle p;
        p.element = elem;
        p.life = 0.0f;
        p.maxLife = 0.40f + std::uniform_real_distribution<float>(0.0f, 0.20f)(s_fxRng);

        float angle = std::uniform_real_distribution<float>(0.0f, 6.283185f)(s_fxRng);
        float speed = std::uniform_real_distribution<float>(30.0f, 90.0f)(s_fxRng);

        p.x = static_cast<float>(targetX);
        p.y = static_cast<float>(targetY);

        switch (elem) {
            case Element::Fire:
                p.vx = std::cos(angle) * speed * 0.7f;
                p.vy = -std::abs(std::sin(angle) * speed) - 30.0f; // Rise upwards
                p.color = (i % 2 == 0) ? Color(255, 60, 30) : Color(255, 200, 50);
                p.size = (i % 3 == 0) ? 3 : 2;
                break;
            case Element::Water:
                p.vx = std::cos(angle) * speed * 1.1f;
                p.vy = std::sin(angle) * speed * 1.1f;
                p.color = (i % 2 == 0) ? Color(100, 200, 255) : Color(240, 250, 255);
                p.size = (i % 2 == 0) ? 2 : 1;
                break;
            case Element::Earth:
                p.vx = std::cos(angle) * speed * 0.8f;
                p.vy = std::sin(angle) * speed * 0.8f + 20.0f; // Gravity pull
                p.color = (i % 2 == 0) ? Color(160, 110, 60) : Color(200, 170, 100);
                p.size = (i % 2 == 0) ? 3 : 2;
                break;
            case Element::Light:
                p.vx = std::cos(angle) * speed * 1.3f;
                p.vy = std::sin(angle) * speed * 1.3f;
                p.color = (i % 2 == 0) ? Color(255, 255, 180) : Color(255, 255, 255);
                p.size = (i % 3 == 0) ? 3 : 2;
                break;
            case Element::Dark:
                // Spiral inward vortex
                p.x += std::cos(angle) * 30.0f;
                p.y += std::sin(angle) * 30.0f;
                p.vx = -std::cos(angle) * speed * 0.8f;
                p.vy = -std::sin(angle) * speed * 0.8f;
                p.color = (i % 2 == 0) ? Color(140, 40, 200) : Color(30, 10, 50);
                p.size = (i % 2 == 0) ? 3 : 2;
                break;
            default: // Physical
                p.vx = std::cos(angle) * speed;
                p.vy = std::sin(angle) * speed;
                p.color = Color(240, 240, 240);
                p.size = 2;
                break;
        }

        m_particles.push_back(p);
    }
}

void SkillFxSystem::update(float dt) {
    for (auto it = m_particles.begin(); it != m_particles.end();) {
        it->life += dt;
        if (it->life >= it->maxLife) {
            it = m_particles.erase(it);
        } else {
            it->x += it->vx * dt;
            it->y += it->vy * dt;
            ++it;
        }
    }
}

void SkillFxSystem::render(Renderer& renderer) const {
    for (const auto& p : m_particles) {
        int ix = static_cast<int>(p.x);
        int iy = static_cast<int>(p.y);

        if (ix < 0 || ix >= SCREEN_WIDTH || iy < 0 || iy >= SCREEN_HEIGHT) continue;

        float alphaRatio = 1.0f - (p.life / p.maxLife);
        Color drawCol = p.color;
        drawCol.a = static_cast<uint8_t>(std::clamp(alphaRatio * 255.0f, 0.0f, 255.0f));

        if (p.size == 1) {
            renderer.setPixel(ix, iy, drawCol);
        } else if (p.size == 2) {
            renderer.fillRect(ix, iy, 2, 2, drawCol);
        } else if (p.size >= 3) {
            renderer.fillRect(ix - 1, iy - 1, 3, 3, drawCol);
        }
    }
}

bool SkillFxSystem::isActive() const {
    return !m_particles.empty();
}

} // namespace JoseonRPG

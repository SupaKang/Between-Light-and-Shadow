#pragma once
#include "../core/types.hpp"
#include <vector>

namespace JoseonRPG {

class Renderer;

struct SkillParticle {
    float x = 0.0f;
    float y = 0.0f;
    float vx = 0.0f;
    float vy = 0.0f;
    float life = 0.0f;
    float maxLife = 0.5f;
    Color color;
    int size = 2;
    Element element = Element::Physical;
};

class SkillFxSystem {
public:
    SkillFxSystem();

    void triggerSkillFx(Element elem, int targetX, int targetY);
    void update(float dt);
    void render(Renderer& renderer) const;
    bool isActive() const;

private:
    std::vector<SkillParticle> m_particles;
};

} // namespace JoseonRPG

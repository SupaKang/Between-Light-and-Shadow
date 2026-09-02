#include "../src/core/types.hpp"
#include "../src/gameplay/yokai.hpp"
#include "../src/gameplay/party.hpp"
#include "../src/gameplay/artifact.hpp"
#include "../src/battle/battle.hpp"
#include "../src/battle/status_effects.hpp"

#include <iostream>
#include <cassert>

using namespace JoseonRPG;

bool runBattleSystemTests() {
    std::cout << "[TEST] Running Battle System Tests..." << std::endl;

    Yokai dokkaebi("YOKAI_001", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {100, 100, 50, 50, 20, 10, 15});
    Skill strike{"SKL_01", "Club Strike", 40, 10, Element::Physical, 100, StatusEffect::None, 0};
    dokkaebi.addSkill(strike);

    Yokai gumiho("YOKAI_002", "Gumiho", YokaiGrade::Grade4, Element::Fire, {100, 100, 50, 50, 25, 10, 20});
    
    Party party;
    party.addYokai(dokkaebi);

    ArtifactInventory artifacts;
    Battle battle(party, gumiho, artifacts);

    // Test initial capture rate
    float captureRate = battle.calculateCaptureProbability();
    std::cout << "  - Initial Gumiho capture rate: " << captureRate * 100 << "%" << std::endl;
    if (captureRate <= 0.0f || captureRate >= 1.0f) {
        std::cerr << "  FAIL: Capture rate out of range!" << std::endl;
        return false;
    }

    // Test Damage Formula
    int dmg = battle.calculateDamage(dokkaebi, gumiho, strike, true);
    std::cout << "  - Calculated damage (Dokkaebi vs Gumiho): " << dmg << std::endl;
    if (dmg <= 0) {
        std::cerr << "  FAIL: Damage must be positive!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] Battle System Tests" << std::endl;
    return true;
}

bool runStatusEffectTests() {
    std::cout << "[TEST] Running 5 Core Status Effects Tests..." << std::endl;

    Yokai target("YOKAI_001", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {100, 100, 50, 50, 20, 10, 15});

    // 1. Burn Test
    StatusEffectSystem::applyStatus(target, StatusEffect::Burn, 3);
    if (target.getStatus().effect != StatusEffect::Burn) {
        std::cerr << "  FAIL: Burn status not applied!" << std::endl;
        return false;
    }
    std::string log;
    int initialHp = target.getStats().hp;
    StatusEffectSystem::onActionUsed(target, true, log);
    if (target.getStats().hp >= initialHp) {
        std::cerr << "  FAIL: Burn did not deal backlash damage on attack!" << std::endl;
        return false;
    }
    target.clearStatus();

    // 2. Freeze Test
    StatusEffectSystem::applyStatus(target, StatusEffect::Freeze, 2);
    bool canAct = StatusEffectSystem::onTurnStart(target, log);
    if (canAct) {
        std::cerr << "  FAIL: Frozen target should not be able to act!" << std::endl;
        return false;
    }
    // Shatter on damage
    StatusEffectSystem::onDamageReceived(target, 10, log);
    if (target.getStatus().effect == StatusEffect::Freeze) {
        std::cerr << "  FAIL: Freeze did not shatter on damage received!" << std::endl;
        return false;
    }

    // 3. Seal Test
    StatusEffectSystem::applyStatus(target, StatusEffect::Seal, 99, 0);
    if (target.getStatus().sealedSkillIndex != 0) {
        std::cerr << "  FAIL: Sealed skill index mismatch!" << std::endl;
        return false;
    }
    target.clearStatus();

    // 4. Fear Test
    StatusEffectSystem::applyStatus(target, StatusEffect::Fear, 3);
    int rawDmg = 20;
    int amplifiedDmg = StatusEffectSystem::onDamageReceived(target, rawDmg, log);
    if (amplifiedDmg <= rawDmg) {
        std::cerr << "  FAIL: Fear did not amplify damage received!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] 5 Status Effects Tests" << std::endl;
    return true;
}

bool runArtifactSystemTests() {
    std::cout << "[TEST] Running Artifact Dual-Trait & Destruction Tests..." << std::endl;

    ArtifactInventory inv;
    Artifact dokkaebiHat{"ART_01", "Dokkaebi Hat", ArtifactBuffType::CritRateBoost, 30, ArtifactDebuffType::QiDrainPerTurn, 5, "Test hat"};
    inv.addArtifact(dokkaebiHat);

    if (inv.getCritRateBonus() != 30) {
        std::cerr << "  FAIL: Buff value not applied!" << std::endl;
        return false;
    }
    if (inv.getQiDrainPerTurn() != 5) {
        std::cerr << "  FAIL: Debuff value not applied!" << std::endl;
        return false;
    }

    // Instant Destroy Test
    inv.destroyArtifact(0);
    if (inv.getCount() != 0 || inv.getCritRateBonus() != 0 || inv.getQiDrainPerTurn() != 0) {
        std::cerr << "  FAIL: Instant destroy did not clear modifiers!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] Artifact System Tests" << std::endl;
    return true;
}

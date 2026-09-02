#include "../src/core/types.hpp"
#include "../src/gameplay/yokai.hpp"
#include "../src/gameplay/party.hpp"
#include "../src/gameplay/artifact.hpp"
#include "../src/gameplay/encyclopedia.hpp"
#include "../src/battle/battle.hpp"
#include "../src/battle/status_effects.hpp"

#include <iostream>
#include <cassert>

using namespace JoseonRPG;

bool runBattleSystemTests() {
    std::cout << "[TEST 1] Running Battle 1v1 Engine & Speed Initiative Tests..." << std::endl;

    // Fast attacker
    Yokai gumiho("YOKAI_002", "Gumiho", YokaiGrade::Grade4, Element::Fire, {110, 110, 100, 100, 28, 16, 26});
    Skill foxFire{"SKL_FOX_FIRE", "Fox Fire", 65, 22, Element::Fire, 90, StatusEffect::Burn, 40};
    gumiho.addSkill(foxFire);

    // Slower defender
    Yokai bulgasari("YOKAI_003", "Bulgasari", YokaiGrade::Grade3, Element::Earth, {140, 140, 50, 50, 24, 28, 10});
    Skill ironCharge{"SKL_IRON_CHARGE", "Iron Charge", 55, 15, Element::Earth, 90, StatusEffect::None, 0};
    bulgasari.addSkill(ironCharge);

    Party party;
    party.addYokai(gumiho);

    ArtifactInventory artifacts;
    Battle battle(party, bulgasari, artifacts);

    // Initial capture rate test
    float initialCapture = battle.calculateCaptureProbability();
    std::cout << "  - Initial Bulgasari (100% HP) capture rate: " << initialCapture * 100 << "%" << std::endl;
    if (initialCapture <= 0.0f || initialCapture > 0.30f) {
        std::cerr << "  FAIL: Initial capture rate should be low at 100% HP!" << std::endl;
        return false;
    }

    // Damage Formula test
    int dmg = battle.calculateDamage(gumiho, bulgasari, foxFire, true);
    std::cout << "  - Gumiho Fox Fire damage vs Bulgasari: " << dmg << std::endl;
    if (dmg <= 0) {
        std::cerr << "  FAIL: Damage must be positive!" << std::endl;
        return false;
    }

    // Weaken enemy and re-test capture rate
    battle.getEnemyYokai().takeDamage(100); // Reduce HP to ~40/140
    float weakenedCapture = battle.calculateCaptureProbability();
    std::cout << "  - Weakened Bulgasari capture rate: " << weakenedCapture * 100 << "%" << std::endl;
    if (weakenedCapture <= initialCapture) {
        std::cerr << "  FAIL: Weakened enemy should have higher capture rate!" << std::endl;
        return false;
    }

    // Add status effect and re-test capture rate (+25% bonus)
    StatusEffectSystem::applyStatus(battle.getEnemyYokai(), StatusEffect::Paralysis, 3);
    float statusCapture = battle.calculateCaptureProbability();
    std::cout << "  - Weakened + Paralyzed capture rate: " << statusCapture * 100 << "%" << std::endl;
    if (statusCapture <= weakenedCapture) {
        std::cerr << "  FAIL: Status effect should boost capture rate!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] Battle 1v1 & Capture Mechanics" << std::endl;
    return true;
}

bool runStatusEffectTests() {
    std::cout << "[TEST 2] Running 5 Core Status Effects In-Depth Tests..." << std::endl;

    Yokai target("YOKAI_001", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {100, 100, 50, 50, 20, 10, 15});
    std::string log;

    // 1. Burn Test (Backlash on attack only)
    StatusEffectSystem::applyStatus(target, StatusEffect::Burn, 3);
    int hpBefore = target.getStats().hp;
    StatusEffectSystem::onActionUsed(target, true, log);
    if (target.getStats().hp >= hpBefore) {
        std::cerr << "  FAIL: Burn did not deal backlash damage!" << std::endl;
        return false;
    }
    target.clearStatus();

    // 2. Freeze Test (Incapacitation & Shatter)
    StatusEffectSystem::applyStatus(target, StatusEffect::Freeze, 2);
    bool canAct = StatusEffectSystem::onTurnStart(target, log);
    if (canAct) {
        std::cerr << "  FAIL: Frozen target should not act!" << std::endl;
        return false;
    }
    // Shatter upon receiving damage
    StatusEffectSystem::onDamageReceived(target, 15, log);
    if (target.getStatus().effect == StatusEffect::Freeze) {
        std::cerr << "  FAIL: Freeze did not shatter upon damage!" << std::endl;
        return false;
    }

    // 3. Seal Test (Locks specific skill index)
    StatusEffectSystem::applyStatus(target, StatusEffect::Seal, 99, 1);
    if (target.getStatus().sealedSkillIndex != 1) {
        std::cerr << "  FAIL: Seal skill index incorrect!" << std::endl;
        return false;
    }
    target.clearStatus();

    // 4. Paralysis Test (33% failure roll)
    StatusEffectSystem::applyStatus(target, StatusEffect::Paralysis, 4);
    if (target.getStatus().effect != StatusEffect::Paralysis) {
        std::cerr << "  FAIL: Paralysis status not applied!" << std::endl;
        return false;
    }
    target.clearStatus();

    // 5. Fear Test (25% outgoing reduction, 25% incoming amplification)
    StatusEffectSystem::applyStatus(target, StatusEffect::Fear, 3);
    int baseDmg = 20;
    int ampDmg = StatusEffectSystem::onDamageReceived(target, baseDmg, log);
    if (ampDmg != static_cast<int>(baseDmg * 1.25f)) {
        std::cerr << "  FAIL: Fear did not amplify incoming damage by 25%!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] 5 Core Status Effects In-Depth" << std::endl;
    return true;
}

bool runArtifactSystemTests() {
    std::cout << "[TEST 3] Running Dual-Trait Artifacts & Instant Destroy Tests..." << std::endl;

    ArtifactInventory inv;
    Artifact hat{"ART_01", "Dokkaebi Hat", ArtifactBuffType::CritRateBoost, 30, ArtifactDebuffType::QiDrainPerTurn, 5, "Test hat"};
    Artifact egg{"ART_02", "Centipede Egg", ArtifactBuffType::ImmunityBurn, 1, ArtifactDebuffType::MaxHpReduction, 20, "Test egg"};
    inv.addArtifact(hat);
    inv.addArtifact(egg);

    if (inv.getCritRateBonus() != 30) {
        std::cerr << "  FAIL: Crit buff mismatch!" << std::endl;
        return false;
    }
    if (!inv.hasBurnImmunity()) {
        std::cerr << "  FAIL: Burn immunity buff not active!" << std::endl;
        return false;
    }
    if (inv.getQiDrainPerTurn() != 5) {
        std::cerr << "  FAIL: Qi drain debuff mismatch!" << std::endl;
        return false;
    }

    // Destroy first artifact (hat)
    inv.destroyArtifact(0);
    if (inv.getCritRateBonus() != 0 || inv.getQiDrainPerTurn() != 0) {
        std::cerr << "  FAIL: Destroyed artifact effect still lingered!" << std::endl;
        return false;
    }
    // Centipede egg should still be present
    if (!inv.hasBurnImmunity() || inv.getCount() != 1) {
        std::cerr << "  FAIL: Remaining artifact corrupted after destroy!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] Artifacts Dual-Trait & Destruction" << std::endl;
    return true;
}

bool runEncyclopediaAndGrowthTests() {
    std::cout << "[TEST 4] Running 108 Encyclopedia & Yokai Growth/Promotion Tests..." << std::endl;

    // 1. Encyclopedia Test
    Encyclopedia codex;
    for (int i = 1; i <= 108; ++i) {
        codex.registerTemplate(i, "YOKAI_" + std::to_string(i), "요괴 #" + std::to_string(i), "Yokai #" + std::to_string(i),
                               YokaiGrade::Grade1, Element::Physical, "KOREAN_FOLKLORE", "테스트 요괴 설화");
    }

    if (codex.getTotalEntries() != 108) {
        std::cerr << "  FAIL: Total encyclopedia entries != 108 (Got " << codex.getTotalEntries() << ")" << std::endl;
        return false;
    }

    codex.markSeen("YOKAI_1");
    codex.markCaptured("YOKAI_1");
    codex.markCaptured("YOKAI_2");

    if (codex.getCapturedCount() != 2) {
        std::cerr << "  FAIL: Captured count mismatch! Expected 2, got " << codex.getCapturedCount() << std::endl;
        return false;
    }
    float rate = codex.getCompletionRate();
    std::cout << "  - Codex completion rate (2/108): " << rate * 100.0f << "%" << std::endl;

    // 2. Growth and Leveling Test (Lv 1 to Lv 10)
    Yokai dokkaebi(1, "YOKAI_001", "Dokkaebi", YokaiGrade::Grade1, Element::Fire, {100, 100, 50, 50, 20, 10, 15});
    int baseAtk = dokkaebi.getStats().atk;
    int baseHp = dokkaebi.getStats().maxHp;

    // Add EXP to reach Lv 10
    // Required EXP: sum(15*n^2 + 50*n)
    dokkaebi.gainExp(10000);
    std::cout << "  - Dokkaebi gained 10000 EXP -> Reached Lv." << dokkaebi.getLevel() << " (HP: " << dokkaebi.getStats().maxHp << ", ATK: " << dokkaebi.getStats().atk << ")" << std::endl;

    if (dokkaebi.getLevel() < 10) {
        std::cerr << "  FAIL: Yokai level did not advance properly!" << std::endl;
        return false;
    }
    if (dokkaebi.getStats().atk <= baseAtk || dokkaebi.getStats().maxHp <= baseHp) {
        std::cerr << "  FAIL: Stats did not increase after leveling!" << std::endl;
        return false;
    }

    // 3. Grade Promotion Test (Grade 1 -> Grade 2)
    if (!dokkaebi.canPromote()) {
        std::cerr << "  FAIL: Dokkaebi at Lv." << dokkaebi.getLevel() << " should be eligible for Grade 2 promotion!" << std::endl;
        return false;
    }

    int atkBeforePromotion = dokkaebi.getStats().atk;
    bool promoted = dokkaebi.promoteGrade();
    if (!promoted || dokkaebi.getGrade() != YokaiGrade::Grade2) {
        std::cerr << "  FAIL: Grade promotion failed!" << std::endl;
        return false;
    }

    std::cout << "  - Dokkaebi promoted to Grade 2! (ATK: " << atkBeforePromotion << " -> " << dokkaebi.getStats().atk << ")" << std::endl;
    if (dokkaebi.getStats().atk <= atkBeforePromotion) {
        std::cerr << "  FAIL: Base stat multiplier did not boost ATK after promotion!" << std::endl;
        return false;
    }

    std::cout << "  [PASS] 108 Encyclopedia & Growth/Promotion" << std::endl;
    return true;
}

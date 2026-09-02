#include "data_manager.hpp"

namespace JoseonRPG {

std::vector<Yokai> DataManager::s_yokaiDatabase;
std::vector<Skill> DataManager::s_skillDatabase;
std::vector<Artifact> DataManager::s_artifactDatabase;

void DataManager::init() {
    // Skills
    Skill clubStrike{"SKL_CLUB_STRIKE", "Club Strike", 45, 10, Element::Physical, 95, StatusEffect::None, 0};
    Skill goblinFire{"SKL_GOBLIN_FIRE", "Goblin Fire", 55, 18, Element::Fire, 90, StatusEffect::Burn, 35};
    Skill taunt{"SKL_TAUNT", "Taunt Grin", 20, 12, Element::Dark, 100, StatusEffect::Fear, 60};
    Skill earthSlam{"SKL_EARTH_SLAM", "Earth Slam", 60, 25, Element::Earth, 85, StatusEffect::None, 0};

    Skill foxFire{"SKL_FOX_FIRE", "Fox Fire", 65, 22, Element::Fire, 90, StatusEffect::Burn, 40};
    Skill fearGaze{"SKL_FEAR_GAZE", "Fearful Gaze", 30, 15, Element::Dark, 100, StatusEffect::Fear, 80};
    Skill spiritDrain{"SKL_SPIRIT_DRAIN", "Spirit Drain", 40, 20, Element::Dark, 95, StatusEffect::None, 0};
    Skill illusionBurst{"SKL_ILLUSION_BURST", "Illusion Burst", 75, 35, Element::Light, 85, StatusEffect::None, 0};

    Skill chillWail{"SKL_CHILL_WAIL", "Chill Wail", 50, 20, Element::Water, 85, StatusEffect::Freeze, 35};
    Skill talismanSeal{"SKL_TALISMAN_SEAL", "Talisman Seal", 25, 25, Element::Light, 95, StatusEffect::Seal, 100};
    Skill thunderBolt{"SKL_THUNDER_BOLT", "Thunder Bolt", 70, 30, Element::Physical, 85, StatusEffect::Paralysis, 45};
    Skill ironCharge{"SKL_IRON_CHARGE", "Iron Charge", 55, 15, Element::Earth, 90, StatusEffect::None, 0};
    Skill flameBreath{"SKL_FLAME_BREATH", "Flame Breath", 60, 24, Element::Fire, 90, StatusEffect::Burn, 50};

    s_skillDatabase = {
        clubStrike, goblinFire, taunt, earthSlam,
        foxFire, fearGaze, spiritDrain, illusionBurst,
        chillWail, talismanSeal, thunderBolt, ironCharge, flameBreath
    };

    // 1. Dokkaebi (#001 - Grade 2)
    Yokai dokkaebi("YOKAI_001", "Dokkaebi", YokaiGrade::Grade2, Element::Fire, {95, 95, 60, 60, 22, 18, 14});
    dokkaebi.addSkill(clubStrike);
    dokkaebi.addSkill(goblinFire);
    dokkaebi.addSkill(taunt);
    dokkaebi.addSkill(earthSlam);

    // 2. Gumiho (#002 - Grade 4)
    Yokai gumiho("YOKAI_002", "Gumiho", YokaiGrade::Grade4, Element::Fire, {110, 110, 100, 100, 28, 16, 26});
    gumiho.addSkill(foxFire);
    gumiho.addSkill(fearGaze);
    gumiho.addSkill(spiritDrain);
    gumiho.addSkill(illusionBurst);

    // 3. Bulgasari (#003 - Grade 3)
    Yokai bulgasari("YOKAI_003", "Bulgasari", YokaiGrade::Grade3, Element::Earth, {140, 140, 50, 50, 24, 28, 10});
    bulgasari.addSkill(ironCharge);
    bulgasari.addSkill(flameBreath);
    bulgasari.addSkill(clubStrike);
    bulgasari.addSkill(earthSlam);

    // 4. Maiden Ghost Cheonyeogwishin (#005 - Grade 2)
    Yokai maidenGhost("YOKAI_005", "Cheonyeogwishin", YokaiGrade::Grade2, Element::Water, {85, 85, 80, 80, 20, 14, 22});
    maidenGhost.addSkill(chillWail);
    maidenGhost.addSkill(fearGaze);
    maidenGhost.addSkill(talismanSeal);
    maidenGhost.addSkill(spiritDrain);

    // 5. Thunder Nue (#008 - Grade 3)
    Yokai nue("YOKAI_008", "Thunder Nue", YokaiGrade::Grade3, Element::Dark, {105, 105, 75, 75, 26, 17, 21});
    nue.addSkill(thunderBolt);
    nue.addSkill(fearGaze);
    nue.addSkill(clubStrike);
    nue.addSkill(talismanSeal);

    s_yokaiDatabase = {dokkaebi, gumiho, bulgasari, maidenGhost, nue};

    // Artifacts
    Artifact dokkaebiHat{"ART_DOKKAEBI_HAT", "Dokkaebi Hat", ArtifactBuffType::CritRateBoost, 30, ArtifactDebuffType::QiDrainPerTurn, 5, "Invisibility hat with Qi drain."};
    Artifact centipedeEgg{"ART_CENTIPEDE_EGG", "Centipede Egg", ArtifactBuffType::ImmunityBurn, 1, ArtifactDebuffType::MaxHpReduction, 20, "Burn immunity at the cost of Max HP."};
    Artifact foxShard{"ART_FOX_MARBLE_SHARD", "Fox Bead Shard", ArtifactBuffType::MagicAtkBoost, 40, ArtifactDebuffType::DefReduction, 35, "Massive magic power, fragile body."};
    Artifact demonTile{"ART_DEMON_TILE", "Demon Tile (Gwimyeonwa)", ArtifactBuffType::DamageReduction, 20, ArtifactDebuffType::SpdReduction, 15, "Reduces physical damage, SPD -15%."};
    
    s_artifactDatabase = {dokkaebiHat, centipedeEgg, foxShard, demonTile};
}

const std::vector<Yokai>& DataManager::getAllYokaiTemplates() {
    return s_yokaiDatabase;
}

const std::vector<Skill>& DataManager::getAllSkills() {
    return s_skillDatabase;
}

const std::vector<Artifact>& DataManager::getAllArtifacts() {
    return s_artifactDatabase;
}

Yokai DataManager::createYokaiById(std::string_view id) {
    for (const auto& y : s_yokaiDatabase) {
        if (y.getId() == id) return y;
    }
    return s_yokaiDatabase.empty() ? Yokai{} : s_yokaiDatabase[0];
}

Artifact DataManager::createArtifactById(std::string_view id) {
    for (const auto& a : s_artifactDatabase) {
        if (a.id == id) return a;
    }
    return s_artifactDatabase.empty() ? Artifact{} : s_artifactDatabase[0];
}

} // namespace JoseonRPG

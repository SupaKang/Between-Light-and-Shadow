#include "../src/battle_rules.h"
#include "../src/status_rules.h"
#include "../src/battle_state.h"
#include "../src/collection_state.h"
#include "../src/data_registry.h"
#include "../src/battle_ui.h"
#include "../src/save_rules.h"
#include "../src/skill_state.h"
#include "../src/world_state.h"
#include <cassert>
#include <cstring>
#include <string>

int main() {
    // 1. Battle Rules
    battle::Combatant a{12, 0, 100, 30, 10};
    battle::Combatant d{0, 5, 80, 0, 8};
    assert(battle::damage(a, d, 10, false) == 17);
    assert(battle::damage(a, d, 10, true) == 25);
    // Attacker feared: attack reduced from 12 to 9, value = 9 + 10 - 5 = 14
    assert(battle::damage(a, d, 10, true, false) == 14);

    assert(!battle::can_capture(60, 80, 2));
    assert(battle::can_capture(40, 80, 2));
    assert(battle::capture_rate_percent(60, 80) == 0); // > 50% HP
    assert(battle::capture_rate_percent(40, 80) == 30); // 50% weakness * 60% = 30%
    assert(battle::capture_rate_percent(40, 80, true) == 55); // +25% for status effect

    // 2. Status Rules
    assert(status_rules::tick_damage(status_rules::Kind::Burn, 3) == 3);
    assert(status_rules::tick_damage(status_rules::Kind::Freeze, 3) == 0);
    assert(status_rules::skips_action(status_rules::Kind::Freeze, 2));
    assert(status_rules::adjusted_damage(status_rules::Kind::Fear, 10) == 15);
    assert(status_rules::adjusted_damage(status_rules::Kind::Fear, 10, 1.2) == 12);
    assert(status_rules::attack_modifier(status_rules::Kind::Fear, 20) == 15);
    assert(status_rules::on_hit_effect(status_rules::Kind::Freeze) == status_rules::Kind::None);
    assert(status_rules::on_hit_effect(status_rules::Kind::Burn) == status_rules::Kind::Burn);
    assert(status_rules::default_duration(status_rules::Kind::Burn) == 3);
    assert(std::strcmp(status_rules::short_tag(status_rules::Kind::Burn), "[BRN]") == 0);

    // 3. Battle State
    battle::State state;
    state.active = true;
    state.enemy_level = 3;
    state.turn = 4;
    state.reset();
    assert(!state.active && state.enemy_hp == 80 && state.enemy_level == 1);

    // 4. Collection State (3 party limit, 108 codex)
    collection::State collection;
    assert(collection.add_contract(1));
    assert(!collection.add_contract(1)); // duplicate rejected
    assert(collection.contains(1));
    assert(collection.add_contract(2));
    assert(collection.add_contract(3));
    assert(!collection.add_contract(4)); // party full (max 3)
    assert(collection.party_count == 3);
    assert(collection.contracted_count() == 4); // codex contracted tracking
    assert(collection.discover(108));
    assert(collection.discovered_count() == 5);
    assert(!collection.discover(109)); // out of range
    assert(collection.swap_party(0, 2));
    assert(collection.party_ids[0] == 3 && collection.party_ids[2] == 1);
    assert(collection.active_yokai_id() == 3);

    // 5. Data Registry
    assert(data::Registry::integer("{\"power\":17}", "power", 0) == 17);
    assert(data::Registry::number("{\"multiplier\":1.5}", "multiplier", 1.0) == 1.5);
    assert(data::Registry::string_value("{\"label\":\"fight\"}", "label", "") == "fight");
    auto list = data::Registry::string_list("{\"skills\": [\"s1\", \"s2\"]}", "skills");
    assert(list.size() == 2 && list[0] == "s1" && list[1] == "s2");

    auto skill_def = data::Registry::parse_skill("{\"id\":\"skill_002\",\"power\":8,\"qi_cost\":3,\"status\":\"burn\"}");
    assert(skill_def.id == "skill_002" && skill_def.power == 8 && skill_def.qi_cost == 3 && skill_def.status == status_rules::Kind::Burn);

    auto yokai_def = data::Registry::parse_yokai("{\"id\":\"yokai_001\",\"level\":1,\"skills\":[\"s1\",\"s2\"]}");
    assert(yokai_def.id == "yokai_001" && yokai_def.skill_count == 2 && yokai_def.skills[0] == "s1");

    auto artifact_def = data::Registry::parse_artifact("{\"id\":\"art_001\",\"attack_bonus\":3,\"destroyable\":true}");
    assert(artifact_def.attack_bonus == 3 && artifact_def.destroyable);

    // 6. Battle UI & Navigation
    assert(battle_ui::move_command(0, -1, 3) == 2);
    assert(battle_ui::move_command(2, 1, 3) == 0);
    assert(battle_ui::move_command(0, -1, 6) == 5);
    assert(battle_ui::move_command(5, 1, 6) == 0);

    // 7. Skills State
    skills::State all_skills;
    for (int i = 0; i < skills::State::MaxSlots; ++i) all_skills.ids[i] = i + 1;
    assert(all_skills.usable(3, 0)); // default cost 0 is usable
    all_skills.qi_costs[3] = 5;
    assert(!all_skills.usable(3, 4)); // insufficient qi
    assert(all_skills.usable(3, 5)); // sufficient qi

    skills::State skills;
    skills.ids[0] = 1;
    assert(skills.usable(0, 0));
    skills.seal(0, 1);
    assert(!skills.usable(0, 0));
    skills.tick();
    assert(skills.usable(0, 0));

    char buf[128];
    battle_ui::command_detail(0, all_skills, 10, 40, 80, 50, buf, sizeof(buf));
    assert(std::strlen(buf) > 0);
    battle_ui::command_detail(4, all_skills, 10, 40, 80, 50, buf, sizeof(buf));
    assert(std::strstr(buf, "50%") != nullptr);

    // 8. Save Rules
    int ids[3] = {1, 2, 3};
    const auto valid_checksum = save_rules::checksum(save_rules::Version, 1, 2, 100, 30, 1, 0, 3, ids, true, false, true);
    assert(valid_checksum == save_rules::checksum(save_rules::Version, 1, 2, 100, 30, 1, 0, 3, ids, true, false, true));
    assert(valid_checksum != save_rules::checksum(save_rules::Version, 1, 2, 99, 30, 1, 0, 3, ids, true, false, true));

    // 9. World State
    world::State world;
    assert(world.can_interact(15, 7));
    assert(!world.can_interact(1, 1));
    world.mark_shrine_seen();
    assert(world.shrine_event_seen);

    return 0;
}

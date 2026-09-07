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
int main() {
    battle::Combatant a{12, 0, 100, 30, 10};
    battle::Combatant d{0, 5, 80, 0, 8};
    assert(battle::damage(a, d, 10, false) == 17);
    assert(battle::damage(a, d, 10, true) == 25);
    assert(!battle::can_capture(60, 80, 2));
    assert(battle::can_capture(40, 80, 2));
    assert(status_rules::tick_damage(status_rules::Kind::Burn, 3) == 3);
    assert(status_rules::skips_action(status_rules::Kind::Freeze, 2));
    assert(status_rules::adjusted_damage(status_rules::Kind::Fear, 10) == 15);
    assert(status_rules::adjusted_damage(status_rules::Kind::Fear, 10, 1.2) == 12);
    battle::State state; state.active = true; state.enemy_level = 3; state.turn = 4; state.reset(); assert(!state.active && state.enemy_hp == 80 && state.enemy_level == 1);
    collection::State collection; assert(collection.add_contract(1)); assert(!collection.add_contract(1)); assert(collection.contains(1));
    assert(collection.add_contract(2)); assert(collection.add_contract(3)); assert(!collection.add_contract(4)); assert(collection.party_count == 3);
    assert(collection.discover(1)); assert(collection.discovered_count() == 1); assert(!collection.discover(109));
    assert(data::Registry::integer("{\"power\":17}", "power", 0) == 17);
    assert(data::Registry::number("{\"multiplier\":1.5}", "multiplier", 1.0) == 1.5);
    assert(data::Registry::string_value("{\"label\":\"fight\"}", "label", "") == "fight");
    assert(battle_ui::move_command(0, -1, 3) == 2); assert(battle_ui::move_command(2, 1, 3) == 0);
    int ids[3]={1,2,3}; const auto valid_checksum=save_rules::checksum(save_rules::Version,1,2,100,30,1,0,3,ids,true,false,true); assert(valid_checksum==save_rules::checksum(save_rules::Version,1,2,100,30,1,0,3,ids,true,false,true)); assert(valid_checksum!=save_rules::checksum(save_rules::Version,1,2,99,30,1,0,3,ids,true,false,true));
    skills::State skills; skills.ids[0]=1; assert(skills.usable(0,0)); skills.sealed_turns[0]=1; assert(!skills.usable(0,0)); skills.tick(); assert(skills.usable(0,0));
    world::State world; assert(world.can_interact(15,7)); assert(!world.can_interact(1,1)); world.mark_shrine_seen(); assert(world.shrine_event_seen);
    return 0;
}

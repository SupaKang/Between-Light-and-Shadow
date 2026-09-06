#include "../src/battle_rules.h"
#include "../src/status_rules.h"
#include <cassert>
int main() {
    battle::Combatant a{12, 0, 100, 30, 10};
    battle::Combatant d{0, 5, 80, 0, 8};
    assert(battle::damage(a, d, 10, false) == 17);
    assert(battle::damage(a, d, 10, true) == 25);
    assert(!battle::can_capture(60, 80, 2));
    assert(battle::can_capture(40, 80, 2));
    assert(status_rules::tick_damage(status_rules::Kind::Burn, 0) == 3);
    assert(status_rules::skips_action(status_rules::Kind::Freeze, 2));
    assert(status_rules::adjusted_damage(status_rules::Kind::Fear, 10) == 15);
    return 0;
}

#include "world_state.h"
namespace world {
bool State::can_interact(int x,int y) const { return (x==15 && (y==7 || y==8)); }
void State::mark_shrine_seen() { shrine_event_seen=true; }
}

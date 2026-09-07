#include "skill_state.h"
namespace skills { bool State::usable(int slot,int qi) const { return slot>=0 && slot<MaxSlots && ids[slot]>0 && sealed_turns[slot]==0 && qi>=0; } void State::tick(){for(int& value:sealed_turns) if(value>0)--value;} }

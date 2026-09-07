#include "battle_ui.h"
namespace battle_ui { int move_command(int current, int direction, int count) { if(count<=0) return 0; int next=(current+direction)%count; return next<0?next+count:next; } }

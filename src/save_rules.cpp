#include "save_rules.h"
namespace save_rules { std::uint32_t checksum(std::uint32_t version,int x,int y,int hp,int qi,int level,int xp,int party,const int ids[3],bool contracted,bool artifact,bool shrine_seen){return 0x108u^version^static_cast<std::uint32_t>(x*31+y*17+hp*13+qi*11+level*9+xp*7+party*5+ids[0]*3+ids[1]*2+ids[2]+contracted+artifact*19+shrine_seen*23);} }

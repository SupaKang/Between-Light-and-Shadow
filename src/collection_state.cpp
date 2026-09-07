#include "collection_state.h"
namespace collection {
bool State::contains(int id) const { for (int i=0;i<party_count;++i) if (party_ids[i]==id) return true; return false; }
bool State::discover(int id) { if(id<1 || id>108) return false; discovered[id-1]=true; return true; }
int State::discovered_count() const { int count=0; for(bool value:discovered) if(value) ++count; return count; }
bool State::add_contract(int id) { if (id<=0 || party_count>=MaxParty || contains(id)) return false; party_ids[party_count++]=id; return true; }
}

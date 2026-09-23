// Game data definitions. Contents are baked from data/*.json into data_gen.h by tools/bake_data.py.
#pragma once
#include <cstdint>
#include <string>

namespace data {
enum class Element : std::uint8_t { Wood, Fire, Earth, Metal, Water };
bool overcomes(Element attacker, Element defender);  // 상극: Wood>Earth>Water>Fire>Metal>Wood
const char* element_hanja(Element e);                // "木" "火" "土" "金" "水"

struct YokaiDef {
    const char* id; int dex; const char* name_ko; const char* name_en; const char* source; bool korean;
    int grade; Element element; int hp, qi, atk, def, spd; const char* skills[4]; const char* trait;
};
struct SkillDef { const char* id; const char* name_ko; int power; int qi_cost; int accuracy; const char* status; };
struct ItemDef { const char* id; const char* name_ko; const char* desc; int price; bool usable_in_battle; };
struct ArtifactDef { const char* id; const char* name_ko; const char* buff; const char* debuff; };

const YokaiDef* find_yokai(const std::string& id);
const SkillDef* find_skill(const std::string& id);
const ItemDef* find_item(const std::string& id);
const ArtifactDef* find_artifact(const std::string& id);
int yokai_count();
const YokaiDef& yokai_at(int i);
int item_count();
const ItemDef& item_at(int i);
}  // namespace data

#include "data.h"
#include "data_gen.h"

namespace data {

bool overcomes(Element a, Element d) {
    // index = attacker; value = the element it overcomes
    static const Element beats[] = {Element::Earth, Element::Metal, Element::Water, Element::Wood, Element::Fire};
    return beats[(int)a] == d;
}

const char* element_hanja(Element e) {
    static const char* h[] = {"木", "火", "土", "金", "水"};
    return h[(int)e];
}

template <class T, size_t N>
static const T* find_in(const T (&arr)[N], const std::string& id) {
    for (const T& x : arr)
        if (id == x.id) return &x;
    return nullptr;
}

const YokaiDef* find_yokai(const std::string& id) { return find_in(kYokai, id); }
const SkillDef* find_skill(const std::string& id) { return find_in(kSkills, id); }
const ItemDef* find_item(const std::string& id) { return find_in(kItems, id); }
const ArtifactDef* find_artifact(const std::string& id) { return find_in(kArtifacts, id); }
int yokai_count() { return (int)(sizeof kYokai / sizeof *kYokai); }
const YokaiDef& yokai_at(int i) { return kYokai[i]; }
int item_count() { return (int)(sizeof kItems / sizeof *kItems); }
const ItemDef& item_at(int i) { return kItems[i]; }

}  // namespace data

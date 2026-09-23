# M1 엔진 기반 Implementation Plan

> **For agentic workers:** REQUIRED SUB-SKILL: Use superpowers:subagent-driven-development (recommended) or superpowers:executing-plans to implement this plan task-by-task. Steps use checkbox (`- [ ]`) syntax for tracking.

**Goal:** 현재 플레이 흐름(프롤로그 → 주막 → 마을 → 장승귀 격파)을 **코드에 박힌 맵·이벤트 없이** 데이터(Tiled 맵, JSON)와 Lua 스크립트로 돌리고, 저장·불러오기와 재사용 UI 위젯을 갖춘다.

**Architecture:** `src/game.cpp`(1,282줄)를 역할별 파일로 나누고, 렌더 결과 해시로 동작 보존을 검증한다. 데이터·맵·스크립트는 빌드 전에 Python 도구가 C++ 헤더로 굽는다(런타임 JSON 파서 없음, 단일 exe 유지). 이벤트는 Lua 5.4 코루틴으로 실행하며, `say`/`ask`/`battle` 호출이 엔진에 제어를 넘겼다가 결과를 받아 이어간다.

**Tech Stack:** C++17, SDL2, Lua 5.4.7(정적 링크), Python 3(빌드 도구), Tiled 1.10+(맵 편집, 개발자 도구).

**Spec:** `docs/superpowers/specs/2026-09-23-rebuild-design.md`, 로드맵 `docs/superpowers/plans/2026-09-24-full-game-roadmap.md`

## Global Constraints

- C++17, 오프라인, 브라우저·서버 의존성 없음, 단일 실행 파일 지향 (AGENTS.md)
- 논리 격자 20x11칸(16px, 월드 320x180), UI 640x360, 출력은 정수배
- 입력: Z·Space=A, X·Esc=B, Enter=메뉴. 대화는 Z·X·Enter로 넘김
- 휴식하면 무조건 다음 날 06:00, 게임 1분 = 실제 1초
- 요괴 속성은 오행 5종(`Wood`/`Fire`/`Earth`/`Metal`/`Water`), 상극 목→토→수→화→금→목
- 문서(`docs/`)와 코드(`src/`)는 항상 일치시킨다. 불확실한 것은 TODO/VERIFY로 표시한다
- 테스트는 `CoreTest`(규칙·플레이스루)와 새로 만드는 `RenderHashTest`(화면 회귀)로 돌린다: `cmake --build build/v1 -j && ./build/v1/CoreTest && ./build/v1/RenderHashTest`

## Review Focus

1. **옛 저장 파일 불러오기:** 키가 빠졌거나 모르는 맵 id가 들어 있는 저장은 기본값으로 채우거나 슬롯을 "손상됨"으로 표시하고, 크래시 없이 타이틀에 남아야 한다 (Task 7 테스트).
2. **Lua 스크립트 오류:** 오타가 난 이벤트는 오류를 로그에 남기고 토스트("이벤트 오류: <이름>")를 띄운 뒤, 플레이어에게 조작권을 돌려줘야 한다. 멈추면 안 된다 (Task 5 테스트).
3. **이벤트 중복 실행:** 대화 중이거나 이벤트 실행 중에 트리거 칸을 밟아도 두 번째 이벤트가 겹쳐 시작되면 안 된다. 예: 장승귀 전투가 두 번 열리는 것 (Task 5 테스트).
4. **없는 맵이나 좌표로 워프:** 맵 데이터 오류로 `warp`가 존재하지 않는 맵·벽 안을 가리키면 이동을 거부하고 로그를 남긴다 (Task 4 테스트).
5. **저장 시점 제한:** 전투, 장면 전환, 이벤트 실행 중에는 저장 메뉴가 비활성이어야 한다. 반쯤 진행된 상태가 저장되면 안 된다 (Task 7 테스트).

---

## File Structure (M1 이후)

| 파일 | 책임 |
|---|---|
| `src/state.h` / `state.cpp` | 공유 상태(`Game g`), 열거형, 공용 도우미(`rnd`, `hash3`, `clock_str`, `transition`, `show_toast`, `phase_of`, `rest`) |
| `src/world.cpp` | 맵 조회, 충돌, 타일 그리기, 필드 이동·카메라·등불, 배우(캐릭터) 그리기, 워프·트리거 |
| `src/ui.h` / `ui.cpp` | UI 위젯(`ListMenu`, `NumberPicker`), 대화창·토스트·커서·이름표·초상화 조회 |
| `src/menu.cpp` | 게임 메뉴 패널, 상점 |
| `src/battle.cpp` | 전투 상태·진행·렌더 (M2에서 규칙 교체) |
| `src/scenes.cpp` | 타이틀·프롤로그·장면 카드·조우 연출 |
| `src/script.h` / `script.cpp` | Lua 상태, 코루틴 이벤트 러너, 게임 API 등록 |
| `src/save.h` / `save.cpp` | 슬롯 저장·불러오기·요약 |
| `src/data.h` + 생성 `src/data_gen.h` | 요괴·기술·아이템·유물 정의 |
| 생성 `src/maps_gen.h` | 맵 타일·NPC·워프·트리거 |
| 생성 `src/scripts_gen.h` | Lua 소스 문자열 |
| `src/game.cpp` | 공개 API(`game_init/update/render/debug`)만 남긴다 |
| `tools/bake_data.py`, `tools/bake_maps.py`, `tools/bake_scripts.py`, `tools/ascii_to_tmj.py`, `tools/make_placeholder_tileset.py`, `tools/fetch_lua.sh` | 빌드 전 굽기와 도구 |
| `data/items.json`, `data/maps/*.tmj`, `data/maps/placeholder.tsj`, `data/scripts/*.lua` | 콘텐츠 |
| `tests/render_hash_test.cpp`, `tests/render_hashes.inc` | 화면 회귀 |

---

### Task 1: 화면 회귀 해시 테스트 (리팩터 안전망)

**Files:**
- Modify: `src/gfx.h`, `src/gfx.cpp` (해시 함수 추가)
- Create: `tests/render_hash_test.cpp`, `tests/render_hashes.inc`
- Modify: `CMakeLists.txt`

**Interfaces:**
- Produces: `std::uint64_t gfx::frame_hash();` 출력 버퍼(640x360) 전체의 FNV-1a 64비트 해시. `RenderHashTest [--print]`.

- [ ] **Step 1: 해시 함수 추가**

`src/gfx.h`의 `compose();` 선언 아래에 추가:
```cpp
std::uint64_t frame_hash();  // FNV-1a over the 640x360 output, for render regression tests
```
`src/gfx.cpp`의 `compose()` 정의 아래에 추가:
```cpp
std::uint64_t frame_hash() {
    std::uint64_t h = 1469598103934665603ull;
    for (Color c : out) { h ^= c; h *= 1099511628211ull; }
    return h;
}
```

- [ ] **Step 2: 테스트 작성**

`tests/render_hash_test.cpp`:
```cpp
// Render regression: every debug scene must hash to the recorded value.
// Refresh deliberately with:  ./build/v1/RenderHashTest --print > tests/render_hashes.inc
#include <cstdio>
#include <cstring>
#include "../src/game.h"
#include "../src/gfx.h"

struct Expect { const char* scene; unsigned long long hash; };
static const Expect kExpected[] = {
#include "render_hashes.inc"
};

static const char* kScenes[] = {"title", "prologue", "choice", "card", "wake", "village", "yard", "gate", "dusk",
                                "night", "talk", "rest", "menu", "shop", "encounter", "battle", "battle_list", "battle_msg"};

static unsigned long long render(const char* s) {
    game_init(1);
    game_debug_scene(s);
    Input none;
    for (int i = 0; i < 3; ++i) game_update(none);
    game_render();
    return gfx::frame_hash();
}

int main(int argc, char** argv) {
    if (argc > 1 && !std::strcmp(argv[1], "--print")) {
        for (const char* s : kScenes) std::printf("{\"%s\", 0x%016llxull},\n", s, render(s));
        return 0;
    }
    int bad = 0;
    for (const Expect& e : kExpected) {
        unsigned long long h = render(e.scene);
        if (h != e.hash) { std::printf("MISMATCH %s: got 0x%016llx\n", e.scene, h); ++bad; }
    }
    std::printf(bad ? "render_hash_test FAILED (%d)\n" : "render_hash_test ok\n", bad);
    return bad ? 1 : 0;
}
```
`tests/render_hashes.inc`는 빈 파일로 만든다.

`CMakeLists.txt`의 `add_test(NAME CoreTest ...)` 아래에 추가:
```cmake
add_executable(RenderHashTest tests/render_hash_test.cpp)
target_link_libraries(RenderHashTest PRIVATE core)
add_test(NAME RenderHashTest COMMAND RenderHashTest)
```

- [ ] **Step 3: 기준 해시 기록**

Run: `cmake --build build/v1 -j && ./build/v1/RenderHashTest --print > tests/render_hashes.inc && cmake --build build/v1 -j && ./build/v1/RenderHashTest`
Expected: `render_hash_test ok` (18개 장면이 기준과 일치)

- [ ] **Step 4: 결정성 확인**

Run: `./build/v1/RenderHashTest && ./build/v1/RenderHashTest`
Expected: 두 번 다 `render_hash_test ok`. 한 번이라도 다르면 `game_init(1)` 외의 비결정 요소(시간, 난수)를 찾아 고정한 뒤 Step 3을 다시 한다.

- [ ] **Step 5: Commit**
```bash
git add src/gfx.h src/gfx.cpp tests/render_hash_test.cpp tests/render_hashes.inc CMakeLists.txt
git commit -m "test: add render hash regression over all debug scenes"
```

---

### Task 2: `game.cpp` 모듈 분리 (동작 변경 없음)

**Files:**
- Create: `src/state.h`, `src/state.cpp`, `src/world.cpp`, `src/ui.h`, `src/ui.cpp`, `src/menu.cpp`, `src/battle.cpp`, `src/scenes.cpp`, `src/story.cpp`
- Modify: `src/game.cpp` (공개 API만 남김), `CMakeLists.txt` (`core` 라이브러리 소스 목록)

**Interfaces:**
- Consumes: Task 1의 `RenderHashTest`
- Produces: 내부 네임스페이스 `yy`와 `src/state.h`에 선언된 공용 함수. 이후 태스크가 쓰는 이름:
  `extern Game g;` · `void say(std::vector<std::string>, std::string speaker = "", std::function<void(int)> done = {}, std::vector<std::string> choices = {});` · `void show_toast(const std::string&);` · `void transition(std::function<void()>);` · `int rnd(int, int);` · `unsigned hash3(int, int, int);` · `std::string clock_str();` · `char tile_at(int map, int x, int y);` · `void start_battle();` · `void wake_up();`

- [ ] **Step 1: `src/state.h` 작성**

현재 `game.cpp`의 익명 네임스페이스에 있는 타입 정의를 그대로 옮긴다: `Dir`, `DX`, `DY`, `Warp`, `Npc`, `Map`, `ItemDef`/`ITEMS`/아이템 enum, `Scene`, `Dialog`, `Msg`, `Battle`, `Game`. 익명 네임스페이스는 `namespace yy`로 바꾼다. 파일 끝에는 위 **Produces** 목록의 함수 선언과 아래 선언을 둔다.
```cpp
#pragma once
#include <deque>
#include <functional>
#include <random>
#include <set>
#include <string>
#include <vector>
#include "art.h"
#include "art_gen.h"
#include "art_legacy.h"
#include "game.h"
#include "gfx.h"

namespace yy {
using namespace gfx;
// ... (game.cpp:34-168의 타입 정의를 그대로 이동) ...
extern Game g;
extern std::vector<Map> maps;
enum { ROOM, VILLAGE };
constexpr int kTrans = 18;
constexpr int kEnc = 48;
// Produces 목록의 선언 + 각 파일이 서로 부르는 함수 선언:
void update_field(const Input&); void render_field(); void render_actors();
void update_dialog(const Input&); void render_dialog(); void render_toast();
void update_menu(const Input&); void render_menu(); void update_shop(const Input&); void render_shop();
void begin_battle(); void update_battle(const Input&); void battle_world(); void battle_ui();
void title_world(); void title_ui(); void prologue_world(); void prologue_ui(); void card_ui(); void render_encounter();
void start_prologue(); void after_place_card(); void talk_npc(Npc&); void search(int, int); void build_maps();
}  // namespace yy
```

- [ ] **Step 2: 함수를 파일로 이동**

`game.cpp`의 함수를 아래 표대로 옮긴다. 함수 본문은 한 글자도 바꾸지 않는다.

| 대상 파일 | 옮길 함수 (현재 game.cpp 위치) |
|---|---|
| `state.cpp` | `Game g;` 정의, `phase_of`, `rest`(전역 유지), `say`, `show_toast`, `transition`, `rnd`, `hash3`, `clock_str` |
| `world.cpp` | `maps`, `build_maps`, `tile_at`, `solid_tile`, `draw_grass`, `draw_dirt`, `draw_floor`, `draw_tile`, `field_camera`, `phase_tint`, `kLampIn`/`kLampOut`, `lantern_lit`, `render_field`, `render_actors`, `on_arrive`, `npc_at`, `bump_edge`, `update_field`, `wake_up`, 타일 색 상수 `GR`/`EA`/`ST` |
| `ui.cpp` | UI 색 상수, `DX0` 등 대화창 상수, `cursor`, `more_mark`, `name_tag`, `portrait_of`, `render_dialog`, `render_toast`, `update_dialog` |
| `menu.cpp` | `MENU`, `kMenuN`, `MENU_ICON`, `header`, `update_menu`, `render_menu`, `SHOP_ITEMS`, `update_shop`, `render_shop` |
| `battle.cpp` | `bmsg`, `end_battle`, `enemy_turn`, `check_enemy_down`, `hit_enemy`, `Entry`, `battle_entries`, `battle_act`, `begin_battle`, `start_battle`, `update_battle`, `battle_world`, `battle_sprites`, `battle_ui` |
| `scenes.cpp` | `render_encounter`, `title_world`, `title_ui`, `prologue_world`, `prologue_ui`, `card_ui` |
| `story.cpp` | `start_prologue`, `after_place_card`, `talk_npc`, `give`, `search` |
| `game.cpp` (남김) | `game_init`, `game_update`, `game_render`, `game_debug_scene`, `game_debug_info` |

여러 파일이 쓰는 상수(`TXT`, `TXT_DIM`, `TXT_RED`, `TXT_LIGHT`, `DX0`, `DY0`, `DW`, `DH`)는 `ui.h`에 `inline constexpr`로 둔다.

- [ ] **Step 3: CMake 소스 목록 갱신**

`CMakeLists.txt`:
```cmake
add_library(core STATIC src/game.cpp src/gfx.cpp src/state.cpp src/world.cpp src/ui.cpp src/menu.cpp
                        src/battle.cpp src/scenes.cpp src/story.cpp)
```

- [ ] **Step 4: 빌드와 회귀 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -E "error|warning"; ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: 경고·오류 없음, `core_test ok`, `render_hash_test ok`. 해시가 하나라도 다르면 옮기는 과정에서 동작이 바뀐 것이므로 되돌려서 찾는다. 기준 해시를 갱신하지 않는다.

- [ ] **Step 5: 파일 크기 확인**

Run: `wc -l src/*.cpp | sort -n`
Expected: `game.cpp` 150줄 이하, 다른 파일 각각 400줄 이하.

- [ ] **Step 6: Commit**
```bash
git add src CMakeLists.txt
git commit -m "refactor: split game.cpp into state/world/ui/menu/battle/scenes/story"
```

---

### Task 3: 게임 데이터 굽기 (요괴·기술·아이템·유물)

**Files:**
- Create: `data/items.json`, `tools/bake_data.py`, `src/data.h`, 생성 `src/data_gen.h`
- Modify: `src/state.h`(하드코딩 `ITEMS` 제거), `src/menu.cpp`, `src/battle.cpp`, `src/story.cpp`(아이템 id 문자열 사용), `tests/core_test.cpp`, `docs/superpowers/specs/2026-09-23-rebuild-design.md`

**Interfaces:**
- Produces:
```cpp
namespace data {
enum class Element : std::uint8_t { Wood, Fire, Earth, Metal, Water };
bool overcomes(Element attacker, Element defender);   // 상극: Wood>Earth>Water>Fire>Metal>Wood
const char* element_hanja(Element e);                // "木" "火" "土" "金" "水"
struct YokaiDef { const char* id; int dex; const char* name_ko; const char* name_en; const char* source; bool korean;
                  int grade; Element element; int hp, qi, atk, def, spd; const char* skills[4]; const char* trait; };
struct SkillDef { const char* id; const char* name_ko; int power; int qi_cost; int accuracy; const char* status; };
struct ItemDef { const char* id; const char* name_ko; const char* desc; int price; bool usable_in_battle; };
struct ArtifactDef { const char* id; const char* name_ko; const char* buff; const char* debuff; };
const YokaiDef* find_yokai(const std::string& id); const SkillDef* find_skill(const std::string& id);
const ItemDef* find_item(const std::string& id);   const ArtifactDef* find_artifact(const std::string& id);
int yokai_count(); const YokaiDef& yokai_at(int i);  int item_count(); const ItemDef& item_at(int i);
}
```
- 게임 상태의 아이템은 `std::map<std::string, int> items`(id → 개수)로 바꾼다.

- [ ] **Step 1: `data/items.json` 작성**
```json
[
  {"id": "hanji", "name_ko": "한지", "desc": "부적을 쓰는 데 드는 종이. 부적 조제 재료.", "price": 5, "usable_in_battle": false},
  {"id": "cheongsimhwan", "name_ko": "청심환", "desc": "체력을 20 회복한다.", "price": 30, "usable_in_battle": true},
  {"id": "contract_talisman", "name_ko": "계약 부적", "desc": "약해진 요괴와 약조를 맺을 때 쓴다.", "price": 40, "usable_in_battle": true}
]
```

- [ ] **Step 2: 실패하는 테스트 작성**

`tests/core_test.cpp`의 `std::puts("core_test ok");` 바로 앞에 추가하고, 파일 위쪽에 `#include "../src/data.h"`를 넣는다.
```cpp
    // Baked game data: 108 yokai with five-phase elements, items with prices.
    assert(data::yokai_count() == 108);
    assert(data::find_yokai("yokai_076")->element == data::Element::Wood);   // 장승 목신
    assert(data::find_yokai("yokai_006")->element == data::Element::Water);  // 해태: fire ward
    assert(data::overcomes(data::Element::Metal, data::Element::Wood));
    assert(!data::overcomes(data::Element::Wood, data::Element::Metal));
    assert(data::find_item("contract_talisman")->price == 40);
    assert(data::find_item("nope") == nullptr);
    int korean = 0;
    for (int i = 0; i < data::yokai_count(); ++i) korean += data::yokai_at(i).korean;
    assert(korean == 76);  // AGENTS: 70% Korean folklore
```

- [ ] **Step 3: 실패 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -m1 error`
Expected: `data.h` 파일 없음 오류

- [ ] **Step 4: `tools/bake_data.py` 작성**
```python
#!/usr/bin/env python3
"""Bake data/*.json into src/data_gen.h. Run after editing data; CMake runs it before compiling."""
import glob, json

ELEM = {"Wood": "Wood", "Fire": "Fire", "Earth": "Earth", "Metal": "Metal", "Water": "Water"}
GRADE = {"I": 1, "II": 2, "III": 3, "IV": 4, "V": 5}
s = lambda v: json.dumps(v if v is not None else "", ensure_ascii=False)
out = ["// Generated by tools/bake_data.py from data/*.json. Do not edit.", "#pragma once", '#include "data.h"', "",
       "namespace data {", "inline const YokaiDef kYokai[] = {"]
for f in sorted(glob.glob("data/yokai_[0-9]*.json")):
    y = json.load(open(f, encoding="utf-8"))
    st = y["stats"]
    sk = (y["skills"] + ["", "", "", ""])[:4]
    out.append(f'    {{{s(y["id"])}, {y["dex_number"]}, {s(y["name_ko"])}, {s(y["name_en"])}, {s(y["folklore_source"])}, '
               f'{"true" if "KOREAN" in y["origin"] else "false"}, {GRADE[y["grade"]]}, Element::{ELEM[y["element"]]}, '
               f'{st["hp"]}, {st["qi"]}, {st["attack"]}, {st["defense"]}, {st["speed"]}, '
               f'{{{", ".join(s(x) for x in sk)}}}, {s(y.get("trait"))}}},')
out += ["};", "inline const SkillDef kSkills[] = {"]
for f in sorted(glob.glob("data/skill_[0-9]*.json")):
    k = json.load(open(f, encoding="utf-8"))
    out.append(f'    {{{s(k["id"])}, {s(k["name_ko"])}, {k.get("power", 0)}, {k.get("qi_cost", 0)}, {k.get("accuracy", 100)}, {s(k.get("status"))}}},')
out += ["};", "inline const ItemDef kItems[] = {"]
for it in json.load(open("data/items.json", encoding="utf-8")):
    out.append(f'    {{{s(it["id"])}, {s(it["name_ko"])}, {s(it["desc"])}, {it["price"]}, {"true" if it["usable_in_battle"] else "false"}}},')
out += ["};", "inline const ArtifactDef kArtifacts[] = {"]
for a in json.load(open("data/artifacts/artifacts_sample.json", encoding="utf-8")):
    out.append(f'    {{{s(a["id"])}, {s(a["name_ko"])}, {s(str(a.get("buff")))}, {s(str(a.get("debuff")))}}},')
out += ["};", "}  // namespace data", ""]
open("src/data_gen.h", "w", encoding="utf-8").write("\n".join(out))
print("baked data")
```

- [ ] **Step 5: `src/data.h`와 조회 함수**

`src/data.h`에 **Interfaces**의 선언을 그대로 쓰고, 조회 구현은 `src/data.cpp`에 둔다.
```cpp
#include "data.h"
#include "data_gen.h"
#include <cstring>
namespace data {
bool overcomes(Element a, Element d) {
    static const Element beats[] = {Element::Earth, Element::Metal, Element::Water, Element::Wood, Element::Fire};
    return beats[(int)a] == d;  // Wood>Earth, Fire>Metal, Earth>Water, Metal>Wood, Water>Fire
}
const char* element_hanja(Element e) { static const char* h[] = {"木", "火", "土", "金", "水"}; return h[(int)e]; }
template <class T, size_t N> static const T* find_in(const T (&arr)[N], const std::string& id) {
    for (const T& x : arr) if (id == x.id) return &x;
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
}
```
`CMakeLists.txt`에서 `core` 목록에 `src/data.cpp`를 추가하고, 빌드 전에 굽기를 실행한다.
```cmake
find_package(Python3 REQUIRED COMPONENTS Interpreter)
add_custom_target(bake ALL
  COMMAND ${Python3_EXECUTABLE} tools/bake_data.py
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  COMMENT "Baking data/*.json")
add_dependencies(core bake)
```

- [ ] **Step 6: 아이템 사용처 전환**

`state.h`의 `ITEMS`, `HANJI`/`CHEONGSIM`/`CONTRACT`/`ITEM_COUNT` enum과 `int items[ITEM_COUNT]`를 지우고, `std::map<std::string, int> items{{"cheongsimhwan", 1}};`로 바꾼다. `menu.cpp`(가방, 상점), `battle.cpp`(도구 목록, 사용), `story.cpp`(`give`)에서 enum 인덱스 대신 id 문자열과 `data::find_item(id)->name_ko`/`->desc`/`->price`를 쓴다. 상점 품목은 `const char* SHOP_ITEMS[] = {"cheongsimhwan", "contract_talisman"};`로 둔다.

- [ ] **Step 7: 테스트 통과 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -E "error|warning"; ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: `core_test ok`, `render_hash_test ok`. 아이템 이름과 가격이 JSON과 같으므로 화면은 바뀌지 않는다.

- [ ] **Step 8: 문서 갱신과 Commit**

설계 문서 "6. 구현 현황"에 "게임 데이터는 `tools/bake_data.py`가 빌드 시 `src/data_gen.h`로 굽는다"를 추가한다.
```bash
git add data/items.json tools/bake_data.py src/data.h src/data.cpp src/state.h src/menu.cpp src/battle.cpp src/story.cpp tests/core_test.cpp CMakeLists.txt docs/superpowers/specs/2026-09-23-rebuild-design.md
git commit -m "feat(data): bake yokai/skill/item/artifact JSON into C++ and use item ids"
```
(`src/data_gen.h`는 생성물이므로 `.gitignore`에 추가한다.)

---

### Task 4: Tiled 맵 굽기와 런타임 맵

**Files:**
- Create: `tools/make_placeholder_tileset.py`, `assets/tiles/placeholder.png`, `data/maps/placeholder.tsj`, `tools/ascii_to_tmj.py`, `data/maps/tavern_room.tmj`, `data/maps/village.tmj`, `tools/bake_maps.py`, `src/maps_api.h`(아래 Produces 선언), `src/maps.cpp`(`find_map`, `warp_valid`), 생성 `src/maps_gen.h`
- Modify: `src/state.h`(`Map`, `Npc`, `Warp`를 생성 구조로), `src/world.cpp`(`build_maps` 제거), `src/story.cpp`(`run_event`), `CMakeLists.txt`, `tests/core_test.cpp`
- Remove: `data/maps/sample_village.json` (옛 형식, 아무 코드도 읽지 않음)

**Interfaces:**
- Produces:
```cpp
struct NpcDef { const char* name; int x, y; char face; const char* sprite; const char* talk_event; };  // face: 'D','U','L','R'
struct WarpDef { int x, y; const char* to_map; int tx, ty; char face; };
struct TriggerDef { int x, y; const char* event; bool once; };
struct MapDef { const char* id; const char* name_ko; int w, h; bool indoor; const char* tiles;  // w*h tile-kind chars
                const NpcDef* npcs; int npc_n; const WarpDef* warps; int warp_n; const TriggerDef* triggers; int trig_n; };
const MapDef* find_map(const std::string& id);
bool warp_valid(const std::string& map, int x, int y);   // map exists, inside bounds, tile walkable
void run_event(const std::string& name, int arg_x = -1, int arg_y = -1);  // story.cpp, Lua in Task 5
```
- 타일 종류 문자는 지금과 같다: `# = B R J D , . T S h H d e W G`. 게임 상태의 `map`은 정수 대신 `std::string map_id`로 바꾼다.

- [ ] **Step 1: 임시 타일셋 만들기**

`tools/make_placeholder_tileset.py`: 16x16 칸 16개(종류마다 한 칸)를 가로로 붙인 256x16 PNG를 만든다. 각 칸에는 종류 문자를 큰 글씨로 쓰고 배경색으로 구분한다. 같은 스크립트가 `data/maps/placeholder.tsj`(Tiled 타일셋 JSON)도 쓴다. 각 타일에는 `properties: [{"name":"kind","type":"string","value":"<문자>"}]`를 넣는다.
```python
#!/usr/bin/env python3
"""Placeholder Tiled tileset: one 16x16 tile per tile-kind char, labelled. Graphics come later."""
import json
from PIL import Image, ImageDraw
KINDS = "#=BRJD,.TShHdeWG"
COLORS = {"#": "#2a1e18", "=": "#d4a860", "B": "#24345a", "R": "#6b4a32", "J": "#5a3a28", "D": "#b8322a", ",": "#6a9a44",
          ".": "#cfae7a", "T": "#3c5a2a", "S": "#8a8680", "h": "#c8a050", "H": "#d8c8a0", "d": "#a07850", "e": "#b8322a",
          "W": "#24345a", "G": "#a07850"}
im = Image.new("RGB", (16 * len(KINDS), 16)); d = ImageDraw.Draw(im)
for i, k in enumerate(KINDS):
    d.rectangle([i * 16, 0, i * 16 + 15, 15], fill=COLORS[k], outline="#000000")
    d.text((i * 16 + 5, 2), k, fill="#ffffff")
im.save("assets/tiles/placeholder.png")
ts = {"type": "tileset", "name": "placeholder", "tilewidth": 16, "tileheight": 16, "tilecount": len(KINDS), "columns": len(KINDS),
      "image": "../../assets/tiles/placeholder.png", "imagewidth": 16 * len(KINDS), "imageheight": 16,
      "tiles": [{"id": i, "properties": [{"name": "kind", "type": "string", "value": k}]} for i, k in enumerate(KINDS)]}
json.dump(ts, open("data/maps/placeholder.tsj", "w"), indent=1)
```

- [ ] **Step 2: 현재 맵을 `.tmj`로 변환**

`tools/ascii_to_tmj.py <id> <name_ko> <indoor 0/1> <rows.txt> <objects.json> <out.tmj>`: ASCII 행을 받아 타일 레이어 `ground`(gid = KINDS 인덱스 + 1)와 오브젝트 레이어 `objects`를 가진 Tiled 맵 JSON을 쓴다. 오브젝트 좌표는 픽셀(`x = tx*16`, `y = ty*16`)이고 `type`은 `npc` / `warp` / `trigger`, 속성은 아래와 같다.
```python
#!/usr/bin/env python3
"""Convert an ASCII tile-kind map + object list into a Tiled .tmj (one-off migration and quick blocking)."""
import json, sys
KINDS = "#=BRJD,.TShHdeWG"
map_id, name_ko, indoor, rows_path, objs_path, out = sys.argv[1:7]
rows = [r.rstrip("\n") for r in open(rows_path, encoding="utf-8") if r.strip()]
w, h = len(rows[0]), len(rows)
assert all(len(r) == w for r in rows), "ragged rows"
objs = json.load(open(objs_path, encoding="utf-8"))  # [{"type":"npc","x":5,"y":11,"props":{...}}, ...]
ptype = lambda v: "bool" if isinstance(v, bool) else "int" if isinstance(v, int) else "string"
tmj = {"type": "map", "orientation": "orthogonal", "renderorder": "right-down", "infinite": False,
       "width": w, "height": h, "tilewidth": 16, "tileheight": 16, "version": "1.10", "tiledversion": "1.10.2",
       "properties": [{"name": "id", "type": "string", "value": map_id}, {"name": "name_ko", "type": "string", "value": name_ko},
                      {"name": "indoor", "type": "bool", "value": indoor == "1"}],
       "tilesets": [{"firstgid": 1, "source": "placeholder.tsj"}],
       "layers": [{"type": "tilelayer", "name": "ground", "width": w, "height": h, "x": 0, "y": 0, "opacity": 1, "visible": True,
                   "data": [KINDS.index(c) + 1 for r in rows for c in r]},
                  {"type": "objectgroup", "name": "objects", "opacity": 1, "visible": True, "objects": [
                      {"id": i + 1, "type": o["type"], "name": o["props"].get("name", ""), "x": o["x"] * 16, "y": o["y"] * 16,
                       "width": 16, "height": 16, "visible": True,
                       "properties": [{"name": k, "type": ptype(v), "value": v} for k, v in o["props"].items()]}
                      for i, o in enumerate(objs)]}]}
json.dump(tmj, open(out, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
```
- npc: `name`, `face`, `sprite`, `talk_event`
- warp: `to_map`, `to_x`, `to_y`, `face`
- trigger: `event`, `once`

현재 `build_maps()` 내용에서 만든 두 맵:
- `tavern_room`: 행 = `room.rows`. 워프 (3,5) → `village` (6,9) `D`. 트리거 없음.
- `village`: 행 = `v.rows`. 워프 (6,8) → `tavern_room` (3,4) `U`. NPC `주모`(5,11,`D`, sprite `jumo`, talk `talk_jumo`), `노인`(11,3,`R`, sprite `village_elder`, talk `talk_elder`). 트리거 (12..15, 2)의 4칸 → `gate_jangseung`(once=true).

Run: `python3 tools/make_placeholder_tileset.py && python3 tools/ascii_to_tmj.py ...` (두 맵)
Expected: Tiled로 `data/maps/village.tmj`를 열면 지금 마을과 같은 배치가 보인다.

- [ ] **Step 3: 실패하는 테스트 작성**

`tests/core_test.cpp`에 추가(`#include "../src/maps_api.h"`, Produces의 선언을 담은 작은 헤더):
```cpp
    // Baked maps: same layout as the old in-code maps; bad warps are rejected.
    const MapDef* v = find_map("village");
    assert(v && v->w == 28 && v->h == 19 && !v->indoor);
    assert(v->tiles[1 * v->w + 11] == 'G');               // left jangseung
    assert(find_map("tavern_room")->indoor);
    assert(warp_valid("tavern_room", 3, 4));
    assert(!warp_valid("tavern_room", 0, 0));             // wall
    assert(!warp_valid("no_such_map", 1, 1));
    assert(!warp_valid("village", 99, 99));               // out of bounds
```

- [ ] **Step 4: `tools/bake_maps.py` 작성**

모든 `data/maps/*.tmj`를 읽어 `src/maps_gen.h`를 쓴다. 워프 대상 맵이 없거나 좌표가 범위 밖·벽이면 **굽기를 실패**시킨다(빌드 단계에서 데이터 오류를 잡기 위해서다). CMake의 `bake` 타깃에 추가한다.
```python
#!/usr/bin/env python3
"""Bake Tiled maps (data/maps/*.tmj) into src/maps_gen.h. Fails the build on broken warps."""
import glob, json, sys
SOLID = set("#BRJSThHWGe~")
ts = json.load(open("data/maps/placeholder.tsj", encoding="utf-8"))
kind = {t["id"] + 1: next(p["value"] for p in t["properties"] if p["name"] == "kind") for t in ts["tiles"]}
props = lambda o: {p["name"]: p["value"] for p in o.get("properties", [])}
maps = {}
for f in sorted(glob.glob("data/maps/*.tmj")):
    m = json.load(open(f, encoding="utf-8")); mp = props(m)
    ground = next(l for l in m["layers"] if l["name"] == "ground")
    objs = next((l["objects"] for l in m["layers"] if l["type"] == "objectgroup"), [])
    maps[mp["id"]] = dict(name=mp["name_ko"], indoor=mp.get("indoor", False), w=m["width"], h=m["height"],
                          tiles="".join(kind[g] for g in ground["data"]),
                          objs=[(o["type"], int(o["x"]) // 16, int(o["y"]) // 16, o.get("name", ""), props(o)) for o in objs])
errors = []
for mid, m in maps.items():
    for t, x, y, name, p in m["objs"]:
        if t != "warp":
            continue
        dst = maps.get(p["to_map"])
        if not dst:
            errors.append(f"{mid}: warp to unknown map {p['to_map']}"); continue
        tx, ty = p["to_x"], p["to_y"]
        if not (0 <= tx < dst["w"] and 0 <= ty < dst["h"]) or dst["tiles"][ty * dst["w"] + tx] in SOLID:
            errors.append(f"{mid}: warp target {p['to_map']}({tx},{ty}) is out of bounds or solid")
if errors:
    print("\n".join(errors)); sys.exit(1)
c = lambda v: json.dumps(v, ensure_ascii=False)
out = ["// Generated by tools/bake_maps.py from data/maps/*.tmj. Do not edit.", "#pragma once", '#include "maps_api.h"', ""]
table = []
for mid, m in maps.items():
    npcs = [f'{{{c(n)}, {x}, {y}, {c(p.get("face", "D"))}[0], {c(p.get("sprite", ""))}, {c(p.get("talk_event", ""))}}}'
            for t, x, y, n, p in m["objs"] if t == "npc"]
    warps = [f'{{{x}, {y}, {c(p["to_map"])}, {p["to_x"]}, {p["to_y"]}, {c(p.get("face", "D"))}[0]}}'
             for t, x, y, n, p in m["objs"] if t == "warp"]
    trigs = [f'{{{x}, {y}, {c(p["event"])}, {"true" if p.get("once") else "false"}}}'
             for t, x, y, n, p in m["objs"] if t == "trigger"]
    for arr, typ, items in (("npcs", "NpcDef", npcs), ("warps", "WarpDef", warps), ("trigs", "TriggerDef", trigs)):
        out.append(f"inline const {typ} {arr}_{mid}[] = {{{', '.join(items) or '{}'}}};")
    table.append(f'{{{c(mid)}, {c(m["name"])}, {m["w"]}, {m["h"]}, {"true" if m["indoor"] else "false"}, {c(m["tiles"])}, '
                 f'npcs_{mid}, {len(npcs)}, warps_{mid}, {len(warps)}, trigs_{mid}, {len(trigs)}}}')
out += ["inline const MapDef kMaps[] = {", ",\n".join("    " + t for t in table), "};", ""]
open("src/maps_gen.h", "w", encoding="utf-8").write("\n".join(out))
print(f"baked {len(maps)} maps")
```
`NpcDef` 등은 집합체(aggregate)로 두고, 빈 배열은 `{}` 한 개로 초기화하되 개수 필드(`npc_n` 등)로 실제 길이를 판단한다.

`src/maps.cpp`:
```cpp
#include "maps_api.h"
#include "maps_gen.h"
#include <string>
const MapDef* find_map(const std::string& id) {
    for (const MapDef& m : kMaps) if (id == m.id) return &m;
    return nullptr;
}
bool warp_valid(const std::string& map, int x, int y) {
    const MapDef* m = find_map(map);
    if (!m || x < 0 || y < 0 || x >= m->w || y >= m->h) return false;
    return std::string("#BRJSThHWGe~").find(m->tiles[y * m->w + x]) == std::string::npos;
}
```

- [ ] **Step 5: 런타임 전환**

- `world.cpp`에서 `build_maps()`와 `maps` 벡터를 지운다. `tile_at`은 `find_map(g.map_id)`의 `tiles`를 읽는다.
- NPC의 런타임 상태(방향)는 `std::map<std::string, Dir> npc_face`로 두고, 맵에 들어올 때 `NpcDef::face`로 초기화한다.
- 스프라이트는 `sprite` 이름으로 찾는다: `"jumo"` → `art::npc_jumo_field_*`, `"village_elder"` → `art::npc_village_elder_field_*`. `ui.cpp`에 `const Sprite* field_sprite(const std::string& name, Dir d)` 조회 표를 두고, 없으면 `nullptr`을 반환해 임시 그림(로드맵 A6)으로 그린다.
- 맵 식별자를 정수에서 문자열로 바꾼다: `g.map` → `std::string g.map_id`, `DebugInfo`의 `int map` → `std::string map`. `CoreTest` 플레이스루의 `info().map == ROOM`/`VILLAGE` 비교는 `"tavern_room"`/`"village"`로, `game_debug_scene`의 `village()` 도우미는 `g.map_id = "village"`로 바꾼다.
- `on_arrive`: 워프는 `warp_valid`를 통과할 때만 이동하고, 실패하면 `std::fprintf(stderr, "bad warp ...")` 후 무시한다. 트리거는 `run_event(t.event)`를 부르고, `once`면 `g.flags["trig:<map>:<x>:<y>"]=1`로 표시한다.
- `story.cpp`의 `run_event(name, x, y)`는 이름으로 분기해 기존 함수를 부른다: `talk_jumo` / `talk_elder` → `talk_npc`, `gate_jangseung` → 기존 장승 대사+전투, `search` → `search(x, y)`. Task 5에서 Lua로 대체한다.

- [ ] **Step 6: 테스트 통과 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -E "error|warning"; ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: `core_test ok`, `render_hash_test ok`(맵 내용이 같으므로 화면 해시도 같다).

- [ ] **Step 7: Commit**
```bash
git rm data/maps/sample_village.json
git add tools/make_placeholder_tileset.py tools/ascii_to_tmj.py tools/bake_maps.py assets/tiles/placeholder.png data/maps src tests CMakeLists.txt
git commit -m "feat(world): author maps in Tiled and bake them; validate warps at bake time"
```

---

### Task 5: Lua 이벤트 러너와 이벤트 이식

**Files:**
- Create: `tools/fetch_lua.sh`, `third_party/lua/` (lua 5.4.7 `src/*.c` `*.h`, `lua.c`·`luac.c` 제외), `src/script.h`, `src/script.cpp`, `tools/bake_scripts.py`, 생성 `src/scripts_gen.h`, `data/scripts/prologue.lua`, `data/scripts/village.lua`, `data/scripts/common.lua`
- Modify: `CMakeLists.txt`(`project(... LANGUAGES C CXX)`, `lua` 라이브러리), `src/story.cpp`(`run_event` → Lua), `src/ui.cpp`(대화 완료 시 재개), `src/battle.cpp`(전투 종료 시 재개), `src/world.cpp`(이벤트 중 입력 차단), `tests/core_test.cpp`

**Interfaces:**
- Consumes: `say`, `show_toast`, `transition`, `start_battle`, `warp_valid`, `find_map`, `data::find_item`, `rest`, `wake_up`
- Produces:
```cpp
void script_init();                                   // load all baked scripts into one lua_State
bool script_start(const std::string& event, int x = -1, int y = -1);  // false if unknown or one is already running
void script_resume(int value);                        // dialog choice / battle result (1 win, 0 lose)
bool script_busy();
std::string script_last_error();
```
Lua에서 쓰는 API (전부 전역 함수):

| 함수 | 동작 | 제어 |
|---|---|---|
| `say(speaker, text, ...)` | 대화 페이지 표시. speaker는 `""` 가능 | 끝까지 읽으면 이어감 |
| `ask(speaker, text, {"예","아니오"})` | 선택지 → 1부터 시작하는 번호 반환 | 선택하면 이어감 |
| `battle(enemy_id)` | 전투 시작 → `true`(승리) / `false`(패배) | 전투가 끝나면 이어감 |
| `flag(name)` / `set_flag(name, n)` | 정수 플래그 | 즉시 |
| `give(item_id, n)` / `take(item_id, n)` / `count(item_id)` | 가방 | 즉시 |
| `money(delta)` → 현재 엽전 | | 즉시 |
| `warp(map, x, y, face)` | 전환 연출 후 이동. 잘못된 목적지는 오류 | 전환 후 이어감 |
| `rest()` | 다음 날 06:00, 회복, 조사 오브젝트 초기화 | 전환 후 이어감 |
| `shop(item_ids_table)` | 상점 열기 | 상점을 닫으면 이어감 |
| `toast(text)` · `phase()` → `"dawn"/"day"/"dusk"/"night"` · `tile(x,y)` → 종류 문자 · `searched(x,y)` / `mark_searched(x,y)` · `face_player(npc_name)` · `scene(name)` | 보조 | 즉시 |

- [ ] **Step 1: Lua 받기**

`tools/fetch_lua.sh`:
```bash
#!/bin/sh
# Vendor Lua 5.4.7 sources (MIT) into third_party/lua. Pin the archive hash on first run.
set -e
cd "$(dirname "$0")/.."
curl -sSLo /tmp/lua.tgz https://www.lua.org/ftp/lua-5.4.7.tar.gz
shasum -a 256 /tmp/lua.tgz
mkdir -p third_party/lua && tar -xzf /tmp/lua.tgz -C /tmp
cp /tmp/lua-5.4.7/src/*.c /tmp/lua-5.4.7/src/*.h third_party/lua/
rm -f third_party/lua/lua.c third_party/lua/luac.c
cp /tmp/lua-5.4.7/doc/readme.html third_party/lua/LICENSE.html
```
Run: `sh tools/fetch_lua.sh`
Expected: SHA-256 한 줄 출력. 그 값을 스크립트의 `shasum` 줄 아래에 `echo "<값>  /tmp/lua.tgz" | shasum -a 256 -c` 검증 줄로 고정한다.

- [ ] **Step 2: CMake**
```cmake
project(YinYangChronicle LANGUAGES C CXX)
file(GLOB LUA_SRC third_party/lua/*.c)
add_library(lua STATIC ${LUA_SRC})
target_include_directories(lua PUBLIC third_party/lua)
if(NOT MSVC)
  target_compile_options(lua PRIVATE -w)
endif()
target_link_libraries(core PUBLIC lua)
```

- [ ] **Step 3: 실패하는 테스트 작성**

`tests/core_test.cpp`에 추가(`#include "../src/script.h"`):
```cpp
    // Lua events: a broken script reports an error and returns control; events never overlap.
    game_init(3);
    assert(!script_start("no_such_event"));
    assert(script_start("__test_error"));        // common.lua defines it to call an undefined function
    assert(!script_busy());
    assert(script_last_error().find("__test_error") != std::string::npos);
    assert(script_start("__test_ask"));          // asks one question, stores the answer in flag "t_ans"
    assert(script_busy());
    assert(!script_start("__test_ask"));         // second start while busy is refused
    script_resume(2);
    assert(!script_busy());
```
`data/scripts/common.lua`에는 테스트용 이벤트를 둔다(게임 흐름에서는 부르지 않는다).
```lua
function __test_error() undefined_function_for_test() end
function __test_ask() set_flag("t_ans", ask("", "테스트", {"가", "나"})) end
```

- [ ] **Step 4: `src/script.cpp` 구현**

핵심 구조:
```cpp
static lua_State* L = nullptr;
static lua_State* co = nullptr;   // running event coroutine (one at a time)
static std::string last_error;

static void finish(int status) {
    if (status == LUA_YIELD) return;                       // waiting for dialog/battle/transition
    if (status != LUA_OK) {
        last_error = lua_tostring(co, -1) ? lua_tostring(co, -1) : "unknown";
        std::fprintf(stderr, "script error: %s\n", last_error.c_str());
        yy::show_toast("이벤트 오류");
    }
    co = nullptr;
}

bool script_start(const std::string& event, int x, int y) {
    if (co) return false;                                  // Review Focus 3: never overlap
    co = lua_newthread(L);
    lua_getglobal(co, event.c_str());
    if (!lua_isfunction(co, -1)) { lua_pop(co, 1); co = nullptr; return false; }
    lua_pushinteger(co, x); lua_pushinteger(co, y);
    int nres = 0;
    finish(lua_resume(co, L, 2, &nres));
    return true;
}

void script_resume(int value) {
    if (!co) return;
    lua_pushinteger(co, value);
    int nres = 0;
    finish(lua_resume(co, L, 1, &nres));
}

bool script_busy() { return co != nullptr; }
```
`say`/`ask`는 `yy::say(...)`로 대화를 만들고, `done` 콜백에서 `script_resume(choice + 1)`을 부른 뒤 yield한다.
```cpp
static int l_say(lua_State* s) {
    std::string who = luaL_checkstring(s, 1);
    std::vector<std::string> pages;
    for (int i = 2; i <= lua_gettop(s); ++i) pages.push_back(luaL_checkstring(s, i));
    yy::say(pages, who, [](int) { script_resume(0); });
    return lua_yield(s, 0);
}
static int l_ask(lua_State* s) {
    std::string who = luaL_checkstring(s, 1), text = luaL_checkstring(s, 2);
    luaL_checktype(s, 3, LUA_TTABLE);
    std::vector<std::string> choices;
    for (lua_Integer i = 1; i <= (lua_Integer)luaL_len(s, 3); ++i) {
        lua_geti(s, 3, i); choices.push_back(luaL_checkstring(s, -1)); lua_pop(s, 1);
    }
    yy::say({text}, who, [](int c) { script_resume(c + 1); }, choices);
    return lua_yield(s, 0);   // the resumed value (choice number) becomes ask()'s return value
}
static int l_battle(lua_State* s) {
    yy::g.battle_enemy = luaL_checkstring(s, 1);
    yy::start_battle();                    // end_battle(won) calls script_resume(won ? 1 : 0)
    return lua_yield(s, 0);
}
```
`battle`의 결과는 재개 값(정수)이므로 Lua에서 `if battle("jangseung") == 1 then ... end`로 쓴다. 등록은 `lua_register(L, "say", l_say);` 식이다. `Game`에 `std::string battle_enemy;`를 추가한다. `battle`은 `yy::start_battle()`을 부르고 `end_battle(won)`이 `script_resume(won)`을 부른다. `warp`는 `warp_valid`가 실패하면 `luaL_error`로 오류를 낸다(Review Focus 4). 나머지 즉시 함수는 `g`를 직접 읽고 쓴다. `script_init()`은 `scripts_gen.h`의 각 소스를 `luaL_loadbuffer` + `lua_pcall`로 한 번 싣는다. `game_init`에서 `script_init()`을 부른다.

- [ ] **Step 5: 스크립트 굽기**

`tools/bake_scripts.py`: `data/scripts/*.lua`를 읽어 `inline const char* const kScripts[][2] = {{"prologue.lua", R"LUA(...)LUA"}, ...};`로 `src/scripts_gen.h`에 쓴다. 파일 안에 `)LUA"`가 있으면 굽기를 실패시킨다. CMake `bake` 타깃에 추가한다.

- [ ] **Step 6: 이벤트 이식**

`story.cpp`의 대사와 분기를 Lua로 옮긴다. 대사 문자열은 한 글자도 바꾸지 않는다(화면 해시 유지).
- `data/scripts/prologue.lua`: `prologue()`는 기존 `start_prologue`의 두 번의 `ask`와 반응, 마지막에 `scene("placecard")`를 수행한다. `wake()`는 기상 독백(화자 `음양사`)과 `toast("임무: 마을 어귀의 요괴를 퇴치하라")`를 수행한다.
- `data/scripts/village.lua`: `talk_jumo()`, `talk_elder()`(퀘스트 플래그와 `phase()=="night"` 분기), `gate_jangseung()`(대사 → `battle("jangseung")` → 승리 시 `set_flag("quest_gate", 1)`와 완료 대사, 패배 시 `rest()`와 기상 대사), `search(x, y)`(`tile(x,y)` 종류별 분기, 뒤주·항아리·우물·이부자리·부적방·장승·돌담·초가), `shop_talisman()`.
- `story.cpp`의 `run_event`는 `script_start(name, x, y)`만 부른다. 기존 C++ 이벤트 함수(`start_prologue`, `after_place_card`, `talk_npc`, `give`, `search`)는 삭제한다.
- 입력 차단: `update_field`는 `script_busy()`일 때 이동·조사·메뉴 입력을 무시한다.

- [ ] **Step 7: 테스트 통과 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -E "error|warning"; ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: `core_test ok`(기존 플레이스루 포함), `render_hash_test ok`.

- [ ] **Step 8: Commit**
```bash
git add tools/fetch_lua.sh tools/bake_scripts.py third_party/lua data/scripts src tests CMakeLists.txt .gitignore
git commit -m "feat(script): run story events as Lua coroutines; port prologue and village events"
```

---

### Task 6: UI 위젯 (스크롤 목록, 수량 선택)

**Files:**
- Modify: `src/ui.h`, `src/ui.cpp`, `src/menu.cpp`(메뉴·가방·상점), `src/scenes.cpp`(타이틀), `tests/core_test.cpp`, `tests/render_hashes.inc`(의도한 변경만)

**Interfaces:**
- Produces:
```cpp
struct ListMenu {
    std::vector<std::string> items;
    std::vector<bool> enabled;          // empty = all enabled
    int sel = 0, top = 0, rows = 6;     // visible rows
    // Returns true when the player confirmed or cancelled; chosen = index or -1 on cancel.
    bool update(const Input& in, int& chosen);
    void render(int x, int y, int w, const std::vector<std::string>& right = {}) const;  // right-aligned column
};
struct NumberPicker { int value = 1, min = 1, max = 99; bool update(const Input& in, int& chosen); void render(int x, int y) const; };
```

- [ ] **Step 1: 실패하는 테스트 작성**

`tests/core_test.cpp`에 추가:
```cpp
    // ListMenu: wraps, skips disabled rows, scrolls, B cancels.
    {
        yy::ListMenu m; m.items = {"a", "b", "c", "d", "e", "f", "g", "h"}; m.enabled = {1, 0, 1, 1, 1, 1, 1, 1}; m.rows = 3;
        Input down; down.pressed[K_DOWN] = true;
        Input up; up.pressed[K_UP] = true;
        Input a; a.pressed[K_A] = true;
        Input b; b.pressed[K_B] = true;
        int c = 99;
        m.update(down, c); assert(m.sel == 2);          // skips disabled "b"
        m.update(up, c); m.update(up, c); assert(m.sel == 7 && m.top == 5);  // wraps to the end and scrolls
        assert(m.update(a, c) && c == 7);
        assert(m.update(b, c) && c == -1);
    }
    {
        yy::NumberPicker p; p.max = 5;
        Input up; up.pressed[K_UP] = true;
        int c = 0;
        for (int i = 0; i < 9; ++i) p.update(up, c);
        assert(p.value == 5);                             // clamps at max
    }
```

- [ ] **Step 2: 실패 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -m1 error`
Expected: `ListMenu` 미정의 오류

- [ ] **Step 3: 구현**
```cpp
bool ListMenu::update(const Input& in, int& chosen) {
    int n = (int)items.size();
    auto ok = [&](int i) { return enabled.empty() || enabled[i]; };
    auto step = [&](int d) { for (int k = 0; k < n; ++k) { sel = (sel + d + n) % n; if (ok(sel)) break; } };
    if (in.pressed[K_DOWN]) step(1);
    if (in.pressed[K_UP]) step(-1);
    if (sel < top) top = sel;
    if (sel >= top + rows) top = sel - rows + 1;
    if (in.pressed[K_A] && ok(sel)) { chosen = sel; return true; }
    if (in.pressed[K_B]) { chosen = -1; return true; }
    return false;
}
void ListMenu::render(int x, int y, int w, const std::vector<std::string>& right) const {
    for (int i = top; i < std::min((int)items.size(), top + rows); ++i) {
        int ry = y + (i - top) * 20;
        bool on = enabled.empty() || enabled[i];
        if (i == sel) cursor(x, ry + 3);
        text(x + 16, ry, items[i], on ? TXT : TXT_DIM);
        if (i < (int)right.size()) text(x + w - text_width(right[i]), ry, right[i], on ? TXT : TXT_DIM);
    }
    if (top > 0) more_up(x + w / 2, y - 8);
    if (top + rows < (int)items.size()) more_mark(x + w / 2, y + rows * 20);
}
bool NumberPicker::update(const Input& in, int& chosen) {
    if (in.pressed[K_UP] || in.pressed[K_RIGHT]) value = std::min(max, value + (in.pressed[K_RIGHT] ? 10 : 1));
    if (in.pressed[K_DOWN] || in.pressed[K_LEFT]) value = std::max(min, value - (in.pressed[K_LEFT] ? 10 : 1));
    if (in.pressed[K_A]) { chosen = value; return true; }
    if (in.pressed[K_B]) { chosen = -1; return true; }
    return false;
}
```
`more_up`은 `more_mark`를 위아래로 뒤집은 삼각형이다(`ui.cpp`에 추가).

- [ ] **Step 4: 메뉴·가방·상점·타이틀을 위젯으로 전환**

- 메뉴 7항목, 타이틀(`새로 시작`/`이어하기`, Task 7 전까지는 이어하기 비활성)을 `ListMenu`로 바꾼다.
- 가방 패널은 아이템이 7개를 넘으면 스크롤한다.
- 상점은 품목 선택 → `NumberPicker`(최대 = 소지금 / 가격) → 구매로 바꾼다.

- [ ] **Step 5: 의도한 화면 변경만 반영**

Run: `cmake --build build/v1 -j && ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: `core_test ok`. `RenderHashTest`는 `title`, `menu`, `shop`만 MISMATCH로 나와야 한다. 그 세 화면을 `--shot`으로 캡처해 눈으로 확인한 뒤 `./build/v1/RenderHashTest --print > tests/render_hashes.inc`로 갱신한다. 다른 장면이 바뀌었다면 버그다.

- [ ] **Step 6: Commit**
```bash
git add src tests
git commit -m "feat(ui): ListMenu and NumberPicker widgets; menu, bag, shop, title use them"
```

---

### Task 7: 저장·불러오기 (슬롯 3개)

**Files:**
- Create: `src/save.h`, `src/save.cpp`
- Modify: `src/main.cpp`(저장 경로), `src/menu.cpp`(기록 패널), `src/scenes.cpp`(타이틀 이어하기·슬롯 선택), `src/state.h`(`playtime`, `flags` 맵), `tests/core_test.cpp`, `tests/render_hashes.inc`

**Interfaces:**
- Consumes: `ListMenu`, `find_map`, `warp_valid`
- Produces:
```cpp
struct SaveInfo { bool exists = false; bool corrupt = false; std::string map_name; int day = 0; int playtime_min = 0; };
void save_set_dir(const std::string& dir);   // main.cpp: SDL_GetPrefPath("supakang", "YinYangChronicle")
bool save_allowed();                         // false during battle, transition, running script, dialog
bool save_game(int slot);                    // slot 0..2 → <dir>/slot<N>.sav
bool load_game(int slot);                    // false on missing/corrupt; game state untouched on failure
SaveInfo save_info(int slot);
```
- 형식 (한 줄에 `key=value`, UTF-8):
```
version=1
map=village
x=13
y=9
dir=D
day=2
minute=412
hp=34
ng=15
level=1
money=80
playtime=37
item.cheongsimhwan=2
flag.quest_gate=1
flag.trig:village:12:2=1
```

- [ ] **Step 1: 실패하는 테스트 작성**

`tests/core_test.cpp`에 추가:
```cpp
    // Save/load: round trip, refuse unsafe moments, survive corrupt or old files.
    {
        std::string dir = "/tmp/yyc_save_test";
        std::system(("rm -rf " + dir + " && mkdir -p " + dir).c_str());
        yy::save_set_dir(dir);
        game_init(5);
        game_debug_scene("village");
        assert(yy::save_allowed());
        assert(yy::save_game(0));
        DebugInfo before = game_debug_info();
        game_init(6);
        assert(yy::load_game(0));
        DebugInfo after = game_debug_info();
        assert(after.map == before.map && after.x == before.x && after.y == before.y && after.quest == before.quest);

        game_debug_scene("battle");
        assert(!yy::save_allowed());                          // Review Focus 5

        std::FILE* f = std::fopen((dir + "/slot1.sav").c_str(), "w");
        std::fputs("version=1\nmap=no_such_map\nx=1\ny=1\n", f); std::fclose(f);
        assert(!yy::load_game(1) && yy::save_info(1).corrupt);  // Review Focus 1: unknown map

        f = std::fopen((dir + "/slot2.sav").c_str(), "w");
        std::fputs("version=1\nmap=village\nx=13\ny=9\n", f); std::fclose(f);  // old file: most keys missing
        assert(yy::load_game(2));                              // defaults fill the rest
        assert(!yy::save_info(0).corrupt && yy::save_info(0).exists);
    }
```
`DebugInfo::map`은 Task 4에서 이미 `std::string`이다.

- [ ] **Step 2: 실패 확인**

Run: `cmake --build build/v1 -j 2>&1 | grep -m1 error`
Expected: `save.h` 없음 오류

- [ ] **Step 3: 구현**

`save_game`: 위 형식으로 `<dir>/slotN.sav.tmp`에 쓴 뒤 `std::rename`으로 교체한다(쓰는 중에 꺼져도 기존 저장이 남도록).
```cpp
bool load_game(int slot) {
    std::ifstream in(path(slot));
    if (!in) return false;
    Game t = Game{};                         // defaults for keys an older save does not have
    t.rng = g.rng;
    std::string line; int version = 0;
    while (std::getline(in, line)) {
        auto eq = line.find('=');
        if (eq == std::string::npos) continue;
        std::string k = line.substr(0, eq), v = line.substr(eq + 1);
        auto num = [&] { return std::atoi(v.c_str()); };
        if (k == "version") version = num();
        else if (k == "map") t.map_id = v;
        else if (k == "x") t.px = num();
        else if (k == "y") t.py = num();
        else if (k == "dir") t.dir = v == "U" ? UP : v == "L" ? LEFT : v == "R" ? RIGHT : DOWN;
        else if (k == "day") t.clock.day = num();
        else if (k == "minute") t.clock.minute = (float)num();
        else if (k == "hp") t.hp = num();
        else if (k == "ng") t.ng = num();
        else if (k == "level") t.level = num();
        else if (k == "money") t.money = num();
        else if (k == "playtime") t.playtime_min = num();
        else if (k.rfind("item.", 0) == 0) t.items[k.substr(5)] = num();
        else if (k.rfind("flag.", 0) == 0) t.flags[k.substr(5)] = num();
    }
    if (version != 1 || !warp_valid(t.map_id, t.px, t.py)) return false;  // game state untouched
    t.scene = Scene::Field;
    t.last_phase = phase_of(t.clock.minute);
    t.quest = t.flags["quest_gate"];
    g = std::move(t);
    return true;
}
```
`load_game`: 한 줄씩 `key=value`를 파싱해 **임시 Game 복사본**에 적용한다. 다음 경우에는 실패로 처리하고 원래 상태를 건드리지 않는다.
- `version`이 없거나 1보다 큼
- `map`이 `find_map`에 없음
- `(x, y)`가 `warp_valid`를 통과하지 못함

없는 키는 `Game{}` 기본값을 쓴다. 성공하면 `g`에 복사하고 `g.scene = Scene::Field`, `last_phase` 재계산.
`save_info`: 파일이 없으면 `exists=false`, 파싱이나 검증에 실패하면 `corrupt=true`.
`save_allowed`: `g.scene == Scene::Field && g.trans_t < 0 && g.enc_t == 0 && g.dq.empty() && !script_busy() && !g.moving`.
`playtime`은 `game_update`에서 필드·메뉴 프레임을 세어 분 단위로 저장한다.

- [ ] **Step 4: UI 연결**

- 메뉴 `기록`: `ListMenu`로 슬롯 3개(`1. 도선사 고개 · 2일째 · 0:37` / `비어 있음` / `손상됨`)를 보여준다. `save_allowed()`가 거짓이면 항목을 비활성으로 한다. 덮어쓰기는 `예/아니오`로 확인한다.
- 타이틀 `이어하기`: 하나라도 `exists && !corrupt`이면 활성화한다. 누르면 슬롯 목록을 띄우고, 불러오면 장소 카드 없이 필드로 들어간다.
- `main.cpp`: `SDL_Init` 뒤에 `char* p = SDL_GetPrefPath("supakang", "YinYangChronicle"); yy::save_set_dir(p ? p : "."); SDL_free(p);`

- [ ] **Step 5: 테스트 통과와 화면 확인**

Run: `cmake --build build/v1 -j && ./build/v1/CoreTest && ./build/v1/RenderHashTest`
Expected: `core_test ok`. `RenderHashTest`는 `title`, `menu`만 다를 수 있다(이어하기 상태, 기록 패널). `--shot`으로 확인한 뒤 `--print`로 갱신한다.

- [ ] **Step 6: 실제 게임 확인**

Run: `./build/v1/YinYangChronicle`
확인할 것:
1. 마을에서 메뉴 → 기록 → 슬롯 1에 저장한다.
2. 게임을 종료한 뒤 다시 실행한다.
3. 타이틀 → 이어하기 → 슬롯 1을 고르면 같은 위치·시간·소지품으로 돌아온다.

- [ ] **Step 7: 문서와 Commit**

설계 문서 "6. 구현 현황"에 저장 형식과 경로, 로드맵 1장 상태표를 갱신한다.
```bash
git add src tests docs
git commit -m "feat(save): three save slots with atomic writes, corrupt/old file handling, title continue"
```

---

## M1 완료 확인

- [ ] `cmake --build build/v1 -j && ./build/v1/CoreTest && ./build/v1/RenderHashTest`가 모두 통과한다.
- [ ] `grep -n "주모\|노인\|장승" src/*.cpp`에 대사 문자열이 없다(전부 `data/scripts/*.lua`로 이동).
- [ ] `grep -n "rows = {" src/*.cpp`에 맵 행 데이터가 없다(전부 `data/maps/*.tmj`로 이동).
- [ ] 실제 게임에서 프롤로그 → 주막 → 상점 → 장승귀 → 저장 → 종료 → 이어하기가 된다.
- [ ] `docs/superpowers/specs/2026-09-23-rebuild-design.md`와 로드맵의 현재 상태가 코드와 일치한다.

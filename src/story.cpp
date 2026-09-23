#include "ui.h"

namespace yy {

void start_prologue() {
    g.scene = Scene::Prologue;
    say({"……어서 오게. 밤길이 차지. 등불 곁으로 앉게나.",
         "옛날 옛적, 하늘과 땅의 기운이 아직 제 길을 찾지 못하던 때의 일이오.",
         "산천의 영맥이 뒤엉켜, 백팔 가지 요괴가 밤낮을 가리지 않고 날뛰었지.",
         "자네, 혹시 이 이야기를 들어 본 적 있는가?"},
        "노승", [](int c) {
            say({c == 0 ? "허허, 그렇다면 끝까지 들어 보게. 사람들이 잊은 대목이 있으니."
                        : "그렇겠지. 요즘 사람들은 이런 옛이야기를 잘 하지 않으니.",
                 "그때 이름 없는 음양사 하나가 나타났소. 그는 요괴를 베지 않고, 하나하나 달래어 약조를 맺었다오.",
                 "백팔 요괴와 맺은 약조를 한 장의 부적에 새기니, 그것이 천지음양부(天地陰陽符)라.",
                 "부적이 영맥을 붙들자 세상은 낮과 밤을 되찾았고, 관상감이 대대로 그것을 지켜왔지.",
                 "……허나 요즘 그 부적이 찢어졌다는 소문이 돌더이다. 그래, 자네는 어디로 가는 길인가?"},
                "노승", [](int c2) {
                    say({c2 == 0 ? "도선사라… 붉은 안개가 낀 곳이지. 등불을 꺼뜨리지 말게."
                                 : "말이 없는 이가 멀리 가는 법이지. 몸조심하게.",
                         "자네가 이 이야기의 다음 장이 되겠구먼."},
                        "노승", [](int) { transition([] { g.scene = Scene::PlaceCard; g.card_t = 0; }); });
                }, {"도선사로 갑니다", "(말없이 고개를 숙인다)"});
        }, {"들어 보았습니다", "처음 듣습니다"});
}

void after_place_card() {
    transition([] {
        wake_up();
        say({"(……꿈이었나. 등불 아래 노승이 옛이야기를 들려주던.)",
             "(벽사청의 명을 받아 도선사로 향하는 길이다. 오늘은 채비를 갖추고 길을 나서야 한다.)"},
            "음양사", [](int) { show_toast("임무: 마을 어귀의 요괴를 퇴치하라"); });
    });
}

void talk_npc(Npc& n) {
    n.face = (Dir)(g.dir == DOWN ? UP : g.dir == UP ? DOWN : g.dir == LEFT ? RIGHT : LEFT);
    bool night = phase_of(g.clock.minute) == Phase::Night;
    if (n.name == "주모") {
        if (g.quest == 0)
            say({"일어났구려, 나리. 벽사청에서 오셨다지요?",
                 "요 며칠 마을 어귀 장승에서 검붉은 기운이 새어 나와요. 밤이면 장승이 걸어 다닌다는 소문까지 돌고…",
                 "길 떠나시기 전에 저 건너 부적방에 들러 채비를 하시구려. 어귀는 북쪽 큰길 끝이오."},
                n.name);
        else
            say({"어귀가 조용해졌어요! 이제야 발 뻗고 자겠네.", "고단하면 방에서 쉬어 가시오. 방값은 벌써 치렀으니."}, n.name);
    } else {
        if (g.quest == 0) {
            std::vector<std::string> t = {"거기 서게. 저 장승이 요사스러워진 뒤로 아무도 어귀를 못 지나다니네.",
                                          "자네가 음양사라면… 부디 장승 할아범을 달래 주게."};
            if (night) t.push_back("…보게, 밤이 되니 장승 눈이 붉게 번들거리지 않나.");
            say(t, n.name);
        } else {
            say({"장승이 제 모습을 찾았구먼. 고맙네, 젊은 음양사.", "도선사로 가려거든 이 길로 북쪽 고개를 넘게."}, n.name);
        }
    }
}

void give(int item, int n, const std::string& msg) {
    g.items[item] += n;
    say({msg});
}

void search(int x, int y) {
    char c = tile_at(g.map, x, y);
    long key = (long)g.map * 100000 + y * 1000 + x;
    bool again = g.searched.count(key) > 0;
    bool night = phase_of(g.clock.minute) == Phase::Night;
    switch (c) {
        case 'R':
            if (again) return say({"쌀이 반쯤 차 있는 뒤주다."});
            g.searched.insert(key);
            return give(HANJI, 2, "뒤주 안 쌀 사이에서 한지 2장을 찾았다!");
        case 'J': {
            if (again) return say({"이미 들여다본 항아리다."});
            g.searched.insert(key);
            unsigned h = hash3(g.clock.day, x, y) % 3;
            if (h == 0) { g.money += 10; return say({"항아리 바닥에서 엽전 10냥을 찾았다!"}); }
            if (h == 1) return give(CHEONGSIM, 1, "항아리 속에 청심환 1개가 숨겨져 있었다!");
            return say({"장 냄새만 가득하다. 아무것도 없다."});
        }
        case 'W':
            if (night) return say({"우물 속 깊은 곳에서 누군가 이름을 부르는 것 같다……", "(밤에는 물가를 조심해야겠다.)"});
            return say({"맑은 우물이다. 두레박이 걸려 있다."});
        case 'B':
            return say({"이부자리가 깔려 있다. 쉬어 갈까?"}, "", [](int c) {
                if (c != 0) return;
                transition([] {
                    rest(g.clock);
                    g.searched.clear();
                    g.hp = g.hp_max; g.ng = g.ng_max;
                    g.last_phase = phase_of(g.clock.minute);
                    show_toast(clock_str() + " — 몸이 가뿐하다");
                });
            }, {"쉰다", "그만둔다"});
        case 'e':
            return say({"어서 오시오. 부적방이오. 무얼 드릴까?"}, "부적방", [](int) { g.shop = true; g.shop_sel = 0; });
        case 'G':
            if (g.quest == 0) return start_battle();
            return say({"요기가 걷힌 장승이 묵묵히 마을 어귀를 지키고 있다."});
        case 'S': return say({"이끼 낀 돌담이다."});
        case 'H': case 'h': return say({"초가집이다."});
        default: break;
    }
}

}  // namespace yy

-- 한양 외곽 주막 마을: NPC talk, the village-gate jangseung (main quest 1), searchable objects.

function talk_jumo()
  if flag("quest_gate") == 0 then
    say("주모",
      "일어났구려, 나리. 벽사청에서 오셨다지요?",
      "요 며칠 마을 어귀 장승에서 검붉은 기운이 새어 나와요. 밤이면 장승이 걸어 다닌다는 소문까지 돌고…",
      "길 떠나시기 전에 저 건너 부적방에 들러 채비를 하시구려. 어귀는 북쪽 큰길 끝이오.")
  else
    say("주모", "어귀가 조용해졌어요! 이제야 발 뻗고 자겠네.", "고단하면 방에서 쉬어 가시오. 방값은 벌써 치렀으니.")
  end
end

function talk_elder()
  if flag("quest_gate") ~= 0 then
    say("노인", "장승이 제 모습을 찾았구먼. 고맙네, 젊은 음양사.", "도선사로 가려거든 이 길로 북쪽 고개를 넘게.")
  elseif phase() == "night" then
    say("노인", "거기 서게. 저 장승이 요사스러워진 뒤로 아무도 어귀를 못 지나다니네.",
      "자네가 음양사라면… 부디 장승 할아범을 달래 주게.",
      "…보게, 밤이 되니 장승 눈이 붉게 번들거리지 않나.")
  else
    say("노인", "거기 서게. 저 장승이 요사스러워진 뒤로 아무도 어귀를 못 지나다니네.",
      "자네가 음양사라면… 부디 장승 할아범을 달래 주게.")
  end
end

local function fight_jangseung()
  say("", "장승에 서린 요기가 꿈틀거린다……!")
  if battle("jangseung") == 1 then
    set_flag("quest_gate", 1)
    say("", "장승의 눈빛이 순하게 가라앉았다.", "임무 완료: 마을 어귀의 요괴 퇴치", "(북쪽 도선사 고개로 가는 길이 열렸다.)")
  else
    rest()
    warp("tavern_room", 1, 2, "D")
    say("", "(……정신을 차려 보니 주막 방이다. 누군가 여기까지 데려다준 모양이다.)")
  end
end

-- Trigger in front of the gate; not once-only, so a lost fight can be retried.
function gate_jangseung()
  if flag("quest_gate") ~= 0 then return end
  face("U")
  say("", "어귀의 장승 둘이 붉은 눈으로 이쪽을 내려다본다.")
  fight_jangseung()
end

function shop_talisman()
  say("부적방", "어서 오시오. 부적방이오. 무얼 드릴까?")
  shop({"cheongsimhwan", "contract_talisman"})
  say("부적방", "또 들르시오. 길 조심하시고.")
end

-- A pressed facing tile (x, y).
function search(x, y)
  local k = tile(x, y)
  if k == "R" then
    if searched(x, y) then say("", "쌀이 반쯤 차 있는 뒤주다.") return end
    mark_searched(x, y)
    give("hanji", 2)
    say("", "뒤주 안 쌀 사이에서 한지 2장을 찾았다!")
  elseif k == "J" then
    if searched(x, y) then say("", "이미 들여다본 항아리다.") return end
    mark_searched(x, y)
    local r = daily_roll(3, x, y)
    if r == 0 then
      money(10)
      say("", "항아리 바닥에서 엽전 10냥을 찾았다!")
    elseif r == 1 then
      give("cheongsimhwan", 1)
      say("", "항아리 속에 청심환 1개가 숨겨져 있었다!")
    else
      say("", "장 냄새만 가득하다. 아무것도 없다.")
    end
  elseif k == "W" then
    if phase() == "night" then
      say("", "우물 속 깊은 곳에서 누군가 이름을 부르는 것 같다……", "(밤에는 물가를 조심해야겠다.)")
    else
      say("", "맑은 우물이다. 두레박이 걸려 있다.")
    end
  elseif k == "B" then
    if ask("", "이부자리가 깔려 있다. 쉬어 갈까?", {"쉰다", "그만둔다"}) == 1 then
      rest()
      toast(clock() .. " — 몸이 가뿐하다")
    end
  elseif k == "e" then
    shop_talisman()
  elseif k == "G" then
    if flag("quest_gate") == 0 then fight_jangseung()
    else say("", "요기가 걷힌 장승이 묵묵히 마을 어귀를 지키고 있다.") end
  elseif k == "S" then
    say("", "이끼 낀 돌담이다.")
  elseif k == "H" or k == "h" then
    say("", "초가집이다.")
  end
end

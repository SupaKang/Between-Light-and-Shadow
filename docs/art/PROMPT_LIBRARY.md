# 아트 제작 프롬프트 라이브러리

> `tools/make_art_prompts.py`로 생성한 파일. 직접 고치지 말 것. 소재는 `data/art/*.json`에서 고치고 스크립트를 다시 실행한다.

총 466개 프롬프트. 생성: `python3 tools/gen_image.py batch assets/prompts/manifest.json <id 접두어>`

## 공통 스타일

```
Pixel art for a dark Joseon-dynasty Korean folk-horror (gwaedam) RPG. Crisp hard-edged square pixels on a strict low-resolution grid, no anti-aliasing, no gradients, no painterly blur, 1px dark outlines, limited palette dominated by black and deep greens (near-black, moss, sickly phosphor green, pale bone white) with only rare accents of dried-blood red and sallow skin. Eerie, quiet, moonless-night mood, faint green ghost-fire (dokkaebibul) light.
```

## 에셋 규격

| 종류 | 크기 | 구성 |
|---|---|---|
| `portrait` | 96x96 | single bust portrait, head and shoulders, three-quarter view facing left, clear readable facial features and strong face outline |
| `battle_front` | 128x128 | single monster battle sprite, full body, facing the viewer, menacing pose, Pokemon-style battler framing |
| `battle_back` | 128x128 | single back view battle sprite seen from behind and slightly to the side, upper body filling the frame, cropped at the bottom edge |
| `tileset` | 32x32 per tile | top-down 3/4 RPG tileset sheet on an exact 32px grid, 8 columns x 8 rows of tiles, seamless repeating ground tiles, no characters, no magenta needed (fill unused cells with black) |
| `item_icon` | 32x32 | single inventory item icon, centered, slightly angled, readable silhouette |
| `ui_sheet` | native 640x360 UI pixels | UI parts sheet on a black background, parts separated by empty space |
| `concept` | wide 16:9 illustration | concept art screenshot mockup of the game at 640x360, top-down 3/4 view, no UI |
| `field_down` | about 512x768, one character | single full-body standing character sprite for a top-down RPG, 2-head-tall chibi proportions with a slightly small head, clearly separated arms and legs, visible feet, FRONT VIEW facing the viewer, symmetrical pose |
| `field_side` | about 512x768, one character | single full-body standing character sprite for a top-down RPG, 2-head-tall chibi proportions with a slightly small head, clearly separated arms and legs, visible feet, SIDE PROFILE VIEW facing left, walking pose |
| `field_up` | about 512x768, one character | single full-body standing character sprite for a top-down RPG, 2-head-tall chibi proportions with a slightly small head, clearly separated arms and legs, visible feet, BACK VIEW seen from behind, face not visible |
| `mini_icon` | 32x32 | tiny party/bestiary icon of the creature's head and upper body, centered, bold silhouette |

## NPC·인물 (95)

### 주인공 (벽사청 음양사) (`hero`)
- **field_down** → `assets/gen/npc/hero_field_down.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff with a brass ring finial firmly in the right hand, the staff touching the ground (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/hero_field_side.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff with a brass ring finial firmly in the right hand, the staff touching the ground (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/hero_field_up.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff with a brass ring finial firmly in the right hand, the staff touching the ground (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/hero_portrait.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff with a brass ring finial firmly in the right hand, the staff touching the ground (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/npc/hero_battle_back.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff with a brass ring finial firmly in the right hand, the staff touching the ground (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 주인공 (밤: 등불) (`hero_night`)
- **field_down** → `assets/gen/npc/hero_night_field_down.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff in the right hand and a small paper lantern glowing green hanging from the left hand, both held close to the body (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/hero_night_field_side.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff in the right hand and a small paper lantern glowing green hanging from the left hand, both held close to the body (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/hero_night_field_up.png`  
  Subject: young Korean yin-yang master (eumyangsa) of the royal Byeoksacheong exorcist bureau: black conical satgat straw hat, short black hair, pale face, dark green dopo robe tied at the waist with a blood-red sash, white collar, white socks, black shoes, holding a long wooden staff in the right hand and a small paper lantern glowing green hanging from the left hand, both held close to the body (approved portrait design). Age about 22: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 정체불명의 노승 (프롤로그 화자) (`old_monk`)
- **portrait** → `assets/gen/npc/old_monk_portrait.png`  
  Subject: mysterious elderly Buddhist monk, wide conical straw satgat hat shading his eyes, long white beard, gray jangsam robe with a faded red kasaya, wooden prayer beads, a knowing smile (TODO: identity, relation to Myogak open). Age about 80: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_down** → `assets/gen/npc/old_monk_field_down.png`  
  Subject: mysterious elderly Buddhist monk, wide conical straw satgat hat shading his eyes, long white beard, gray jangsam robe with a faded red kasaya, wooden prayer beads, a knowing smile (TODO: identity, relation to Myogak open). Age about 80: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/old_monk_field_side.png`  
  Subject: mysterious elderly Buddhist monk, wide conical straw satgat hat shading his eyes, long white beard, gray jangsam robe with a faded red kasaya, wooden prayer beads, a knowing smile (TODO: identity, relation to Myogak open). Age about 80: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/old_monk_field_up.png`  
  Subject: mysterious elderly Buddhist monk, wide conical straw satgat hat shading his eyes, long white beard, gray jangsam robe with a faded red kasaya, wooden prayer beads, a knowing smile (TODO: identity, relation to Myogak open). Age about 80: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 주막 주모 (`jumo`)
- **field_down** → `assets/gen/npc/jumo_field_down.png`  
  Subject: tavern keeper woman in her mid-fifties: plump sturdy build, round face with wrinkles and laugh lines, graying black hair in a low chignon with a wooden binyeo pin, faded white jeogori with a dark red ribbon, long dark chima skirt with an apron, sleeves rolled up, tired kind eyes. Age about 55: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/jumo_field_side.png`  
  Subject: tavern keeper woman in her mid-fifties: plump sturdy build, round face with wrinkles and laugh lines, graying black hair in a low chignon with a wooden binyeo pin, faded white jeogori with a dark red ribbon, long dark chima skirt with an apron, sleeves rolled up, tired kind eyes. Age about 55: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/jumo_field_up.png`  
  Subject: tavern keeper woman in her mid-fifties: plump sturdy build, round face with wrinkles and laugh lines, graying black hair in a low chignon with a wooden binyeo pin, faded white jeogori with a dark red ribbon, long dark chima skirt with an apron, sleeves rolled up, tired kind eyes. Age about 55: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/jumo_portrait.png`  
  Subject: tavern keeper woman in her mid-fifties: plump sturdy build, round face with wrinkles and laugh lines, graying black hair in a low chignon with a wooden binyeo pin, faded white jeogori with a dark red ribbon, long dark chima skirt with an apron, sleeves rolled up, tired kind eyes. Age about 55: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 마을 어귀 노인 (`village_elder`)
- **field_down** → `assets/gen/npc/village_elder_field_down.png`  
  Subject: super-deformed chibi old village man, head as large as the body: black horsehair gat hat with a wide flat brim and tall crown, white hanbok coat, long white goatee, furrowed brows, gnarled wooden cane. Age about 72: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/village_elder_field_side.png`  
  Subject: super-deformed chibi old village man, head as large as the body: black horsehair gat hat with a wide flat brim and tall crown, white hanbok coat, long white goatee, furrowed brows, gnarled wooden cane. Age about 72: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/village_elder_field_up.png`  
  Subject: super-deformed chibi old village man, head as large as the body: black horsehair gat hat with a wide flat brim and tall crown, white hanbok coat, long white goatee, furrowed brows, gnarled wooden cane. Age about 72: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/village_elder_portrait.png`  
  Subject: super-deformed chibi old village man, head as large as the body: black horsehair gat hat with a wide flat brim and tall crown, white hanbok coat, long white goatee, furrowed brows, gnarled wooden cane. Age about 72: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 부적방 주인 (`talisman_seller`)
- **field_down** → `assets/gen/npc/talisman_seller_field_down.png`  
  Subject: thin talisman shop owner with ink-stained fingers, round spectacles, headband, holding a stack of yellow paper talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/talisman_seller_field_side.png`  
  Subject: thin talisman shop owner with ink-stained fingers, round spectacles, headband, holding a stack of yellow paper talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/talisman_seller_field_up.png`  
  Subject: thin talisman shop owner with ink-stained fingers, round spectacles, headband, holding a stack of yellow paper talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/talisman_seller_portrait.png`  
  Subject: thin talisman shop owner with ink-stained fingers, round spectacles, headband, holding a stack of yellow paper talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 벽사청 상관 (TODO: 이름) (`bureau_chief`)
- **portrait** → `assets/gen/npc/bureau_chief_portrait.png`  
  Subject: stern senior official of the exorcist bureau, black official hat (samo), dark crimson official robe with a crane rank badge, scar across one eyebrow. Age about 52: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 괴승 묘각 (1장 보스) (`myogak`)
- **field_down** → `assets/gen/npc/myogak_field_down.png`  
  Subject: sinister renegade monk of the Eumyangdang, shaved head tattooed with black sutra script, tattered black robe, rosary of small skulls, red-glowing eyes, holding a staff wrapped in forbidden talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/myogak_field_side.png`  
  Subject: sinister renegade monk of the Eumyangdang, shaved head tattooed with black sutra script, tattered black robe, rosary of small skulls, red-glowing eyes, holding a staff wrapped in forbidden talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/myogak_field_up.png`  
  Subject: sinister renegade monk of the Eumyangdang, shaved head tattooed with black sutra script, tattered black robe, rosary of small skulls, red-glowing eyes, holding a staff wrapped in forbidden talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/myogak_portrait.png`  
  Subject: sinister renegade monk of the Eumyangdang, shaved head tattooed with black sutra script, tattered black robe, rosary of small skulls, red-glowing eyes, holding a staff wrapped in forbidden talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/myogak_battle_front.png`  
  Subject: sinister renegade monk of the Eumyangdang, shaved head tattooed with black sutra script, tattered black robe, rosary of small skulls, red-glowing eyes, holding a staff wrapped in forbidden talismans. Age about 45: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음양당 하수인 술사 (`eumyangdang_acolyte`)
- **field_down** → `assets/gen/npc/eumyangdang_acolyte_field_down.png`  
  Subject: masked cult sorcerer of the Eumyangdang, black hooded robe with a white yin-yang sigil broken in half, paper mask with painted eyes, bundle of cursed talismans. Age about 30: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/eumyangdang_acolyte_field_side.png`  
  Subject: masked cult sorcerer of the Eumyangdang, black hooded robe with a white yin-yang sigil broken in half, paper mask with painted eyes, bundle of cursed talismans. Age about 30: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/eumyangdang_acolyte_field_up.png`  
  Subject: masked cult sorcerer of the Eumyangdang, black hooded robe with a white yin-yang sigil broken in half, paper mask with painted eyes, bundle of cursed talismans. Age about 30: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/eumyangdang_acolyte_portrait.png`  
  Subject: masked cult sorcerer of the Eumyangdang, black hooded robe with a white yin-yang sigil broken in half, paper mask with painted eyes, bundle of cursed talismans. Age about 30: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/eumyangdang_acolyte_battle_front.png`  
  Subject: masked cult sorcerer of the Eumyangdang, black hooded robe with a white yin-yang sigil broken in half, paper mask with painted eyes, bundle of cursed talismans. Age about 30: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음양당 술사 (2장) (`sobaek_sorcerer`)
- **portrait** → `assets/gen/npc/sobaek_sorcerer_portrait.png`  
  Subject: gaunt mountain sorcerer with iron chains and an iron mask, commands the iron-eating Bulgasari. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/sobaek_sorcerer_battle_front.png`  
  Subject: gaunt mountain sorcerer with iron chains and an iron mask, commands the iron-eating Bulgasari. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음양당 수로방주 (3장 보스) (`waterway_master`)
- **portrait** → `assets/gen/npc/waterway_master_portrait.png`  
  Subject: drowned-looking cult master of the waterways, soaked black robes, seaweed hair, pale bloated hands holding a rope of drowned souls. Age about 48: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/waterway_master_battle_front.png`  
  Subject: drowned-looking cult master of the waterways, soaked black robes, seaweed hair, pale bloated hands holding a rope of drowned souls. Age about 48: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음양당 좌호법 (4장 보스) (`left_guardian`)
- **portrait** → `assets/gen/npc/left_guardian_portrait.png`  
  Subject: elegant cult lieutenant in white robes with fox-fur collar, fan in hand, cold beauty, bamboo forest behind. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/left_guardian_battle_front.png`  
  Subject: elegant cult lieutenant in white robes with fox-fur collar, fan in hand, cold beauty, bamboo forest behind. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음양당 당주 묵영 (최종 보스) (`mukyeong`)
- **portrait** → `assets/gen/npc/mukyeong_portrait.png`  
  Subject: leader of the Eumyangdang, figure in ink-black robes that bleed into shadow, half of the face covered by a cracked yin-yang mask, eyes like an eclipse. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_front** → `assets/gen/npc/mukyeong_battle_front.png`  
  Subject: leader of the Eumyangdang, figure in ink-black robes that bleed into shadow, half of the face covered by a cracked yin-yang mask, eyes like an eclipse. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 나루터 뱃사공 (`ferryman`)
- **field_down** → `assets/gen/npc/ferryman_field_down.png`  
  Subject: weathered ferryman with a straw raincoat (dorongi) and straw hat, long oar, lantern on the boat bow. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/ferryman_field_side.png`  
  Subject: weathered ferryman with a straw raincoat (dorongi) and straw hat, long oar, lantern on the boat bow. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/ferryman_field_up.png`  
  Subject: weathered ferryman with a straw raincoat (dorongi) and straw hat, long oar, lantern on the boat bow. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/ferryman_portrait.png`  
  Subject: weathered ferryman with a straw raincoat (dorongi) and straw hat, long oar, lantern on the boat bow. Age about 60: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 방귀쟁이 며느리 (SQ-01) (`daughter_in_law`)
- **field_down** → `assets/gen/npc/daughter_in_law_field_down.png`  
  Subject: embarrassed young daughter-in-law in a plain hanbok covering her mouth, wind swirling around her skirt. Age about 20: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/daughter_in_law_field_side.png`  
  Subject: embarrassed young daughter-in-law in a plain hanbok covering her mouth, wind swirling around her skirt. Age about 20: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/daughter_in_law_field_up.png`  
  Subject: embarrassed young daughter-in-law in a plain hanbok covering her mouth, wind swirling around her skirt. Age about 20: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/daughter_in_law_portrait.png`  
  Subject: embarrassed young daughter-in-law in a plain hanbok covering her mouth, wind swirling around her skirt. Age about 20: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 은혜 갚은 소녀 (SQ-02) (`toad_girl`)
- **field_down** → `assets/gen/npc/toad_girl_field_down.png`  
  Subject: brave village girl in a worn hanbok holding a bowl of rice for a toad, braided hair. Age about 14: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/toad_girl_field_side.png`  
  Subject: brave village girl in a worn hanbok holding a bowl of rice for a toad, braided hair. Age about 14: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/toad_girl_field_up.png`  
  Subject: brave village girl in a worn hanbok holding a bowl of rice for a toad, braided hair. Age about 14: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/toad_girl_portrait.png`  
  Subject: brave village girl in a worn hanbok holding a bowl of rice for a toad, braided hair. Age about 14: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 다자구 할머니 (SQ-03) (`dajagu_grandma`)
- **field_down** → `assets/gen/npc/dajagu_grandma_field_down.png`  
  Subject: sly old grandmother on a mountain pass, hunched, with a walking stick and a secret grin, calling out into the night. Age about 78: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/dajagu_grandma_field_side.png`  
  Subject: sly old grandmother on a mountain pass, hunched, with a walking stick and a secret grin, calling out into the night. Age about 78: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/dajagu_grandma_field_up.png`  
  Subject: sly old grandmother on a mountain pass, hunched, with a walking stick and a secret grin, calling out into the night. Age about 78: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/dajagu_grandma_portrait.png`  
  Subject: sly old grandmother on a mountain pass, hunched, with a walking stick and a secret grin, calling out into the night. Age about 78: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 서낭당 무녀 (`shrine_shaman`)
- **field_down** → `assets/gen/npc/shrine_shaman_field_down.png`  
  Subject: shaman woman (mudang) with a red and white ritual robe, brass bells and a fan, face calm and unreadable. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/shrine_shaman_field_side.png`  
  Subject: shaman woman (mudang) with a red and white ritual robe, brass bells and a fan, face calm and unreadable. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/shrine_shaman_field_up.png`  
  Subject: shaman woman (mudang) with a red and white ritual robe, brass bells and a fan, face calm and unreadable. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/shrine_shaman_portrait.png`  
  Subject: shaman woman (mudang) with a red and white ritual robe, brass bells and a fan, face calm and unreadable. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 산속 민가 노파 (휴식 거점) (`mountain_hut_widow`)
- **field_down** → `assets/gen/npc/mountain_hut_widow_field_down.png`  
  Subject: old widow living alone in a mountain hut, white headscarf, weaving straw sandals by a small fire. Age about 75: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/mountain_hut_widow_field_side.png`  
  Subject: old widow living alone in a mountain hut, white headscarf, weaving straw sandals by a small fire. Age about 75: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/mountain_hut_widow_field_up.png`  
  Subject: old widow living alone in a mountain hut, white headscarf, weaving straw sandals by a small fire. Age about 75: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/mountain_hut_widow_portrait.png`  
  Subject: old widow living alone in a mountain hut, white headscarf, weaving straw sandals by a small fire. Age about 75: elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; clearly old even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 마을 아이 (`villager_child`)
- **field_down** → `assets/gen/npc/villager_child_field_down.png`  
  Subject: small village child with a pigtail and a pinwheel, curious but frightened. Age about 8: a small child, round face, short limbs. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/villager_child_field_side.png`  
  Subject: small village child with a pigtail and a pinwheel, curious but frightened. Age about 8: a small child, round face, short limbs. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/villager_child_field_up.png`  
  Subject: small village child with a pigtail and a pinwheel, curious but frightened. Age about 8: a small child, round face, short limbs. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 포졸 (`constable`)
- **field_down** → `assets/gen/npc/constable_field_down.png`  
  Subject: Joseon constable with a black felt hat marked with red, blue uniform, wooden club. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/constable_field_side.png`  
  Subject: Joseon constable with a black felt hat marked with red, blue uniform, wooden club. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/constable_field_up.png`  
  Subject: Joseon constable with a black felt hat marked with red, blue uniform, wooden club. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/constable_portrait.png`  
  Subject: Joseon constable with a black felt hat marked with red, blue uniform, wooden club. Age about 35: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 보부상 (`peddler`)
- **field_down** → `assets/gen/npc/peddler_field_down.png`  
  Subject: traveling peddler with a big A-frame pack (jige) and a bamboo hat with cotton tufts. Age about 40: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/peddler_field_side.png`  
  Subject: traveling peddler with a big A-frame pack (jige) and a bamboo hat with cotton tufts. Age about 40: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/peddler_field_up.png`  
  Subject: traveling peddler with a big A-frame pack (jige) and a bamboo hat with cotton tufts. Age about 40: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/peddler_portrait.png`  
  Subject: traveling peddler with a big A-frame pack (jige) and a bamboo hat with cotton tufts. Age about 40: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 지리산 포수 (`hunter`)
- **field_down** → `assets/gen/npc/hunter_field_down.png`  
  Subject: tiger hunter with a matchlock rifle, fur vest, scar across the cheek. Age about 42: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/hunter_field_side.png`  
  Subject: tiger hunter with a matchlock rifle, fur vest, scar across the cheek. Age about 42: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/hunter_field_up.png`  
  Subject: tiger hunter with a matchlock rifle, fur vest, scar across the cheek. Age about 42: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/hunter_portrait.png`  
  Subject: tiger hunter with a matchlock rifle, fur vest, scar across the cheek. Age about 42: adult face, faint lines around the eyes. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 남해 해녀 (`haenyeo`)
- **field_down** → `assets/gen/npc/haenyeo_field_down.png`  
  Subject: sea diver woman in dark diving clothes with a gourd float (taewak) and net. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/haenyeo_field_side.png`  
  Subject: sea diver woman in dark diving clothes with a gourd float (taewak) and net. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/haenyeo_field_up.png`  
  Subject: sea diver woman in dark diving clothes with a gourd float (taewak) and net. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/haenyeo_portrait.png`  
  Subject: sea diver woman in dark diving clothes with a gourd float (taewak) and net. Age about 50: middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; mature adult face even in chibi style, never childlike. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 천년 사찰 승려 (`temple_monk`)
- **field_down** → `assets/gen/npc/temple_monk_field_down.png`  
  Subject: calm young temple monk sweeping leaves, gray robe, shaved head. Age about 25: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/temple_monk_field_side.png`  
  Subject: calm young temple monk sweeping leaves, gray robe, shaved head. Age about 25: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/temple_monk_field_up.png`  
  Subject: calm young temple monk sweeping leaves, gray robe, shaved head. Age about 25: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/temple_monk_portrait.png`  
  Subject: calm young temple monk sweeping leaves, gray robe, shaved head. Age about 25: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 떠돌이 퇴마사 (트레이너형 NPC) (`exorcist_rival`)
- **field_down** → `assets/gen/npc/exorcist_rival_field_down.png`  
  Subject: arrogant wandering exorcist with a sword and paper charms tucked in the belt, challenges travelers. Age about 24: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_side** → `assets/gen/npc/exorcist_rival_field_side.png`  
  Subject: arrogant wandering exorcist with a sword and paper charms tucked in the belt, challenges travelers. Age about 24: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **field_up** → `assets/gen/npc/exorcist_rival_field_up.png`  
  Subject: arrogant wandering exorcist with a sword and paper charms tucked in the belt, challenges travelers. Age about 24: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **portrait** → `assets/gen/npc/exorcist_rival_portrait.png`  
  Subject: arrogant wandering exorcist with a sword and paper charms tucked in the belt, challenges travelers. Age about 24: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/npc/exorcist_rival_battle_back.png`  
  Subject: arrogant wandering exorcist with a sword and paper charms tucked in the belt, challenges travelers. Age about 24: youthful face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.

## 요괴 (108종) (324)

### 도깨비 (`yokai_001`)
- **battle_front** → `assets/gen/yokai/yokai_001_battle_front.png`  
  Subject: stocky one-horned goblin with shaggy hair and a wide toothy grin, spiked wooden club (bangmangi), straw sandals, tiger-pelt loincloth, faint blue goblin-fire (dokkaebibul) floating around it. Creature from Korean/East Asian folklore (한국 삼국유사): Dokkaebi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_001_battle_back.png`  
  Subject: stocky one-horned goblin with shaggy hair and a wide toothy grin, spiked wooden club (bangmangi), straw sandals, tiger-pelt loincloth, faint blue goblin-fire (dokkaebibul) floating around it. Creature from Korean/East Asian folklore (한국 삼국유사): Dokkaebi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_001_mini_icon.png`  
  Subject: stocky one-horned goblin with shaggy hair and a wide toothy grin, spiked wooden club (bangmangi), straw sandals, tiger-pelt loincloth, faint blue goblin-fire (dokkaebibul) floating around it. Creature from Korean/East Asian folklore (한국 삼국유사): Dokkaebi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 구미호 (`yokai_002`)
- **battle_front** → `assets/gen/yokai/yokai_002_battle_front.png`  
  Subject: slender white fox with nine flowing tails, eyes glowing, a luminous fox bead held in its mouth, half-visible ghostly woman's face in the mist behind it. Creature from Korean/East Asian folklore (한국 삼국유사): Nine-Tailed Fox. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_002_battle_back.png`  
  Subject: slender white fox with nine flowing tails, eyes glowing, a luminous fox bead held in its mouth, half-visible ghostly woman's face in the mist behind it. Creature from Korean/East Asian folklore (한국 삼국유사): Nine-Tailed Fox. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_002_mini_icon.png`  
  Subject: slender white fox with nine flowing tails, eyes glowing, a luminous fox bead held in its mouth, half-visible ghostly woman's face in the mist behind it. Creature from Korean/East Asian folklore (한국 삼국유사): Nine-Tailed Fox. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 불가사리 (`yokai_003`)
- **battle_front** → `assets/gen/yokai/yokai_003_battle_front.png`  
  Subject: iron-eating beast with a bear body, elephant trunk, rhinoceros eyes, ox tail and tiger legs, hide covered in rusted metal scales, gnawing a broken sword. Creature from Korean/East Asian folklore (한국 고려야사): Bulgasari. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_003_battle_back.png`  
  Subject: iron-eating beast with a bear body, elephant trunk, rhinoceros eyes, ox tail and tiger legs, hide covered in rusted metal scales, gnawing a broken sword. Creature from Korean/East Asian folklore (한국 고려야사): Bulgasari. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_003_mini_icon.png`  
  Subject: iron-eating beast with a bear body, elephant trunk, rhinoceros eyes, ox tail and tiger legs, hide covered in rusted metal scales, gnawing a broken sword. Creature from Korean/East Asian folklore (한국 고려야사): Bulgasari. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 삼족오 (`yokai_004`)
- **battle_front** → `assets/gen/yokai/yokai_004_battle_front.png`  
  Subject: large black crow with three legs perched inside a burning sun disc, feathers edged with ember light. Creature from Korean/East Asian folklore (한국 고구려 고분벽화): Three-Legged Crow. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_004_battle_back.png`  
  Subject: large black crow with three legs perched inside a burning sun disc, feathers edged with ember light. Creature from Korean/East Asian folklore (한국 고구려 고분벽화): Three-Legged Crow. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_004_mini_icon.png`  
  Subject: large black crow with three legs perched inside a burning sun disc, feathers edged with ember light. Creature from Korean/East Asian folklore (한국 고구려 고분벽화): Three-Legged Crow. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 이무기 (`yokai_005`)
- **battle_front** → `assets/gen/yokai/yokai_005_battle_front.png`  
  Subject: huge hornless serpent-dragon not yet ascended, coiled in murky water, scales dull and mossy, a pale pearl it cannot reach floating above. Creature from Korean/East Asian folklore (한국 용설화): Imugi. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_005_battle_back.png`  
  Subject: huge hornless serpent-dragon not yet ascended, coiled in murky water, scales dull and mossy, a pale pearl it cannot reach floating above. Creature from Korean/East Asian folklore (한국 용설화): Imugi. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_005_mini_icon.png`  
  Subject: huge hornless serpent-dragon not yet ascended, coiled in murky water, scales dull and mossy, a pale pearl it cannot reach floating above. Creature from Korean/East Asian folklore (한국 용설화): Imugi. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 해태 (`yokai_006`)
- **battle_front** → `assets/gen/yokai/yokai_006_battle_front.png`  
  Subject: lion-like guardian beast with a single horn, curly mane, bell collar, stern stone-statue texture cracking to reveal living flesh. Creature from Korean/East Asian folklore (한국 조선왕조실록): Haetae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_006_battle_back.png`  
  Subject: lion-like guardian beast with a single horn, curly mane, bell collar, stern stone-statue texture cracking to reveal living flesh. Creature from Korean/East Asian folklore (한국 조선왕조실록): Haetae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_006_mini_icon.png`  
  Subject: lion-like guardian beast with a single horn, curly mane, bell collar, stern stone-statue texture cracking to reveal living flesh. Creature from Korean/East Asian folklore (한국 조선왕조실록): Haetae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 영감 (`yokai_007`)
- **battle_front** → `assets/gen/yokai/yokai_007_battle_front.png`  
  Subject: Jeju sea goblin god (yeonggam) in a torn fisherman's coat and horsehair hat, one eye glowing, carrying a torch over dark waves. Creature from Korean/East Asian folklore (제주 도깨비 신앙 (영감본풀이)): Yeonggam. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_007_battle_back.png`  
  Subject: Jeju sea goblin god (yeonggam) in a torn fisherman's coat and horsehair hat, one eye glowing, carrying a torch over dark waves. Creature from Korean/East Asian folklore (제주 도깨비 신앙 (영감본풀이)): Yeonggam. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_007_mini_icon.png`  
  Subject: Jeju sea goblin god (yeonggam) in a torn fisherman's coat and horsehair hat, one eye glowing, carrying a torch over dark waves. Creature from Korean/East Asian folklore (제주 도깨비 신앙 (영감본풀이)): Yeonggam. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 수살귀 (`yokai_008`)
- **battle_front** → `assets/gen/yokai/yokai_008_battle_front.png`  
  Subject: drowned vengeful spirit with bloated gray skin and weed-tangled hair, rising from black water, clutching at ankles. Creature from Korean/East Asian folklore (한국 무속 (물에 빠져 죽은 원귀)): Susalgwi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_008_battle_back.png`  
  Subject: drowned vengeful spirit with bloated gray skin and weed-tangled hair, rising from black water, clutching at ankles. Creature from Korean/East Asian folklore (한국 무속 (물에 빠져 죽은 원귀)): Susalgwi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_008_mini_icon.png`  
  Subject: drowned vengeful spirit with bloated gray skin and weed-tangled hair, rising from black water, clutching at ankles. Creature from Korean/East Asian folklore (한국 무속 (물에 빠져 죽은 원귀)): Susalgwi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 두두리 (`yokai_009`)
- **battle_front** → `assets/gen/yokai/yokai_009_battle_front.png`  
  Subject: ancient wooden goblin god of Gyeongju (mokrang), body of weathered carved wood, holding a mallet, eyes like knots in timber. Creature from Korean/East Asian folklore (삼국유사·동경잡기 (경주 목랑 두두리)): Duduri. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_009_battle_back.png`  
  Subject: ancient wooden goblin god of Gyeongju (mokrang), body of weathered carved wood, holding a mallet, eyes like knots in timber. Creature from Korean/East Asian folklore (삼국유사·동경잡기 (경주 목랑 두두리)): Duduri. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_009_mini_icon.png`  
  Subject: ancient wooden goblin god of Gyeongju (mokrang), body of weathered carved wood, holding a mallet, eyes like knots in timber. Creature from Korean/East Asian folklore (삼국유사·동경잡기 (경주 목랑 두두리)): Duduri. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 객귀 (`yokai_010`)
- **battle_front** → `assets/gen/yokai/yokai_010_battle_front.png`  
  Subject: wandering spirit of one who died far from home, traveler's straw hat and bundle, gaunt face, road dust trailing. Creature from Korean/East Asian folklore (한국 무속 (객사한 원혼)): Gaekgwi. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_010_battle_back.png`  
  Subject: wandering spirit of one who died far from home, traveler's straw hat and bundle, gaunt face, road dust trailing. Creature from Korean/East Asian folklore (한국 무속 (객사한 원혼)): Gaekgwi. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_010_mini_icon.png`  
  Subject: wandering spirit of one who died far from home, traveler's straw hat and bundle, gaunt face, road dust trailing. Creature from Korean/East Asian folklore (한국 무속 (객사한 원혼)): Gaekgwi. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 천지왕 (`yokai_011`)
- **battle_front** → `assets/gen/yokai/yokai_011_battle_front.png`  
  Subject: celestial king of heaven and earth in shining ceremonial robes and crown, holding sun and moon discs, radiating light. Creature from Korean/East Asian folklore (한국 제주신화): Cheonjiwang. Presence: godlike divine being, overwhelming aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_011_battle_back.png`  
  Subject: celestial king of heaven and earth in shining ceremonial robes and crown, holding sun and moon discs, radiating light. Creature from Korean/East Asian folklore (한국 제주신화): Cheonjiwang. Presence: godlike divine being, overwhelming aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_011_mini_icon.png`  
  Subject: celestial king of heaven and earth in shining ceremonial robes and crown, holding sun and moon discs, radiating light. Creature from Korean/East Asian folklore (한국 제주신화): Cheonjiwang. Presence: godlike divine being, overwhelming aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 마고할미 (`yokai_012`)
- **battle_front** → `assets/gen/yokai/yokai_012_battle_front.png`  
  Subject: colossal old giantess with wild gray hair, carrying rocks in her skirt to build mountains, kind but terrifying scale. Creature from Korean/East Asian folklore (한국 창세설화): Mago Giantess. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_012_battle_back.png`  
  Subject: colossal old giantess with wild gray hair, carrying rocks in her skirt to build mountains, kind but terrifying scale. Creature from Korean/East Asian folklore (한국 창세설화): Mago Giantess. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_012_mini_icon.png`  
  Subject: colossal old giantess with wild gray hair, carrying rocks in her skirt to build mountains, kind but terrifying scale. Creature from Korean/East Asian folklore (한국 창세설화): Mago Giantess. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 강철이 (`yokai_013`)
- **battle_front** → `assets/gen/yokai/yokai_013_battle_front.png`  
  Subject: drought dragon of fire, cracked lava-red scales, scorched wings, dried fields burning where it passes. Creature from Korean/East Asian folklore (한국 용재총화): Gangcheol Dragon. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_013_battle_back.png`  
  Subject: drought dragon of fire, cracked lava-red scales, scorched wings, dried fields burning where it passes. Creature from Korean/East Asian folklore (한국 용재총화): Gangcheol Dragon. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_013_mini_icon.png`  
  Subject: drought dragon of fire, cracked lava-red scales, scorched wings, dried fields burning where it passes. Creature from Korean/East Asian folklore (한국 용재총화): Gangcheol Dragon. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 금돼지 (`yokai_014`)
- **battle_front** → `assets/gen/yokai/yokai_014_battle_front.png`  
  Subject: monstrous golden boar that abducts women into its cave, bristling metallic hide, tusks, deer-skin weakness. Creature from Korean/East Asian folklore (최치원 금돼지 설화): Golden Boar. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_014_battle_back.png`  
  Subject: monstrous golden boar that abducts women into its cave, bristling metallic hide, tusks, deer-skin weakness. Creature from Korean/East Asian folklore (최치원 금돼지 설화): Golden Boar. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_014_mini_icon.png`  
  Subject: monstrous golden boar that abducts women into its cave, bristling metallic hide, tusks, deer-skin weakness. Creature from Korean/East Asian folklore (최치원 금돼지 설화): Golden Boar. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 허주 (`yokai_015`)
- **battle_front** → `assets/gen/yokai/yokai_015_battle_front.png`  
  Subject: hungry wandering ghost with an empty rice bowl, ragged white clothes, hollow cheeks, drifting between houses. Creature from Korean/East Asian folklore (한국 무속 (떠도는 잡귀 허주)): Heoju. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_015_battle_back.png`  
  Subject: hungry wandering ghost with an empty rice bowl, ragged white clothes, hollow cheeks, drifting between houses. Creature from Korean/East Asian folklore (한국 무속 (떠도는 잡귀 허주)): Heoju. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_015_mini_icon.png`  
  Subject: hungry wandering ghost with an empty rice bowl, ragged white clothes, hollow cheeks, drifting between houses. Creature from Korean/East Asian folklore (한국 무속 (떠도는 잡귀 허주)): Heoju. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 물도깨비 (`yokai_016`)
- **battle_front** → `assets/gen/yokai/yokai_016_battle_front.png`  
  Subject: slimy water goblin with webbed hands, lotus-leaf hat, one horn, dragging fishing nets. Creature from Korean/East Asian folklore (한국 민담): Water Goblin. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_016_battle_back.png`  
  Subject: slimy water goblin with webbed hands, lotus-leaf hat, one horn, dragging fishing nets. Creature from Korean/East Asian folklore (한국 민담): Water Goblin. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_016_mini_icon.png`  
  Subject: slimy water goblin with webbed hands, lotus-leaf hat, one horn, dragging fishing nets. Creature from Korean/East Asian folklore (한국 민담): Water Goblin. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 술도깨비 (`yokai_017`)
- **battle_front** → `assets/gen/yokai/yokai_017_battle_front.png`  
  Subject: red-faced drunken goblin hugging a makgeolli jar, staggering, club slung over the shoulder. Creature from Korean/East Asian folklore (한국 민담): Wine Goblin. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_017_battle_back.png`  
  Subject: red-faced drunken goblin hugging a makgeolli jar, staggering, club slung over the shoulder. Creature from Korean/East Asian folklore (한국 민담): Wine Goblin. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_017_mini_icon.png`  
  Subject: red-faced drunken goblin hugging a makgeolli jar, staggering, club slung over the shoulder. Creature from Korean/East Asian folklore (한국 민담): Wine Goblin. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 강시귀 (`yokai_018`)
- **battle_front** → `assets/gen/yokai/yokai_018_battle_front.png`  
  Subject: hopping corpse in a Qing official's robe, arms stretched forward, a yellow paper talisman stuck on its forehead. Creature from Korean/East Asian folklore (동양 전승): Jiangshi. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_018_battle_back.png`  
  Subject: hopping corpse in a Qing official's robe, arms stretched forward, a yellow paper talisman stuck on its forehead. Creature from Korean/East Asian folklore (동양 전승): Jiangshi. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_018_mini_icon.png`  
  Subject: hopping corpse in a Qing official's robe, arms stretched forward, a yellow paper talisman stuck on its forehead. Creature from Korean/East Asian folklore (동양 전승): Jiangshi. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 오수 의견 (`yokai_019`)
- **battle_front** → `assets/gen/yokai/yokai_019_battle_front.png`  
  Subject: loyal dog spirit soaked in river water that saved its sleeping master from a grass fire, singed fur, translucent glow. Creature from Korean/East Asian folklore (오수 의견 설화 (보한집)): Loyal Dog of Osu. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_019_battle_back.png`  
  Subject: loyal dog spirit soaked in river water that saved its sleeping master from a grass fire, singed fur, translucent glow. Creature from Korean/East Asian folklore (오수 의견 설화 (보한집)): Loyal Dog of Osu. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_019_mini_icon.png`  
  Subject: loyal dog spirit soaked in river water that saved its sleeping master from a grass fire, singed fur, translucent glow. Creature from Korean/East Asian folklore (오수 의견 설화 (보한집)): Loyal Dog of Osu. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 우렁각시 (`yokai_020`)
- **battle_front** → `assets/gen/yokai/yokai_020_battle_front.png`  
  Subject: pale bride emerging from a giant river snail shell, wet hair, water dripping from her hanbok. Creature from Korean/East Asian folklore (우렁각시 설화): Snail Bride. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_020_battle_back.png`  
  Subject: pale bride emerging from a giant river snail shell, wet hair, water dripping from her hanbok. Creature from Korean/East Asian folklore (우렁각시 설화): Snail Bride. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_020_mini_icon.png`  
  Subject: pale bride emerging from a giant river snail shell, wet hair, water dripping from her hanbok. Creature from Korean/East Asian folklore (우렁각시 설화): Snail Bride. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 천록 (`yokai_021`)
- **battle_front** → `assets/gen/yokai/yokai_021_battle_front.png`  
  Subject: palace stone guardian beast (cheonrok) with scaled body and single horn, moss-covered granite, eyes that glow. Creature from Korean/East Asian folklore (한국 경복궁 석수): Cheonrok. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_021_battle_back.png`  
  Subject: palace stone guardian beast (cheonrok) with scaled body and single horn, moss-covered granite, eyes that glow. Creature from Korean/East Asian folklore (한국 경복궁 석수): Cheonrok. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_021_mini_icon.png`  
  Subject: palace stone guardian beast (cheonrok) with scaled body and single horn, moss-covered granite, eyes that glow. Creature from Korean/East Asian folklore (한국 경복궁 석수): Cheonrok. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 지네귀신 (`yokai_022`)
- **battle_front** → `assets/gen/yokai/yokai_022_battle_front.png`  
  Subject: enormous centipede demon with a woman's pale face, hundreds of legs, venomous mandibles. Creature from Korean/East Asian folklore (한국 전설의 고향): Centipede Demon. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_022_battle_back.png`  
  Subject: enormous centipede demon with a woman's pale face, hundreds of legs, venomous mandibles. Creature from Korean/East Asian folklore (한국 전설의 고향): Centipede Demon. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_022_mini_icon.png`  
  Subject: enormous centipede demon with a woman's pale face, hundreds of legs, venomous mandibles. Creature from Korean/East Asian folklore (한국 전설의 고향): Centipede Demon. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 네코마타 (`yokai_023`)
- **battle_front** → `assets/gen/yokai/yokai_023_battle_front.png`  
  Subject: two-tailed cat demon with ghost-fire flames on its tails, standing on hind legs, sly human-like grin. Creature from Korean/East Asian folklore (일본 전승 (고양이 요괴 네코마타)): Nekomata. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_023_battle_back.png`  
  Subject: two-tailed cat demon with ghost-fire flames on its tails, standing on hind legs, sly human-like grin. Creature from Korean/East Asian folklore (일본 전승 (고양이 요괴 네코마타)): Nekomata. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_023_mini_icon.png`  
  Subject: two-tailed cat demon with ghost-fire flames on its tails, standing on hind legs, sly human-like grin. Creature from Korean/East Asian folklore (일본 전승 (고양이 요괴 네코마타)): Nekomata. Presence: small and eerie. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 귀신새 (`yokai_024`)
- **battle_front** → `assets/gen/yokai/yokai_024_battle_front.png`  
  Subject: black bird with a human face that cries at night, ragged feathers, perched on a dead branch. Creature from Korean/East Asian folklore (한국 민담): Ghost Bird. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_024_battle_back.png`  
  Subject: black bird with a human face that cries at night, ragged feathers, perched on a dead branch. Creature from Korean/East Asian folklore (한국 민담): Ghost Bird. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_024_mini_icon.png`  
  Subject: black bird with a human face that cries at night, ragged feathers, perched on a dead branch. Creature from Korean/East Asian folklore (한국 민담): Ghost Bird. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 보은 두꺼비 (`yokai_025`)
- **battle_front** → `assets/gen/yokai/yokai_025_battle_front.png`  
  Subject: big warty toad that repays a girl's kindness, puffing venom mist toward a giant centipede. Creature from Korean/East Asian folklore (지네장터 두꺼비 보은 설화): Grateful Toad. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_025_battle_back.png`  
  Subject: big warty toad that repays a girl's kindness, puffing venom mist toward a giant centipede. Creature from Korean/East Asian folklore (지네장터 두꺼비 보은 설화): Grateful Toad. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_025_mini_icon.png`  
  Subject: big warty toad that repays a girl's kindness, puffing venom mist toward a giant centipede. Creature from Korean/East Asian folklore (지네장터 두꺼비 보은 설화): Grateful Toad. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 영노 (`yokai_026`)
- **battle_front** → `assets/gen/yokai/yokai_026_battle_front.png`  
  Subject: masked-dance beast (yeongno) with a grotesque painted face mask, long red tongue, scaled body from the Goseong ogwangdae dance. Creature from Korean/East Asian folklore (한국 야류 설화): Yeongno. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_026_battle_back.png`  
  Subject: masked-dance beast (yeongno) with a grotesque painted face mask, long red tongue, scaled body from the Goseong ogwangdae dance. Creature from Korean/East Asian folklore (한국 야류 설화): Yeongno. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_026_mini_icon.png`  
  Subject: masked-dance beast (yeongno) with a grotesque painted face mask, long red tongue, scaled body from the Goseong ogwangdae dance. Creature from Korean/East Asian folklore (한국 야류 설화): Yeongno. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 손각시 (`yokai_027`)
- **battle_front** → `assets/gen/yokai/yokai_027_battle_front.png`  
  Subject: unmarried maiden ghost (songaksi) in a faded bridal hwarot robe, face veiled by long hair, holding a wilted wedding flower. Creature from Korean/East Asian folklore (한국 무속 (혼인 못 한 처녀 원혼 손각시)): Songaksi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_027_battle_back.png`  
  Subject: unmarried maiden ghost (songaksi) in a faded bridal hwarot robe, face veiled by long hair, holding a wilted wedding flower. Creature from Korean/East Asian folklore (한국 무속 (혼인 못 한 처녀 원혼 손각시)): Songaksi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_027_mini_icon.png`  
  Subject: unmarried maiden ghost (songaksi) in a faded bridal hwarot robe, face veiled by long hair, holding a wilted wedding flower. Creature from Korean/East Asian folklore (한국 무속 (혼인 못 한 처녀 원혼 손각시)): Songaksi. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 여우누이 (`yokai_028`)
- **battle_front** → `assets/gen/yokai/yokai_028_battle_front.png`  
  Subject: fox that became a little sister, sweet child's face with a fox shadow, sleeves stained from eating livestock livers. Creature from Korean/East Asian folklore (여우누이 설화): Fox Sister. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_028_battle_back.png`  
  Subject: fox that became a little sister, sweet child's face with a fox shadow, sleeves stained from eating livestock livers. Creature from Korean/East Asian folklore (여우누이 설화): Fox Sister. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_028_mini_icon.png`  
  Subject: fox that became a little sister, sweet child's face with a fox shadow, sleeves stained from eating livestock livers. Creature from Korean/East Asian folklore (여우누이 설화): Fox Sister. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 청룡 (`yokai_029`)
- **battle_front** → `assets/gen/yokai/yokai_029_battle_front.png`  
  Subject: azure dragon of the east, long serpentine body in blue-green scales, clouds and rain around it (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Azure Dragon. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_029_battle_back.png`  
  Subject: azure dragon of the east, long serpentine body in blue-green scales, clouds and rain around it (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Azure Dragon. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_029_mini_icon.png`  
  Subject: azure dragon of the east, long serpentine body in blue-green scales, clouds and rain around it (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Azure Dragon. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 백호 (`yokai_030`)
- **battle_front** → `assets/gen/yokai/yokai_030_battle_front.png`  
  Subject: white tiger of the west with black stripes, fierce stance, metallic sheen (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_030_battle_back.png`  
  Subject: white tiger of the west with black stripes, fierce stance, metallic sheen (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_030_mini_icon.png`  
  Subject: white tiger of the west with black stripes, fierce stance, metallic sheen (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 주작 (`yokai_031`)
- **battle_front** → `assets/gen/yokai/yokai_031_battle_front.png`  
  Subject: vermilion bird of the south, phoenix-like with fiery tail feathers spread wide (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Vermilion Bird. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_031_battle_back.png`  
  Subject: vermilion bird of the south, phoenix-like with fiery tail feathers spread wide (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Vermilion Bird. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_031_mini_icon.png`  
  Subject: vermilion bird of the south, phoenix-like with fiery tail feathers spread wide (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Vermilion Bird. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 현무 (`yokai_032`)
- **battle_front** → `assets/gen/yokai/yokai_032_battle_front.png`  
  Subject: black tortoise of the north entwined with a black serpent, heavy shell, icy water (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Black Tortoise. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_032_battle_back.png`  
  Subject: black tortoise of the north entwined with a black serpent, heavy shell, icy water (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Black Tortoise. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_032_mini_icon.png`  
  Subject: black tortoise of the north entwined with a black serpent, heavy shell, icy water (sasindo mural style). Creature from Korean/East Asian folklore (한국 사신도): Black Tortoise. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 황룡 (`yokai_033`)
- **battle_front** → `assets/gen/yokai/yokai_033_battle_front.png`  
  Subject: yellow dragon of the center, golden scales, imperial aura, coiled around a jade orb. Creature from Korean/East Asian folklore (한국 신라 설화): Yellow Dragon. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_033_battle_back.png`  
  Subject: yellow dragon of the center, golden scales, imperial aura, coiled around a jade orb. Creature from Korean/East Asian folklore (한국 신라 설화): Yellow Dragon. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_033_mini_icon.png`  
  Subject: yellow dragon of the center, golden scales, imperial aura, coiled around a jade orb. Creature from Korean/East Asian folklore (한국 신라 설화): Yellow Dragon. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 달토끼 (`yokai_034`)
- **battle_front** → `assets/gen/yokai/yokai_034_battle_front.png`  
  Subject: white moon hare pounding herbs with a mortar and pestle, standing on a crescent moon. Creature from Korean/East Asian folklore (한국 전래동화): Moon Hare. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_034_battle_back.png`  
  Subject: white moon hare pounding herbs with a mortar and pestle, standing on a crescent moon. Creature from Korean/East Asian folklore (한국 전래동화): Moon Hare. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_034_mini_icon.png`  
  Subject: white moon hare pounding herbs with a mortar and pestle, standing on a crescent moon. Creature from Korean/East Asian folklore (한국 전래동화): Moon Hare. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 인면조 (`yokai_035`)
- **battle_front** → `assets/gen/yokai/yokai_035_battle_front.png`  
  Subject: bird with a human face from Goguryeo tomb murals, crested hair, long tail feathers, serene uncanny smile. Creature from Korean/East Asian folklore (고구려 덕흥리 고분벽화 (천추·만세)): Human-Faced Bird. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_035_battle_back.png`  
  Subject: bird with a human face from Goguryeo tomb murals, crested hair, long tail feathers, serene uncanny smile. Creature from Korean/East Asian folklore (고구려 덕흥리 고분벽화 (천추·만세)): Human-Faced Bird. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_035_mini_icon.png`  
  Subject: bird with a human face from Goguryeo tomb murals, crested hair, long tail feathers, serene uncanny smile. Creature from Korean/East Asian folklore (고구려 덕흥리 고분벽화 (천추·만세)): Human-Faced Bird. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 인어 (`yokai_036`)
- **battle_front** → `assets/gen/yokai/yokai_036_battle_front.png`  
  Subject: Korean mermaid recorded in Eoudamdam, pale human upper body with long hair, fish tail with dark scales, sad eyes, caught in a fishing net. Creature from Korean/East Asian folklore (어우야담 (흡곡현 인어)): Mermaid of Eoyu. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_036_battle_back.png`  
  Subject: Korean mermaid recorded in Eoudamdam, pale human upper body with long hair, fish tail with dark scales, sad eyes, caught in a fishing net. Creature from Korean/East Asian folklore (어우야담 (흡곡현 인어)): Mermaid of Eoyu. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_036_mini_icon.png`  
  Subject: Korean mermaid recorded in Eoudamdam, pale human upper body with long hair, fish tail with dark scales, sad eyes, caught in a fishing net. Creature from Korean/East Asian folklore (어우야담 (흡곡현 인어)): Mermaid of Eoyu. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 삼두구 (`yokai_037`)
- **battle_front** → `assets/gen/yokai/yokai_037_battle_front.png`  
  Subject: three-headed underworld dog, heads snarling in different directions, chain leash to the afterlife gate. Creature from Korean/East Asian folklore (한국 저승설화): Three-Headed Dog. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_037_battle_back.png`  
  Subject: three-headed underworld dog, heads snarling in different directions, chain leash to the afterlife gate. Creature from Korean/East Asian folklore (한국 저승설화): Three-Headed Dog. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_037_mini_icon.png`  
  Subject: three-headed underworld dog, heads snarling in different directions, chain leash to the afterlife gate. Creature from Korean/East Asian folklore (한국 저승설화): Three-Headed Dog. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 지하국 대적 (`yokai_038`)
- **battle_front** → `assets/gen/yokai/yokai_038_battle_front.png`  
  Subject: giant bandit monster of the underground kingdom, nine heads (the classic version), iron armor, stolen princesses in cages. Creature from Korean/East Asian folklore (지하국대적퇴치 설화): Underworld Bandit King. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_038_battle_back.png`  
  Subject: giant bandit monster of the underground kingdom, nine heads (the classic version), iron armor, stolen princesses in cages. Creature from Korean/East Asian folklore (지하국대적퇴치 설화): Underworld Bandit King. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_038_mini_icon.png`  
  Subject: giant bandit monster of the underground kingdom, nine heads (the classic version), iron armor, stolen princesses in cages. Creature from Korean/East Asian folklore (지하국대적퇴치 설화): Underworld Bandit King. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 영등할망 (`yokai_039`)
- **battle_front** → `assets/gen/yokai/yokai_039_battle_front.png`  
  Subject: wind goddess grandmother of Jeju riding a gale over the sea, seaweed and conch shells in her hair, ragged robes streaming. Creature from Korean/East Asian folklore (제주 영등굿 (바람의 여신)): Yeongdeung Halmang. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_039_battle_back.png`  
  Subject: wind goddess grandmother of Jeju riding a gale over the sea, seaweed and conch shells in her hair, ragged robes streaming. Creature from Korean/East Asian folklore (제주 영등굿 (바람의 여신)): Yeongdeung Halmang. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_039_mini_icon.png`  
  Subject: wind goddess grandmother of Jeju riding a gale over the sea, seaweed and conch shells in her hair, ragged robes streaming. Creature from Korean/East Asian folklore (제주 영등굿 (바람의 여신)): Yeongdeung Halmang. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 용왕 (`yokai_040`)
- **battle_front** → `assets/gen/yokai/yokai_040_battle_front.png`  
  Subject: dragon king of the sea in royal robes with a dragon head, holding a jeweled scepter, underwater palace behind. Creature from Korean/East Asian folklore (한국 심청전/별주부전): Dragon King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_040_battle_back.png`  
  Subject: dragon king of the sea in royal robes with a dragon head, holding a jeweled scepter, underwater palace behind. Creature from Korean/East Asian folklore (한국 심청전/별주부전): Dragon King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_040_mini_icon.png`  
  Subject: dragon king of the sea in royal robes with a dragon head, holding a jeweled scepter, underwater palace behind. Creature from Korean/East Asian folklore (한국 심청전/별주부전): Dragon King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 그슨대 (`yokai_041`)
- **battle_front** → `assets/gen/yokai/yokai_041_battle_front.png`  
  Subject: shadow giant that grows taller the more you look up at it, featureless dark silhouette looming over a road. Creature from Korean/East Asian folklore (한국 민담): Geuseundae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_041_battle_back.png`  
  Subject: shadow giant that grows taller the more you look up at it, featureless dark silhouette looming over a road. Creature from Korean/East Asian folklore (한국 민담): Geuseundae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_041_mini_icon.png`  
  Subject: shadow giant that grows taller the more you look up at it, featureless dark silhouette looming over a road. Creature from Korean/East Asian folklore (한국 민담): Geuseundae. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 두억시니 (`yokai_042`)
- **battle_front** → `assets/gen/yokai/yokai_042_battle_front.png`  
  Subject: hulking brute demon with a massive head crushing down, bulging muscles, broken shackles. Creature from Korean/East Asian folklore (한국 민담): Dueoksini. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_042_battle_back.png`  
  Subject: hulking brute demon with a massive head crushing down, bulging muscles, broken shackles. Creature from Korean/East Asian folklore (한국 민담): Dueoksini. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_042_mini_icon.png`  
  Subject: hulking brute demon with a massive head crushing down, bulging muscles, broken shackles. Creature from Korean/East Asian folklore (한국 민담): Dueoksini. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 어둑시니 (`yokai_043`)
- **battle_front** → `assets/gen/yokai/yokai_043_battle_front.png`  
  Subject: darkness spirit that grows when stared at, a mass of black fog with many faint eyes. Creature from Korean/East Asian folklore (한국 민담): Eoduksini. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_043_battle_back.png`  
  Subject: darkness spirit that grows when stared at, a mass of black fog with many faint eyes. Creature from Korean/East Asian folklore (한국 민담): Eoduksini. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_043_mini_icon.png`  
  Subject: darkness spirit that grows when stared at, a mass of black fog with many faint eyes. Creature from Korean/East Asian folklore (한국 민담): Eoduksini. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 창귀 (`yokai_044`)
- **battle_front** → `assets/gen/yokai/yokai_044_battle_front.png`  
  Subject: servant ghost of a tiger's victim, pale corpse-like man with torn clothes, leading the tiger toward its next prey. Creature from Korean/East Asian folklore (한국 박지원 호질): Changgwi. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_044_battle_back.png`  
  Subject: servant ghost of a tiger's victim, pale corpse-like man with torn clothes, leading the tiger toward its next prey. Creature from Korean/East Asian folklore (한국 박지원 호질): Changgwi. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_044_mini_icon.png`  
  Subject: servant ghost of a tiger's victim, pale corpse-like man with torn clothes, leading the tiger toward its next prey. Creature from Korean/East Asian folklore (한국 박지원 호질): Changgwi. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 산군 (`yokai_045`)
- **battle_front** → `assets/gen/yokai/yokai_045_battle_front.png`  
  Subject: mountain lord tiger (sangun), enormous striped tiger with an old man's wisdom in its eyes, pine trees and mist. Creature from Korean/East Asian folklore (한국 호랑이 설화): Sangun Mountain Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_045_battle_back.png`  
  Subject: mountain lord tiger (sangun), enormous striped tiger with an old man's wisdom in its eyes, pine trees and mist. Creature from Korean/East Asian folklore (한국 호랑이 설화): Sangun Mountain Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_045_mini_icon.png`  
  Subject: mountain lord tiger (sangun), enormous striped tiger with an old man's wisdom in its eyes, pine trees and mist. Creature from Korean/East Asian folklore (한국 호랑이 설화): Sangun Mountain Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 처녀귀신 (`yokai_046`)
- **battle_front** → `assets/gen/yokai/yokai_046_battle_front.png`  
  Subject: virgin ghost in a white sobok mourning dress, long black hair hanging over her face, blood at the lips, floating. Creature from Korean/East Asian folklore (한국 전설의 고향): Virgin Ghost. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_046_battle_back.png`  
  Subject: virgin ghost in a white sobok mourning dress, long black hair hanging over her face, blood at the lips, floating. Creature from Korean/East Asian folklore (한국 전설의 고향): Virgin Ghost. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_046_mini_icon.png`  
  Subject: virgin ghost in a white sobok mourning dress, long black hair hanging over her face, blood at the lips, floating. Creature from Korean/East Asian folklore (한국 전설의 고향): Virgin Ghost. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 몽달귀신 (`yokai_047`)
- **battle_front** → `assets/gen/yokai/yokai_047_battle_front.png`  
  Subject: bachelor ghost in a blue robe with a topknot, sorrowful face, carrying a wedding goose. Creature from Korean/East Asian folklore (한국 야담): Bachelor Ghost. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_047_battle_back.png`  
  Subject: bachelor ghost in a blue robe with a topknot, sorrowful face, carrying a wedding goose. Creature from Korean/East Asian folklore (한국 야담): Bachelor Ghost. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_047_mini_icon.png`  
  Subject: bachelor ghost in a blue robe with a topknot, sorrowful face, carrying a wedding goose. Creature from Korean/East Asian folklore (한국 야담): Bachelor Ghost. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 달걀귀신 (`yokai_048`)
- **battle_front** → `assets/gen/yokai/yokai_048_battle_front.png`  
  Subject: faceless ghost with a smooth egg-like blank face, white robe, standing in a dark alley. Creature from Korean/East Asian folklore (한국 민담): Egg Face Ghost. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_048_battle_back.png`  
  Subject: faceless ghost with a smooth egg-like blank face, white robe, standing in a dark alley. Creature from Korean/East Asian folklore (한국 민담): Egg Face Ghost. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_048_mini_icon.png`  
  Subject: faceless ghost with a smooth egg-like blank face, white robe, standing in a dark alley. Creature from Korean/East Asian folklore (한국 민담): Egg Face Ghost. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 야광귀 (`yokai_049`)
- **battle_front** → `assets/gen/yokai/yokai_049_battle_front.png`  
  Subject: small night imp that steals shoes on New Year's night, pointy ears, holding a straw shoe, sieve hung nearby. Creature from Korean/East Asian folklore (한국 세시풍속): Shoe Stealer Imp. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_049_battle_back.png`  
  Subject: small night imp that steals shoes on New Year's night, pointy ears, holding a straw shoe, sieve hung nearby. Creature from Korean/East Asian folklore (한국 세시풍속): Shoe Stealer Imp. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_049_mini_icon.png`  
  Subject: small night imp that steals shoes on New Year's night, pointy ears, holding a straw shoe, sieve hung nearby. Creature from Korean/East Asian folklore (한국 세시풍속): Shoe Stealer Imp. Presence: small and eerie. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 아스왕 (`yokai_050`)
- **battle_front** → `assets/gen/yokai/yokai_050_battle_front.png`  
  Subject: shape-shifting ghoul with a long proboscis tongue, bat-like limbs, perched on a nipa hut roof. Creature from Korean/East Asian folklore (필리핀 전승 (아스왕)): Aswang. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_050_battle_back.png`  
  Subject: shape-shifting ghoul with a long proboscis tongue, bat-like limbs, perched on a nipa hut roof. Creature from Korean/East Asian folklore (필리핀 전승 (아스왕)): Aswang. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_050_mini_icon.png`  
  Subject: shape-shifting ghoul with a long proboscis tongue, bat-like limbs, perched on a nipa hut roof. Creature from Korean/East Asian folklore (필리핀 전승 (아스왕)): Aswang. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 비형랑 (`yokai_051`)
- **battle_front** → `assets/gen/yokai/yokai_051_battle_front.png`  
  Subject: Silla nobleman half-ghost commanding goblins at night, elegant robe, spectral dokkaebi behind him. Creature from Korean/East Asian folklore (한국 삼국유사): Bihyeongrang. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_051_battle_back.png`  
  Subject: Silla nobleman half-ghost commanding goblins at night, elegant robe, spectral dokkaebi behind him. Creature from Korean/East Asian folklore (한국 삼국유사): Bihyeongrang. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_051_mini_icon.png`  
  Subject: Silla nobleman half-ghost commanding goblins at night, elegant robe, spectral dokkaebi behind him. Creature from Korean/East Asian folklore (한국 삼국유사): Bihyeongrang. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 길달 (`yokai_052`)
- **battle_front** → `assets/gen/yokai/yokai_052_battle_front.png`  
  Subject: goblin turned into a fox trying to flee, mid-transformation, sly expression. Creature from Korean/East Asian folklore (한국 삼국유사): Gildal. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_052_battle_back.png`  
  Subject: goblin turned into a fox trying to flee, mid-transformation, sly expression. Creature from Korean/East Asian folklore (한국 삼국유사): Gildal. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_052_mini_icon.png`  
  Subject: goblin turned into a fox trying to flee, mid-transformation, sly expression. Creature from Korean/East Asian folklore (한국 삼국유사): Gildal. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 저승사자 (`yokai_053`)
- **battle_front** → `assets/gen/yokai/yokai_053_battle_front.png`  
  Subject: grim reaper in a black gat hat and black durumagi robe, pale face, holding a scroll of names. Creature from Korean/East Asian folklore (한국 무속신앙): Grim Messenger. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_053_battle_back.png`  
  Subject: grim reaper in a black gat hat and black durumagi robe, pale face, holding a scroll of names. Creature from Korean/East Asian folklore (한국 무속신앙): Grim Messenger. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_053_mini_icon.png`  
  Subject: grim reaper in a black gat hat and black durumagi robe, pale face, holding a scroll of names. Creature from Korean/East Asian folklore (한국 무속신앙): Grim Messenger. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 강림차사 (`yokai_054`)
- **battle_front** → `assets/gen/yokai/yokai_054_battle_front.png`  
  Subject: underworld messenger Gangrim in official armor with a red-tasseled hat, iron chain and warrant tablet. Creature from Korean/East Asian folklore (한국 제주 차사본풀이): Gangrim Messenger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_054_battle_back.png`  
  Subject: underworld messenger Gangrim in official armor with a red-tasseled hat, iron chain and warrant tablet. Creature from Korean/East Asian folklore (한국 제주 차사본풀이): Gangrim Messenger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_054_mini_icon.png`  
  Subject: underworld messenger Gangrim in official armor with a red-tasseled hat, iron chain and warrant tablet. Creature from Korean/East Asian folklore (한국 제주 차사본풀이): Gangrim Messenger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 바리데기 (`yokai_055`)
- **battle_front** → `assets/gen/yokai/yokai_055_battle_front.png`  
  Subject: Princess Bari, abandoned princess turned shaman goddess, holding a flower of life and a bell, colorful shaman robes. Creature from Korean/East Asian folklore (한국 바리공주 설화): Princess Bari. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_055_battle_back.png`  
  Subject: Princess Bari, abandoned princess turned shaman goddess, holding a flower of life and a bell, colorful shaman robes. Creature from Korean/East Asian folklore (한국 바리공주 설화): Princess Bari. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_055_mini_icon.png`  
  Subject: Princess Bari, abandoned princess turned shaman goddess, holding a flower of life and a bell, colorful shaman robes. Creature from Korean/East Asian folklore (한국 바리공주 설화): Princess Bari. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 삼신할머니 (`yokai_056`)
- **battle_front** → `assets/gen/yokai/yokai_056_battle_front.png`  
  Subject: birth goddess grandmother with white hair, holding a rice-straw rope and a newborn's blessing. Creature from Korean/East Asian folklore (한국 무속신앙): Birth Goddess. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_056_battle_back.png`  
  Subject: birth goddess grandmother with white hair, holding a rice-straw rope and a newborn's blessing. Creature from Korean/East Asian folklore (한국 무속신앙): Birth Goddess. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_056_mini_icon.png`  
  Subject: birth goddess grandmother with white hair, holding a rice-straw rope and a newborn's blessing. Creature from Korean/East Asian folklore (한국 무속신앙): Birth Goddess. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 가망신 (`yokai_057`)
- **battle_front** → `assets/gen/yokai/yokai_057_battle_front.png`  
  Subject: guiding spirit in shaman's ribbons with a lantern, leading souls along a path. Creature from Korean/East Asian folklore (한국 무속신앙): Guiding Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_057_battle_back.png`  
  Subject: guiding spirit in shaman's ribbons with a lantern, leading souls along a path. Creature from Korean/East Asian folklore (한국 무속신앙): Guiding Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_057_mini_icon.png`  
  Subject: guiding spirit in shaman's ribbons with a lantern, leading souls along a path. Creature from Korean/East Asian folklore (한국 무속신앙): Guiding Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 성주신 (`yokai_058`)
- **battle_front** → `assets/gen/yokai/yokai_058_battle_front.png`  
  Subject: house guardian spirit living in the roof beam, wrapped in paper and thread bundles, old man face. Creature from Korean/East Asian folklore (한국 가신신앙): House Guardian. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_058_battle_back.png`  
  Subject: house guardian spirit living in the roof beam, wrapped in paper and thread bundles, old man face. Creature from Korean/East Asian folklore (한국 가신신앙): House Guardian. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_058_mini_icon.png`  
  Subject: house guardian spirit living in the roof beam, wrapped in paper and thread bundles, old man face. Creature from Korean/East Asian folklore (한국 가신신앙): House Guardian. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 조왕신 (`yokai_059`)
- **battle_front** → `assets/gen/yokai/yokai_059_battle_front.png`  
  Subject: kitchen fire spirit rising from a hearth, woman in white with a water bowl, flames at her feet. Creature from Korean/East Asian folklore (한국 가신신앙): Kitchen Fire Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_059_battle_back.png`  
  Subject: kitchen fire spirit rising from a hearth, woman in white with a water bowl, flames at her feet. Creature from Korean/East Asian folklore (한국 가신신앙): Kitchen Fire Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_059_mini_icon.png`  
  Subject: kitchen fire spirit rising from a hearth, woman in white with a water bowl, flames at her feet. Creature from Korean/East Asian folklore (한국 가신신앙): Kitchen Fire Spirit. Presence: medium, menacing. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 터주신 (`yokai_060`)
- **battle_front** → `assets/gen/yokai/yokai_060_battle_front.png`  
  Subject: earth ground spirit in a straw-wrapped jar (teoju), mossy stone face peeking out. Creature from Korean/East Asian folklore (한국 가신신앙): Earth Ground Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_060_battle_back.png`  
  Subject: earth ground spirit in a straw-wrapped jar (teoju), mossy stone face peeking out. Creature from Korean/East Asian folklore (한국 가신신앙): Earth Ground Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_060_mini_icon.png`  
  Subject: earth ground spirit in a straw-wrapped jar (teoju), mossy stone face peeking out. Creature from Korean/East Asian folklore (한국 가신신앙): Earth Ground Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 업신 (`yokai_061`)
- **battle_front** → `assets/gen/yokai/yokai_061_battle_front.png`  
  Subject: large wealth-guardian snake (eopgureongi) coiled in a rice chest, calm and glossy. Creature from Korean/East Asian folklore (한국 가신신앙): Wealth Snake Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_061_battle_back.png`  
  Subject: large wealth-guardian snake (eopgureongi) coiled in a rice chest, calm and glossy. Creature from Korean/East Asian folklore (한국 가신신앙): Wealth Snake Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_061_mini_icon.png`  
  Subject: large wealth-guardian snake (eopgureongi) coiled in a rice chest, calm and glossy. Creature from Korean/East Asian folklore (한국 가신신앙): Wealth Snake Spirit. Presence: medium, menacing. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 측간귀신 (`yokai_062`)
- **battle_front** → `assets/gen/yokai/yokai_062_battle_front.png`  
  Subject: latrine wraith, a disheveled woman with long hair who counts her hair, emerging from an outhouse. Creature from Korean/East Asian folklore (한국 민속신앙): Latrine Wraith. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_062_battle_back.png`  
  Subject: latrine wraith, a disheveled woman with long hair who counts her hair, emerging from an outhouse. Creature from Korean/East Asian folklore (한국 민속신앙): Latrine Wraith. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_062_mini_icon.png`  
  Subject: latrine wraith, a disheveled woman with long hair who counts her hair, emerging from an outhouse. Creature from Korean/East Asian folklore (한국 민속신앙): Latrine Wraith. Presence: small and eerie. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 문전신 (`yokai_063`)
- **battle_front** → `assets/gen/yokai/yokai_063_battle_front.png`  
  Subject: gate ward spirit standing by a wooden gate, holding a spear, stern door-god face. Creature from Korean/East Asian folklore (한국 제주 문전본풀이): Gate Ward Spirit. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_063_battle_back.png`  
  Subject: gate ward spirit standing by a wooden gate, holding a spear, stern door-god face. Creature from Korean/East Asian folklore (한국 제주 문전본풀이): Gate Ward Spirit. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_063_mini_icon.png`  
  Subject: gate ward spirit standing by a wooden gate, holding a spear, stern door-god face. Creature from Korean/East Asian folklore (한국 제주 문전본풀이): Gate Ward Spirit. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 아기장수 우투리 (`yokai_064`)
- **battle_front** → `assets/gen/yokai/yokai_064_battle_front.png`  
  Subject: baby hero Uturi with small wings under his arms, strong infant body, armor made of reeds. Creature from Korean/East Asian folklore (한국 민담): Winged Hero Uturi. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_064_battle_back.png`  
  Subject: baby hero Uturi with small wings under his arms, strong infant body, armor made of reeds. Creature from Korean/East Asian folklore (한국 민담): Winged Hero Uturi. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_064_mini_icon.png`  
  Subject: baby hero Uturi with small wings under his arms, strong infant body, armor made of reeds. Creature from Korean/East Asian folklore (한국 민담): Winged Hero Uturi. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 신립의 망령 (`yokai_065`)
- **battle_front** → `assets/gen/yokai/yokai_065_battle_front.png`  
  Subject: drowned ghost of a general in waterlogged armor, river weeds tangled in his helmet. Creature from Korean/East Asian folklore (한국 임진록 야사): General Sin Rip Ghost. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_065_battle_back.png`  
  Subject: drowned ghost of a general in waterlogged armor, river weeds tangled in his helmet. Creature from Korean/East Asian folklore (한국 임진록 야사): General Sin Rip Ghost. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_065_mini_icon.png`  
  Subject: drowned ghost of a general in waterlogged armor, river weeds tangled in his helmet. Creature from Korean/East Asian folklore (한국 임진록 야사): General Sin Rip Ghost. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 자청비 (`yokai_066`)
- **battle_front** → `assets/gen/yokai/yokai_066_battle_front.png`  
  Subject: agriculture goddess Jacheongbi dressed as a young man, carrying a basket of five grains and a flower of resurrection. Creature from Korean/East Asian folklore (제주 세경본풀이 (농경의 여신)): Jacheongbi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_066_battle_back.png`  
  Subject: agriculture goddess Jacheongbi dressed as a young man, carrying a basket of five grains and a flower of resurrection. Creature from Korean/East Asian folklore (제주 세경본풀이 (농경의 여신)): Jacheongbi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_066_mini_icon.png`  
  Subject: agriculture goddess Jacheongbi dressed as a young man, carrying a basket of five grains and a flower of resurrection. Creature from Korean/East Asian folklore (제주 세경본풀이 (농경의 여신)): Jacheongbi. Presence: small and eerie. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 물귀신 (`yokai_067`)
- **battle_front** → `assets/gen/yokai/yokai_067_battle_front.png`  
  Subject: drowning ghost with a bloated pale body and wet hair, reaching up from dark water to drag the living down. Creature from Korean/East Asian folklore (한국 민담): Drowning Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_067_battle_back.png`  
  Subject: drowning ghost with a bloated pale body and wet hair, reaching up from dark water to drag the living down. Creature from Korean/East Asian folklore (한국 민담): Drowning Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_067_mini_icon.png`  
  Subject: drowning ghost with a bloated pale body and wet hair, reaching up from dark water to drag the living down. Creature from Korean/East Asian folklore (한국 민담): Drowning Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 장화홍련 원혼 (`yokai_068`)
- **battle_front** → `assets/gen/yokai/yokai_068_battle_front.png`  
  Subject: two sister ghosts in wet white clothes standing together at a pond, dripping hair, one holding a red lotus. Creature from Korean/East Asian folklore (장화홍련전): Janghwa Hongryeon. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_068_battle_back.png`  
  Subject: two sister ghosts in wet white clothes standing together at a pond, dripping hair, one holding a red lotus. Creature from Korean/East Asian folklore (장화홍련전): Janghwa Hongryeon. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_068_mini_icon.png`  
  Subject: two sister ghosts in wet white clothes standing together at a pond, dripping hair, one holding a red lotus. Creature from Korean/East Asian folklore (장화홍련전): Janghwa Hongryeon. Presence: medium, menacing. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 백택 (`yokai_069`)
- **battle_front** → `assets/gen/yokai/yokai_069_battle_front.png`  
  Subject: white omniscient beast baize with a human-like face, horns and six eyes, holding a scroll of all monsters. Creature from Korean/East Asian folklore (동양 산해경): Baize Omniscient Beast. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_069_battle_back.png`  
  Subject: white omniscient beast baize with a human-like face, horns and six eyes, holding a scroll of all monsters. Creature from Korean/East Asian folklore (동양 산해경): Baize Omniscient Beast. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_069_mini_icon.png`  
  Subject: white omniscient beast baize with a human-like face, horns and six eyes, holding a scroll of all monsters. Creature from Korean/East Asian folklore (동양 산해경): Baize Omniscient Beast. Presence: godlike divine being, overwhelming aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 기린 (`yokai_070`)
- **battle_front** → `assets/gen/yokai/yokai_070_battle_front.png`  
  Subject: qilin with a deer body, dragon scales, single horn, walking on clouds without crushing grass. Creature from Korean/East Asian folklore (동양 설화): Qilin Benevolent Beast. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_070_battle_back.png`  
  Subject: qilin with a deer body, dragon scales, single horn, walking on clouds without crushing grass. Creature from Korean/East Asian folklore (동양 설화): Qilin Benevolent Beast. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_070_mini_icon.png`  
  Subject: qilin with a deer body, dragon scales, single horn, walking on clouds without crushing grass. Creature from Korean/East Asian folklore (동양 설화): Qilin Benevolent Beast. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 봉황 (`yokai_071`)
- **battle_front** → `assets/gen/yokai/yokai_071_battle_front.png`  
  Subject: fenghuang phoenix with five-colored plumage, long elegant tail. Creature from Korean/East Asian folklore (동양 고대설화): Fenghuang Phoenix. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_071_battle_back.png`  
  Subject: fenghuang phoenix with five-colored plumage, long elegant tail. Creature from Korean/East Asian folklore (동양 고대설화): Fenghuang Phoenix. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_071_mini_icon.png`  
  Subject: fenghuang phoenix with five-colored plumage, long elegant tail. Creature from Korean/East Asian folklore (동양 고대설화): Fenghuang Phoenix. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 폰티아낙 (`yokai_072`)
- **battle_front** → `assets/gen/yokai/yokai_072_battle_front.png`  
  Subject: long-haired woman ghost in white under a banana tree, sharp nails, face hidden, frangipani scent visualized as mist. Creature from Korean/East Asian folklore (말레이 전승 (폰티아낙)): Pontianak. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_072_battle_back.png`  
  Subject: long-haired woman ghost in white under a banana tree, sharp nails, face hidden, frangipani scent visualized as mist. Creature from Korean/East Asian folklore (말레이 전승 (폰티아낙)): Pontianak. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_072_mini_icon.png`  
  Subject: long-haired woman ghost in white under a banana tree, sharp nails, face hidden, frangipani scent visualized as mist. Creature from Korean/East Asian folklore (말레이 전승 (폰티아낙)): Pontianak. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 목우유마 (`yokai_073`)
- **battle_front** → `assets/gen/yokai/yokai_073_battle_front.png`  
  Subject: wooden ox automaton from the Three Kingdoms, carved joints and cogs, carrying grain sacks, paper talisman on its back. Creature from Korean/East Asian folklore (중국 삼국지연의 (목우유마)): Wooden Ox. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_073_battle_back.png`  
  Subject: wooden ox automaton from the Three Kingdoms, carved joints and cogs, carrying grain sacks, paper talisman on its back. Creature from Korean/East Asian folklore (중국 삼국지연의 (목우유마)): Wooden Ox. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_073_mini_icon.png`  
  Subject: wooden ox automaton from the Three Kingdoms, carved joints and cogs, carrying grain sacks, paper talisman on its back. Creature from Korean/East Asian folklore (중국 삼국지연의 (목우유마)): Wooden Ox. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 서낭신 (`yokai_074`)
- **battle_front** → `assets/gen/yokai/yokai_074_battle_front.png`  
  Subject: village cairn spirit, a pile of stones with colored cloth strips, an old face formed in the stones. Creature from Korean/East Asian folklore (한국 성황신앙): Village Cairn Spirit. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_074_battle_back.png`  
  Subject: village cairn spirit, a pile of stones with colored cloth strips, an old face formed in the stones. Creature from Korean/East Asian folklore (한국 성황신앙): Village Cairn Spirit. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_074_mini_icon.png`  
  Subject: village cairn spirit, a pile of stones with colored cloth strips, an old face formed in the stones. Creature from Korean/East Asian folklore (한국 성황신앙): Village Cairn Spirit. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 당산나무 신목 (`yokai_075`)
- **battle_front** → `assets/gen/yokai/yokai_075_battle_front.png`  
  Subject: sacred zelkova tree spirit, ancient tree with straw rope and white paper strips, a face in the bark. Creature from Korean/East Asian folklore (한국 당산신앙): Sacred Zelkova. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_075_battle_back.png`  
  Subject: sacred zelkova tree spirit, ancient tree with straw rope and white paper strips, a face in the bark. Creature from Korean/East Asian folklore (한국 당산신앙): Sacred Zelkova. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_075_mini_icon.png`  
  Subject: sacred zelkova tree spirit, ancient tree with straw rope and white paper strips, a face in the bark. Creature from Korean/East Asian folklore (한국 당산신앙): Sacred Zelkova. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 장승 목신 (`yokai_076`)
- **battle_front** → `assets/gen/yokai/yokai_076_battle_front.png`  
  Subject: jangseung totem deity, carved wooden guardian pole with bulging eyes and big grin, black official hat. Creature from Korean/East Asian folklore (한국 장승신앙): Jangseung Totem Deity. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_076_battle_back.png`  
  Subject: jangseung totem deity, carved wooden guardian pole with bulging eyes and big grin, black official hat. Creature from Korean/East Asian folklore (한국 장승신앙): Jangseung Totem Deity. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_076_mini_icon.png`  
  Subject: jangseung totem deity, carved wooden guardian pole with bulging eyes and big grin, black official hat. Creature from Korean/East Asian folklore (한국 장승신앙): Jangseung Totem Deity. Presence: medium, menacing. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 동자귀 (`yokai_077`)
- **battle_front** → `assets/gen/yokai/yokai_077_battle_front.png`  
  Subject: child ghost in a small hanbok holding a pinwheel, eyes too large and dark. Creature from Korean/East Asian folklore (한국 무속설화): Child Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_077_battle_back.png`  
  Subject: child ghost in a small hanbok holding a pinwheel, eyes too large and dark. Creature from Korean/East Asian folklore (한국 무속설화): Child Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_077_mini_icon.png`  
  Subject: child ghost in a small hanbok holding a pinwheel, eyes too large and dark. Creature from Korean/East Asian folklore (한국 무속설화): Child Ghost. Presence: small and eerie. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 텐구 (`yokai_078`)
- **battle_front** → `assets/gen/yokai/yokai_078_battle_front.png`  
  Subject: mountain goblin with a long red nose, crow wings, yamabushi robes, feather fan. Creature from Korean/East Asian folklore (일본 전승 (텐구)): Tengu. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_078_battle_back.png`  
  Subject: mountain goblin with a long red nose, crow wings, yamabushi robes, feather fan. Creature from Korean/East Asian folklore (일본 전승 (텐구)): Tengu. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_078_mini_icon.png`  
  Subject: mountain goblin with a long red nose, crow wings, yamabushi robes, feather fan. Creature from Korean/East Asian folklore (일본 전승 (텐구)): Tengu. Presence: medium, menacing. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 신령 백호 (`yokai_079`)
- **battle_front** → `assets/gen/yokai/yokai_079_battle_front.png`  
  Subject: sacred white tiger, mountain god's steed, glowing stripes of light. Creature from Korean/East Asian folklore (한국 산신설화): Sacred White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_079_battle_back.png`  
  Subject: sacred white tiger, mountain god's steed, glowing stripes of light. Creature from Korean/East Asian folklore (한국 산신설화): Sacred White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_079_mini_icon.png`  
  Subject: sacred white tiger, mountain god's steed, glowing stripes of light. Creature from Korean/East Asian folklore (한국 산신설화): Sacred White Tiger. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 구렁덩덩 신선비 (`yokai_080`)
- **battle_front** → `assets/gen/yokai/yokai_080_battle_front.png`  
  Subject: great serpent bridegroom shedding its skin to become a handsome scholar, half snake half man. Creature from Korean/East Asian folklore (구렁덩덩 신선비 설화): Serpent Scholar. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_080_battle_back.png`  
  Subject: great serpent bridegroom shedding its skin to become a handsome scholar, half snake half man. Creature from Korean/East Asian folklore (구렁덩덩 신선비 설화): Serpent Scholar. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_080_mini_icon.png`  
  Subject: great serpent bridegroom shedding its skin to become a handsome scholar, half snake half man. Creature from Korean/East Asian folklore (구렁덩덩 신선비 설화): Serpent Scholar. Presence: large, ominous presence. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 삼태성군 (`yokai_081`)
- **battle_front** → `assets/gen/yokai/yokai_081_battle_front.png`  
  Subject: three terraces star lord, Taoist deity in starry robes with three glowing stars above. Creature from Korean/East Asian folklore (도교 성수신앙): Three Terraces Star Lord. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_081_battle_back.png`  
  Subject: three terraces star lord, Taoist deity in starry robes with three glowing stars above. Creature from Korean/East Asian folklore (도교 성수신앙): Three Terraces Star Lord. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_081_mini_icon.png`  
  Subject: three terraces star lord, Taoist deity in starry robes with three glowing stars above. Creature from Korean/East Asian folklore (도교 성수신앙): Three Terraces Star Lord. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 벽력장군 (`yokai_082`)
- **battle_front** → `assets/gen/yokai/yokai_082_battle_front.png`  
  Subject: thunderbolt general in armor with a thunder drum ring behind him, lightning spear. Creature from Korean/East Asian folklore (한국 무속 뇌신): Thunderbolt General. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_082_battle_back.png`  
  Subject: thunderbolt general in armor with a thunder drum ring behind him, lightning spear. Creature from Korean/East Asian folklore (한국 무속 뇌신): Thunderbolt General. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_082_mini_icon.png`  
  Subject: thunderbolt general in armor with a thunder drum ring behind him, lightning spear. Creature from Korean/East Asian folklore (한국 무속 뇌신): Thunderbolt General. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 풍백 (`yokai_083`)
- **battle_front** → `assets/gen/yokai/yokai_083_battle_front.png`  
  Subject: wind lord (pungbaek) holding a wind bag, flowing scarves. Creature from Korean/East Asian folklore (한국 단군신화): Wind Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_083_battle_back.png`  
  Subject: wind lord (pungbaek) holding a wind bag, flowing scarves. Creature from Korean/East Asian folklore (한국 단군신화): Wind Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_083_mini_icon.png`  
  Subject: wind lord (pungbaek) holding a wind bag, flowing scarves. Creature from Korean/East Asian folklore (한국 단군신화): Wind Lord. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 우사 (`yokai_084`)
- **battle_front** → `assets/gen/yokai/yokai_084_battle_front.png`  
  Subject: rain master (usa) pouring rain from a gourd, robes of dark clouds. Creature from Korean/East Asian folklore (한국 단군신화): Rain Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_084_battle_back.png`  
  Subject: rain master (usa) pouring rain from a gourd, robes of dark clouds. Creature from Korean/East Asian folklore (한국 단군신화): Rain Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_084_mini_icon.png`  
  Subject: rain master (usa) pouring rain from a gourd, robes of dark clouds. Creature from Korean/East Asian folklore (한국 단군신화): Rain Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 운사 (`yokai_085`)
- **battle_front** → `assets/gen/yokai/yokai_085_battle_front.png`  
  Subject: cloud master (unsa) riding a cloud, holding a cloud fan. Creature from Korean/East Asian folklore (한국 단군신화): Cloud Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_085_battle_back.png`  
  Subject: cloud master (unsa) riding a cloud, holding a cloud fan. Creature from Korean/East Asian folklore (한국 단군신화): Cloud Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_085_mini_icon.png`  
  Subject: cloud master (unsa) riding a cloud, holding a cloud fan. Creature from Korean/East Asian folklore (한국 단군신화): Cloud Master. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 치우천왕 (`yokai_086`)
- **battle_front** → `assets/gen/yokai/yokai_086_battle_front.png`  
  Subject: Chiyou the war god, bronze-faced with horns and many arms holding weapons, fog of war. Creature from Korean/East Asian folklore (동양 고대전설): Chiyou War God. Presence: godlike divine being, overwhelming aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_086_battle_back.png`  
  Subject: Chiyou the war god, bronze-faced with horns and many arms holding weapons, fog of war. Creature from Korean/East Asian folklore (동양 고대전설): Chiyou War God. Presence: godlike divine being, overwhelming aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_086_mini_icon.png`  
  Subject: Chiyou the war god, bronze-faced with horns and many arms holding weapons, fog of war. Creature from Korean/East Asian folklore (동양 고대전설): Chiyou War God. Presence: godlike divine being, overwhelming aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 자미대제 (`yokai_087`)
- **battle_front** → `assets/gen/yokai/yokai_087_battle_front.png`  
  Subject: polar star emperor on a celestial throne, crown of stars. Creature from Korean/East Asian folklore (도교 천문신앙): Polar Star Emperor. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_087_battle_back.png`  
  Subject: polar star emperor on a celestial throne, crown of stars. Creature from Korean/East Asian folklore (도교 천문신앙): Polar Star Emperor. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_087_mini_icon.png`  
  Subject: polar star emperor on a celestial throne, crown of stars. Creature from Korean/East Asian folklore (도교 천문신앙): Polar Star Emperor. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 흑룡 (`yokai_088`)
- **battle_front** → `assets/gen/yokai/yokai_088_battle_front.png`  
  Subject: abyssal black dragon with obsidian scales, rising from a whirlpool. Creature from Korean/East Asian folklore (한국 용설화): Abyssal Black Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_088_battle_back.png`  
  Subject: abyssal black dragon with obsidian scales, rising from a whirlpool. Creature from Korean/East Asian folklore (한국 용설화): Abyssal Black Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_088_mini_icon.png`  
  Subject: abyssal black dragon with obsidian scales, rising from a whirlpool. Creature from Korean/East Asian folklore (한국 용설화): Abyssal Black Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 응룡 (`yokai_089`)
- **battle_front** → `assets/gen/yokai/yokai_089_battle_front.png`  
  Subject: winged rain dragon (yinglong) with feathered wings, storm clouds. Creature from Korean/East Asian folklore (동양 산해경): Winged Rain Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_089_battle_back.png`  
  Subject: winged rain dragon (yinglong) with feathered wings, storm clouds. Creature from Korean/East Asian folklore (동양 산해경): Winged Rain Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_089_mini_icon.png`  
  Subject: winged rain dragon (yinglong) with feathered wings, storm clouds. Creature from Korean/East Asian folklore (동양 산해경): Winged Rain Dragon. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 가릉빈가 (`yokai_090`)
- **battle_front** → `assets/gen/yokai/yokai_090_battle_front.png`  
  Subject: kalavinka, bird with a human head of a beautiful singer, jeweled plumage. Creature from Korean/East Asian folklore (불교 설화): Kalavinka Bird. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_090_battle_back.png`  
  Subject: kalavinka, bird with a human head of a beautiful singer, jeweled plumage. Creature from Korean/East Asian folklore (불교 설화): Kalavinka Bird. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_090_mini_icon.png`  
  Subject: kalavinka, bird with a human head of a beautiful singer, jeweled plumage. Creature from Korean/East Asian folklore (불교 설화): Kalavinka Bird. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 긴나라 (`yokai_091`)
- **battle_front** → `assets/gen/yokai/yokai_091_battle_front.png`  
  Subject: kinnara celestial musician, half-human half-bird, playing a flute. Creature from Korean/East Asian folklore (불교 팔부중): Kinnara Singer. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_091_battle_back.png`  
  Subject: kinnara celestial musician, half-human half-bird, playing a flute. Creature from Korean/East Asian folklore (불교 팔부중): Kinnara Singer. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_091_mini_icon.png`  
  Subject: kinnara celestial musician, half-human half-bird, playing a flute. Creature from Korean/East Asian folklore (불교 팔부중): Kinnara Singer. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 마후라가 (`yokai_092`)
- **battle_front** → `assets/gen/yokai/yokai_092_battle_front.png`  
  Subject: mahoraga great serpent deity with a human torso and serpent body. Creature from Korean/East Asian folklore (불교 팔부중): Mahoraga Great Serpent. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_092_battle_back.png`  
  Subject: mahoraga great serpent deity with a human torso and serpent body. Creature from Korean/East Asian folklore (불교 팔부중): Mahoraga Great Serpent. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_092_mini_icon.png`  
  Subject: mahoraga great serpent deity with a human torso and serpent body. Creature from Korean/East Asian folklore (불교 팔부중): Mahoraga Great Serpent. Presence: large, ominous presence. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 건달바 (`yokai_093`)
- **battle_front** → `assets/gen/yokai/yokai_093_battle_front.png`  
  Subject: gandharva spirit of fragrance and music, floating in incense smoke. Creature from Korean/East Asian folklore (불교 팔부중): Gandharva Spirit. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_093_battle_back.png`  
  Subject: gandharva spirit of fragrance and music, floating in incense smoke. Creature from Korean/East Asian folklore (불교 팔부중): Gandharva Spirit. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_093_mini_icon.png`  
  Subject: gandharva spirit of fragrance and music, floating in incense smoke. Creature from Korean/East Asian folklore (불교 팔부중): Gandharva Spirit. Presence: large, ominous presence. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 야차 (`yokai_094`)
- **battle_front** → `assets/gen/yokai/yokai_094_battle_front.png`  
  Subject: yaksha swift guardian, fierce blue-skinned demon with fangs and a trident. Creature from Korean/East Asian folklore (불교 팔부중): Yaksha Swift Guardian. Presence: large, ominous presence. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_094_battle_back.png`  
  Subject: yaksha swift guardian, fierce blue-skinned demon with fangs and a trident. Creature from Korean/East Asian folklore (불교 팔부중): Yaksha Swift Guardian. Presence: large, ominous presence. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_094_mini_icon.png`  
  Subject: yaksha swift guardian, fierce blue-skinned demon with fangs and a trident. Creature from Korean/East Asian folklore (불교 팔부중): Yaksha Swift Guardian. Presence: large, ominous presence. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 아수라 (`yokai_095`)
- **battle_front** → `assets/gen/yokai/yokai_095_battle_front.png`  
  Subject: asura fighting lord with three faces and six arms, battle-scarred. Creature from Korean/East Asian folklore (불교 팔부중): Asura Fighting Lord. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_095_battle_back.png`  
  Subject: asura fighting lord with three faces and six arms, battle-scarred. Creature from Korean/East Asian folklore (불교 팔부중): Asura Fighting Lord. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_095_mini_icon.png`  
  Subject: asura fighting lord with three faces and six arms, battle-scarred. Creature from Korean/East Asian folklore (불교 팔부중): Asura Fighting Lord. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 나찰 (`yokai_096`)
- **battle_front** → `assets/gen/yokai/yokai_096_battle_front.png`  
  Subject: rakshasa wrath demon, red skin, wild hair, claws and necklace of skulls. Creature from Korean/East Asian folklore (동양 불교설화): Rakshasa Wrath Demon. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_096_battle_back.png`  
  Subject: rakshasa wrath demon, red skin, wild hair, claws and necklace of skulls. Creature from Korean/East Asian folklore (동양 불교설화): Rakshasa Wrath Demon. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_096_mini_icon.png`  
  Subject: rakshasa wrath demon, red skin, wild hair, claws and necklace of skulls. Creature from Korean/East Asian folklore (동양 불교설화): Rakshasa Wrath Demon. Presence: large, ominous presence. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 다문천왕 (`yokai_097`)
- **battle_front** → `assets/gen/yokai/yokai_097_battle_front.png`  
  Subject: heavenly king of the north (Damuncheonwang) in temple-guardian armor, holding a pagoda. Creature from Korean/East Asian folklore (불교 사천왕): Vaisravana North King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_097_battle_back.png`  
  Subject: heavenly king of the north (Damuncheonwang) in temple-guardian armor, holding a pagoda. Creature from Korean/East Asian folklore (불교 사천왕): Vaisravana North King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_097_mini_icon.png`  
  Subject: heavenly king of the north (Damuncheonwang) in temple-guardian armor, holding a pagoda. Creature from Korean/East Asian folklore (불교 사천왕): Vaisravana North King. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 증장천왕 (`yokai_098`)
- **battle_front** → `assets/gen/yokai/yokai_098_battle_front.png`  
  Subject: heavenly king of the south (Jeungjangcheonwang), holding a sword, fierce face. Creature from Korean/East Asian folklore (불교 사천왕): Virudhaka South King. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_098_battle_back.png`  
  Subject: heavenly king of the south (Jeungjangcheonwang), holding a sword, fierce face. Creature from Korean/East Asian folklore (불교 사천왕): Virudhaka South King. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_098_mini_icon.png`  
  Subject: heavenly king of the south (Jeungjangcheonwang), holding a sword, fierce face. Creature from Korean/East Asian folklore (불교 사천왕): Virudhaka South King. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 광목천왕 (`yokai_099`)
- **battle_front** → `assets/gen/yokai/yokai_099_battle_front.png`  
  Subject: heavenly king of the west (Gwangmokcheonwang), holding a dragon and a pearl. Creature from Korean/East Asian folklore (불교 사천왕): Virupaksa West King. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_099_battle_back.png`  
  Subject: heavenly king of the west (Gwangmokcheonwang), holding a dragon and a pearl. Creature from Korean/East Asian folklore (불교 사천왕): Virupaksa West King. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_099_mini_icon.png`  
  Subject: heavenly king of the west (Gwangmokcheonwang), holding a dragon and a pearl. Creature from Korean/East Asian folklore (불교 사천왕): Virupaksa West King. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 지국천왕 (`yokai_100`)
- **battle_front** → `assets/gen/yokai/yokai_100_battle_front.png`  
  Subject: heavenly king of the east (Jigukcheonwang), holding a lute (bipa). Creature from Korean/East Asian folklore (불교 사천왕): Dhrtarastra East King. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_100_battle_back.png`  
  Subject: heavenly king of the east (Jigukcheonwang), holding a lute (bipa). Creature from Korean/East Asian folklore (불교 사천왕): Dhrtarastra East King. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_100_mini_icon.png`  
  Subject: heavenly king of the east (Jigukcheonwang), holding a lute (bipa). Creature from Korean/East Asian folklore (불교 사천왕): Dhrtarastra East King. Presence: towering spirit-lord presence with an aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 염라대왕 (`yokai_101`)
- **battle_front** → `assets/gen/yokai/yokai_101_battle_front.png`  
  Subject: King Yama judge of the underworld, black official robe, judge's tablet, mirror of karma. Creature from Korean/East Asian folklore (명부 시왕설화): King Yama Fifth Court. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_101_battle_back.png`  
  Subject: King Yama judge of the underworld, black official robe, judge's tablet, mirror of karma. Creature from Korean/East Asian folklore (명부 시왕설화): King Yama Fifth Court. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_101_mini_icon.png`  
  Subject: King Yama judge of the underworld, black official robe, judge's tablet, mirror of karma. Creature from Korean/East Asian folklore (명부 시왕설화): King Yama Fifth Court. Presence: godlike divine being, overwhelming aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 진광대왕 (`yokai_102`)
- **battle_front** → `assets/gen/yokai/yokai_102_battle_front.png`  
  Subject: first court king (Jingwang), stern judge in underworld robes with a ledger. Creature from Korean/East Asian folklore (명부 시왕설화): King Qinguang First Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_102_battle_back.png`  
  Subject: first court king (Jingwang), stern judge in underworld robes with a ledger. Creature from Korean/East Asian folklore (명부 시왕설화): King Qinguang First Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_102_mini_icon.png`  
  Subject: first court king (Jingwang), stern judge in underworld robes with a ledger. Creature from Korean/East Asian folklore (명부 시왕설화): King Qinguang First Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 초강대왕 (`yokai_103`)
- **battle_front** → `assets/gen/yokai/yokai_103_battle_front.png`  
  Subject: second court king (Chogang), judge by a river of the dead. Creature from Korean/East Asian folklore (명부 시왕설화): King Chujiang Second Court. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_103_battle_back.png`  
  Subject: second court king (Chogang), judge by a river of the dead. Creature from Korean/East Asian folklore (명부 시왕설화): King Chujiang Second Court. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_103_mini_icon.png`  
  Subject: second court king (Chogang), judge by a river of the dead. Creature from Korean/East Asian folklore (명부 시왕설화): King Chujiang Second Court. Presence: towering spirit-lord presence with an aura. Mood: dripping, drowned, reflections of dark water, cold mist. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 송제대왕 (`yokai_104`)
- **battle_front** → `assets/gen/yokai/yokai_104_battle_front.png`  
  Subject: third court king (Songje), judge with scales. Creature from Korean/East Asian folklore (명부 시왕설화): King Songdi Third Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_104_battle_back.png`  
  Subject: third court king (Songje), judge with scales. Creature from Korean/East Asian folklore (명부 시왕설화): King Songdi Third Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_104_mini_icon.png`  
  Subject: third court king (Songje), judge with scales. Creature from Korean/East Asian folklore (명부 시왕설화): King Songdi Third Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 오관대왕 (`yokai_105`)
- **battle_front** → `assets/gen/yokai/yokai_105_battle_front.png`  
  Subject: fourth court king (Ogwan), judge with a weighing balance of sins. Creature from Korean/East Asian folklore (명부 시왕설화): King Wuguan Fourth Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_105_battle_back.png`  
  Subject: fourth court king (Ogwan), judge with a weighing balance of sins. Creature from Korean/East Asian folklore (명부 시왕설화): King Wuguan Fourth Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_105_mini_icon.png`  
  Subject: fourth court king (Ogwan), judge with a weighing balance of sins. Creature from Korean/East Asian folklore (명부 시왕설화): King Wuguan Fourth Court. Presence: towering spirit-lord presence with an aura. Mood: iron, blades, chains and cold sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 변성대왕 (`yokai_106`)
- **battle_front** → `assets/gen/yokai/yokai_106_battle_front.png`  
  Subject: sixth court king (Byeonseong), judge surrounded by flames. Creature from Korean/East Asian folklore (명부 시왕설화): King Biancheng Sixth Court. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_106_battle_back.png`  
  Subject: sixth court king (Byeonseong), judge surrounded by flames. Creature from Korean/East Asian folklore (명부 시왕설화): King Biancheng Sixth Court. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_106_mini_icon.png`  
  Subject: sixth court king (Byeonseong), judge surrounded by flames. Creature from Korean/East Asian folklore (명부 시왕설화): King Biancheng Sixth Court. Presence: towering spirit-lord presence with an aura. Mood: embers and heat shimmer, still rendered in the green-black palette with rare red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 태산대왕 (`yokai_107`)
- **battle_front** → `assets/gen/yokai/yokai_107_battle_front.png`  
  Subject: seventh court king (Taesan), mountain judge with stone tablets. Creature from Korean/East Asian folklore (명부 시왕설화): King Taishan Seventh Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_107_battle_back.png`  
  Subject: seventh court king (Taesan), mountain judge with stone tablets. Creature from Korean/East Asian folklore (명부 시왕설화): King Taishan Seventh Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_107_mini_icon.png`  
  Subject: seventh court king (Taesan), mountain judge with stone tablets. Creature from Korean/East Asian folklore (명부 시왕설화): King Taishan Seventh Court. Presence: towering spirit-lord presence with an aura. Mood: stone, soil and grave dirt, heavy and ancient. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 전륜대왕 (`yokai_108`)
- **battle_front** → `assets/gen/yokai/yokai_108_battle_front.png`  
  Subject: tenth court king (Jeollyun), judge turning the wheel of rebirth. Creature from Korean/East Asian folklore (명부 시왕설화): King Tenth Wheel Court. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **battle_back** → `assets/gen/yokai/yokai_108_battle_back.png`  
  Subject: tenth court king (Jeollyun), judge turning the wheel of rebirth. Creature from Korean/East Asian folklore (명부 시왕설화): King Tenth Wheel Court. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
- **mini_icon** → `assets/gen/yokai/yokai_108_mini_icon.png`  
  Subject: tenth court king (Jeollyun), judge turning the wheel of rebirth. Creature from Korean/East Asian folklore (명부 시왕설화): King Tenth Wheel Court. Presence: godlike divine being, overwhelming aura. Mood: roots, vines, bark and wind, overgrown and twisting. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.

## 맵 타일셋 (9)

### REG_01 한양 외곽 (`village_outskirts`)
- **tileset** → `assets/gen/tileset/village_outskirts_tileset.png`  
  Subject: Joseon village at night: grass, dirt paths with edges and corners, tall encounter grass, thatched-roof (choga) houses split into roof/wall/door/window tiles, stone walls (doldam), wattle fences, jangdokdae jar platforms, a stone well, village gate jangseung poles, sotdae bird poles, zelkova tree, cairn (seonangdang) with cloth strips.
### REG_01 주막 내부 (`tavern_interior`)
- **tileset** → `assets/gen/tileset/tavern_interior_tileset.png`  
  Subject: tavern interior: oiled-paper ondol floor, hanji paper walls with wooden posts, lattice doors, low table (soban), folded bedding, rice chest (duiju), clay jars, oil lamp, kitchen hearth (agungi), wooden beams.
### REG_01 도선사 고개 (`doseonsa_pass`)
- **tileset** → `assets/gen/tileset/doseonsa_pass_tileset.png`  
  Subject: mountain pass with red fog: rocky slopes, one-way ledges, pine trees, dead trees, stone steps, broken shrine, temple gate, fallen talismans on the ground, crows.
### REG_02 소백산맥 (`sobaek_mountains`)
- **tileset** → `assets/gen/tileset/sobaek_mountains_tileset.png`  
  Subject: rugged mountains: cliffs, waterfalls, deep valleys, hermitage (amja), rope bridge, cave entrances, iron-scarred rocks.
### REG_02 천년 사찰 (`temple`)
- **tileset** → `assets/gen/tileset/temple_tileset.png`  
  Subject: old Buddhist temple: dancheong-painted eaves faded to gray-green, stone pagoda, lanterns, temple bell pavilion, wooden floors, guardian king statues.
### REG_03 남해 포구 (`south_coast`)
- **tileset** → `assets/gen/tileset/south_coast_tileset.png`  
  Subject: fishing harbor and ghostly tidal flats: wooden piers, moored boats, wrecked ships, nets drying, mudflats with pools, fog, lighthouse beacon fire.
### REG_04 지리산 죽림 (`jirisan_bamboo`)
- **tileset** → `assets/gen/tileset/jirisan_bamboo_tileset.png`  
  Subject: dense bamboo forest and fox valley: bamboo walls, abandoned shrine, ancient burial mounds (gobun), stone path, will-o-wisps.
### REG_05 음양당 본거지 (`eumyangdang_fortress`)
- **tileset** → `assets/gen/tileset/eumyangdang_fortress_tileset.png`  
  Subject: eclipse fortress of the cult: black stone floors inlaid with broken taegeuk circles, barrier towers, ritual altars, chained talisman pillars, distorted ley-line cracks glowing green.
### 전투 배경 (`battle_backgrounds`)
- **tileset** → `assets/gen/tileset/battle_backgrounds_tileset.png`  
  Subject: set of battle background strips (grassland at night, mountain pass fog, temple courtyard, tidal flat, bamboo forest, cult altar), each 640x230, top-down-ish ground with a platform ellipse for each combatant.

## 아이템·유물 (26)

### 한지 (`hanji`)
- **item_icon** → `assets/gen/item/hanji_item_icon.png`  
  Subject: stack of handmade mulberry paper sheets tied with a string. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 청심환 (`cheongsimhwan`)
- **item_icon** → `assets/gen/item/cheongsimhwan_item_icon.png`  
  Subject: round herbal pill wrapped in gold foil inside a small lacquered case. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 계약 부적 (`contract_talisman`)
- **item_icon** → `assets/gen/item/contract_talisman_item_icon.png`  
  Subject: yellow paper talisman with red cinnabar script and a knot of red thread. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 화부 (火符) (`fire_talisman`)
- **item_icon** → `assets/gen/item/fire_talisman_item_icon.png`  
  Subject: talisman with a flame sigil, edges smoldering. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 수부 (水符) (`water_talisman`)
- **item_icon** → `assets/gen/item/water_talisman_item_icon.png`  
  Subject: talisman with a wave sigil, dripping water. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 목부 (木符) (`wood_talisman`)
- **item_icon** → `assets/gen/item/wood_talisman_item_icon.png`  
  Subject: talisman with a sprout sigil, a leaf growing from it. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 금부 (金符) (`metal_talisman`)
- **item_icon** → `assets/gen/item/metal_talisman_item_icon.png`  
  Subject: talisman with a needle sigil, metallic sheen. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 토부 (土符) (`earth_talisman`)
- **item_icon** → `assets/gen/item/earth_talisman_item_icon.png`  
  Subject: talisman with a mountain sigil, dusted with soil. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 엽전 (`coins`)
- **item_icon** → `assets/gen/item/coins_item_icon.png`  
  Subject: string of square-holed brass coins (sangpyeong tongbo). Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 메밀묵 (`buckwheat_jelly`)
- **item_icon** → `assets/gen/item/buckwheat_jelly_item_icon.png`  
  Subject: slab of buckwheat jelly on a wooden plate, dokkaebi's favorite offering. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 등불 기름 (`lamp_oil`)
- **item_icon** → `assets/gen/item/lamp_oil_item_icon.png`  
  Subject: small clay bottle of lamp oil glowing faintly green. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 소금 주머니 (`salt_pouch`)
- **item_icon** → `assets/gen/item/salt_pouch_item_icon.png`  
  Subject: cloth pouch of coarse salt used to ward off spirits. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 도술 비급 (`skill_scroll`)
- **item_icon** → `assets/gen/item/skill_scroll_item_icon.png`  
  Subject: rolled secret-technique scroll sealed with a talisman. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 천지음양부 조각 (핵심 아이템) (`cheonji_fragment`)
- **item_icon** → `assets/gen/item/cheonji_fragment_item_icon.png`  
  Subject: torn fragment of an ancient talisman, half black half white, glowing along the tear. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 벽사청 호패 (`bureau_badge`)
- **item_icon** → `assets/gen/item/bureau_badge_item_icon.png`  
  Subject: wooden identification tag (hopae) branded with the exorcist bureau mark. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 도깨비 감투 (`ART_DOKKAEBI_HAT`)
- **item_icon** → `assets/gen/item/ART_DOKKAEBI_HAT_item_icon.png`  
  Subject: goblin's invisibility cap, worn black horsehair cap with a faint shimmer. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 백년 묵은 지네의 알 (`ART_CENTIPEDE_EGG`)
- **item_icon** → `assets/gen/item/ART_CENTIPEDE_EGG_item_icon.png`  
  Subject: sickly green centipede egg with veins, faintly pulsing. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 깨진 여우구슬 (`ART_FOX_MARBLE_SHARD`)
- **item_icon** → `assets/gen/item/ART_FOX_MARBLE_SHARD_item_icon.png`  
  Subject: cracked fox bead glowing from inside. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 귀면와 (도깨비 기와) (`ART_DEMON_TILE`)
- **item_icon** → `assets/gen/item/ART_DEMON_TILE_item_icon.png`  
  Subject: roof-end tile with a goblin face relief. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 벽사 옥경 (玉鏡) (`ART_JADE_MIRROR`)
- **item_icon** → `assets/gen/item/ART_JADE_MIRROR_item_icon.png`  
  Subject: jade exorcism mirror with a bronze frame. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 신령 무당방울 (`ART_GOLDEN_BELL`)
- **item_icon** → `assets/gen/item/ART_GOLDEN_BELL_item_icon.png`  
  Subject: shaman's cluster of brass bells on a handle with colored ribbons. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 음기도방의 향로 (`ART_SHADOW_INCENSE`)
- **item_icon** → `assets/gen/item/ART_SHADOW_INCENSE_item_icon.png`  
  Subject: black bronze incense burner leaking green smoke. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 저승사자의 명부 조각 (`ART_REAPER_SCROLL`)
- **item_icon** → `assets/gen/item/ART_REAPER_SCROLL_item_icon.png`  
  Subject: torn page of the underworld register with names written in red. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 신목의 파편 (`artifact_001`)
- **item_icon** → `assets/gen/item/artifact_001_item_icon.png`  
  Subject: splinter of a sacred tree wrapped in straw rope and white paper. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 도깨비 은패 (`artifact_002`)
- **item_icon** → `assets/gen/item/artifact_002_item_icon.png`  
  Subject: silver goblin medallion with a horned face. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.
### 해태의 방울 (`artifact_003`)
- **item_icon** → `assets/gen/item/artifact_003_item_icon.png`  
  Subject: bell shaped like a haetae head. Solid flat pure magenta (#FF00FF) background filling the entire canvas. No text, no frame, no grid lines, no ground shadow.

## UI (5)

### ui_windows (`ui_windows`)
- **ui_sheet** → `assets/gen/ui/ui_windows_ui_sheet.png`  
  Subject: window frame 9-slice parts: black panel with double moss-green border lines, small knot ornaments at corners; name tag; choice box; dialog box; all on black.
### ui_icons_menu (`ui_icons_menu`)
- **ui_sheet** → `assets/gen/ui/ui_icons_menu_ui_sheet.png`  
  Subject: 16x16 menu icons in one row: yokai (horned mask), bestiary book, pouch, yin-yang master (taegeuk), brush (save), knot (settings), close X, staff (skill), talisman (spell), feet (flee).
### ui_icons_elements (`ui_icons_elements`)
- **ui_sheet** → `assets/gen/ui/ui_icons_elements_ui_sheet.png`  
  Subject: five 16x16 elemental icons for wood, fire, earth, metal, water (Chinese characters 木 火 土 金 水 carved on dark stone tokens).
### ui_icons_status (`ui_icons_status`)
- **ui_sheet** → `assets/gen/ui/ui_icons_status_ui_sheet.png`  
  Subject: five 16x16 status icons: burn (flame), freeze (ice crystal), seal (talisman with a lock), paralysis (lightning), fear (skull-like ghost face).
### ui_cursor (`ui_cursor`)
- **ui_sheet** → `assets/gen/ui/ui_cursor_ui_sheet.png`  
  Subject: small pixel cursor pointing right, a green ghost-fire flame shape, plus a bouncing down-arrow for 'more text'.

## 컨셉 아트 (7)

### key_title (`key_title`)
- **concept** → `assets/gen/concept/key_title_concept.png`  
  Subject: title screen key art: the hero with a green ghost-fire lantern standing before two cursed jangseung poles at a village entrance under a moonless sky, red fog creeping.
### key_prologue (`key_prologue`)
- **concept** → `assets/gen/concept/key_prologue_concept.png`  
  Subject: the mysterious old monk telling a story by an oil lamp in a dark room, only his beard and smile lit.
### key_ch1 (`key_ch1`)
- **concept** → `assets/gen/concept/key_ch1_concept.png`  
  Subject: chapter 1 Doseonsa red fog: mountain temple wrapped in red fog, silhouettes of goblins, the renegade monk Myogak on the stairs.
### key_ch2 (`key_ch2`)
- **concept** → `assets/gen/concept/key_ch2_concept.png`  
  Subject: chapter 2 Sobaek iron beast: giant Bulgasari devouring an iron bell in a rocky valley.
### key_ch3 (`key_ch3`)
- **concept** → `assets/gen/concept/key_ch3_concept.png`  
  Subject: chapter 3 silent harbor: drowned ghosts rising from tidal flats around a wrecked boat at night.
### key_ch4 (`key_ch4`)
- **concept** → `assets/gen/concept/key_ch4_concept.png`  
  Subject: chapter 4 fox valley maze: white-haired nine-tailed fox in a bamboo forest labyrinth.
### key_final (`key_final`)
- **concept** → `assets/gen/concept/key_final_concept.png`  
  Subject: final chapter eclipse fortress: the cult leader Mukyeong under a black sun, barrier towers glowing green.

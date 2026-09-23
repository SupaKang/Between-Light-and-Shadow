#!/usr/bin/env python3
"""Build the art prompt library from game data.

Inputs : data/art/art_subjects.json, data/art/yokai_visuals.json, data/yokai_*.json
Outputs: assets/prompts/manifest.json  (machine list for tools/gen_image.py batch)
         docs/art/PROMPT_LIBRARY.md    (human-readable, grouped by category)
Usage  : python3 tools/make_art_prompts.py
"""
import glob
import json
import os

subj = json.load(open("data/art/art_subjects.json", encoding="utf-8"))
vis = json.load(open("data/art/yokai_visuals.json", encoding="utf-8"))
style = subj["style"]
TYPES = dict(style["types"])
TYPES["mini_icon"] = {"size": "32x32", "layout": "tiny party/bestiary icon of the creature's head and upper body, centered, bold silhouette",
                      "import": {"size": [32, 32]}}
SPRITE_TYPES = {"field_down", "field_side", "field_up", "portrait", "battle_front", "battle_back", "item_icon", "mini_icon"}
ELEMENT_MOOD = {  # 오행
    "Wood": "roots, vines, bark and wind, overgrown and twisting",
    "Fire": "embers and heat shimmer, still rendered in the green-black palette with rare red",
    "Earth": "stone, soil and grave dirt, heavy and ancient",
    "Metal": "iron, blades, chains and cold sheen",
    "Water": "dripping, drowned, reflections of dark water, cold mist",
}
GRADE_SCALE = {"I": "small and eerie", "II": "medium, menacing", "III": "large, ominous presence",
               "IV": "towering spirit-lord presence with an aura", "V": "godlike divine being, overwhelming aura"}


def prompt(kind, desc, extra=""):
    t = TYPES[kind]
    parts = [style["global"], f"Asset: {t['layout']}. Target size {t['size']} pixels (drawn large, pixel-perfect).", f"Subject: {desc}."]
    if extra:
        parts.append(extra)
    if kind in SPRITE_TYPES:
        parts.append(style["sprite_bg"])
    return " ".join(parts)


entries = []


SHORT = {"field_down": "RPG chibi character sprite, front view", "field_side": "RPG chibi character sprite, side view",
         "field_up": "RPG chibi character sprite, back view",
         "portrait": "character bust portrait, three-quarter view, clear face outline",
         "battle_front": "monster battle sprite, full body, front view",
         "battle_back": "character battle sprite seen from behind",
         "tileset": "top-down RPG tileset sheet, 32px grid tiles",
         "item_icon": "single inventory item icon", "mini_icon": "small creature head icon",
         "ui_sheet": "game UI parts sheet", "concept": "top-down RPG game screenshot"}


def short_prompt(kind, desc):
    """Compact prompt for Stable Diffusion-family models (AI Horde), which read only ~75 tokens."""
    bg = ", solid magenta background" if kind in SPRITE_TYPES else ""
    return (f"pixel art, {SHORT[kind]}, {desc.split(',')[0]}, {', '.join(desc.split(',')[1:3]).strip()}, "
            f"Joseon Korean folk horror, black and dark green palette, crisp pixels{bg}"
            " ### blurry, anti-aliasing, 3d render, photo, text, watermark, extra limbs")


def add(category, subject_id, name_ko, kind, desc, extra="", meta=None):
    entries.append({
        "id": f"{category}/{subject_id}/{kind}",
        "category": category,
        "subject": subject_id,
        "name_ko": name_ko,
        "type": kind,
        "out": f"assets/gen/{category}/{subject_id}_{kind}.png",
        "prompt": prompt(kind, desc, extra),
        "prompt_short": short_prompt(kind, desc),
        "import": TYPES[kind].get("import"),
        "meta": meta or {},
    })


def age_cues(age):
    """Visible age markers so chibi proportions do not make every NPC look young."""
    if age < 13: return "a small child, round face, short limbs"
    if age < 30: return "youthful face"
    if age < 45: return "adult face, faint lines around the eyes"
    if age < 60: return ("middle-aged: wrinkles and laugh lines, some gray hair, sturdier or heavier build; "
                         "mature adult face even in chibi style, never childlike")
    return ("elderly: deep wrinkles, white or gray hair, slightly hunched posture, thin frame, slow stance; "
            "clearly old even in chibi style, never childlike")


for n in subj["npcs"]:
    for kind in n["types"]:
        add("npc", n["id"], n["name_ko"], kind, n["desc"], extra=f"Age about {n['age']}: {age_cues(n['age'])}.",
            meta={"chapter": n["chapter"], "age": n["age"]})

for f in sorted(glob.glob("data/yokai_[0-9]*.json")):
    y = json.load(open(f, encoding="utf-8"))
    desc = vis.get(y["id"])
    if not desc:
        raise SystemExit(f"missing visual description for {y['id']} ({y['name_ko']})")
    extra = (f"Creature from Korean/East Asian folklore ({y['folklore_source']}): {y['name_en']}. "
             f"Presence: {GRADE_SCALE.get(y['grade'], '')}. Mood: {ELEMENT_MOOD.get(y['element'], '')}.")
    meta = {"dex": y["dex_number"], "grade": y["grade"], "element": y["element"], "verify": "VERIFY" in desc}
    for kind in ("battle_front", "battle_back", "mini_icon"):
        add("yokai", y["id"], y["name_ko"], kind, desc.replace(" (VERIFY)", ""), extra, meta)

for t in subj["tilesets"]:
    add("tileset", t["id"], t["region"], "tileset", t["desc"])
for it in subj["items"]:
    add("item", it["id"], it["name_ko"], "item_icon", it["desc"])
for u in subj["ui"]:
    add("ui", u["id"], u["id"], "ui_sheet", u["desc"])
for c in subj["concepts"]:
    add("concept", c["id"], c["id"], "concept", c["desc"])

os.makedirs("assets/prompts", exist_ok=True)
json.dump({"style": style, "entries": entries}, open("assets/prompts/manifest.json", "w", encoding="utf-8"), ensure_ascii=False, indent=1)

LABEL = {"npc": "NPC·인물", "yokai": "요괴 (108종)", "tileset": "맵 타일셋", "item": "아이템·유물", "ui": "UI", "concept": "컨셉 아트"}
md = ["# 아트 제작 프롬프트 라이브러리",
      "",
      "> `tools/make_art_prompts.py`로 생성한 파일. 직접 고치지 말 것. 소재는 `data/art/*.json`에서 고치고 스크립트를 다시 실행한다.",
      "",
      f"총 {len(entries)}개 프롬프트. 생성: `python3 tools/gen_image.py batch assets/prompts/manifest.json <id 접두어>`",
      "",
      "## 공통 스타일", "", "```", style["global"], "```", "",
      "## 에셋 규격", "", "| 종류 | 크기 | 구성 |", "|---|---|---|"]
for k, t in TYPES.items():
    md.append(f"| `{k}` | {t['size']} | {t['layout']} |")
for cat in ("npc", "yokai", "tileset", "item", "ui", "concept"):
    rows = [e for e in entries if e["category"] == cat]
    md += ["", f"## {LABEL[cat]} ({len(rows)})", ""]
    last = None
    for e in rows:
        if e["subject"] != last:
            flag = " ⚠VERIFY" if e["meta"].get("verify") else ""
            md.append(f"### {e['name_ko']} (`{e['subject']}`){flag}")
            last = e["subject"]
        body = e["prompt"].split("Subject: ", 1)[1]
        md.append(f"- **{e['type']}** → `{e['out']}`  \n  Subject: {body}")
os.makedirs("docs/art", exist_ok=True)
open("docs/art/PROMPT_LIBRARY.md", "w", encoding="utf-8").write("\n".join(md) + "\n")
counts = {c: sum(1 for e in entries if e["category"] == c) for c in LABEL}
print(len(entries), "prompts", counts)

# Generated art sources

The current production prompts are generated. Read `docs/art/ART_BIBLE.md` for the pipeline and
`docs/art/PROMPT_LIBRARY.md` for the full list; the machine-readable list is `assets/prompts/manifest.json`.
Batch output goes to `assets/gen/<category>/`.

Earlier spike images kept for reference:
- `gba/`: GBA-style portraits and battle sprites (monk, jumo, elder, hero_back, jangseung). They are the
  current sources for `src/art_gen.h` until the manifest batch produces replacements.
- `*.png` in this folder: gen-1 4-shade test and `sd/`, `ds/` experiments. None of these are used in the game.

TODO(art): the jangseung trunk inscription must be regenerated (customary 天下大將軍, VERIFY).

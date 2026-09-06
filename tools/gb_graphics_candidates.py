"""Find likely 2bpp tile ranges without exporting or reconstructing images."""
from pathlib import Path
import argparse, json

def score(block):
    if len(block) != 16: return 0.0
    nonzero = sum(b != 0 for b in block) / 16
    transitions = sum((block[i] ^ block[i-1]).bit_count() for i in range(1, 16)) / 120
    return round(0.55 * nonzero + 0.45 * transitions, 4)

def main():
    p = argparse.ArgumentParser(); p.add_argument("rom", type=Path); p.add_argument("--out", type=Path, default=Path("docs/rom_analysis/graphics_candidates.json")); a=p.parse_args()
    data=a.rom.read_bytes(); rows=[]
    for off in range(0, len(data)-15, 16):
        s=score(data[off:off+16])
        if 0.18 <= s <= 0.78: rows.append((s, off))
    rows.sort(reverse=True)
    result={"policy":"read_only_graphics_shape_candidates_no_export","tile_bytes":16,"candidate_count":len(rows),"top_candidates":[{"offset":f"0x{o:06X}","score":s,"bank":o//0x4000} for s,o in rows[:120]]}
    a.out.parent.mkdir(parents=True,exist_ok=True); a.out.write_text(json.dumps(result,indent=2),encoding="utf-8")
    print(f"[PASS] graphics candidates: {len(rows)} blocks -> {a.out}")
if __name__ == "__main__": main()

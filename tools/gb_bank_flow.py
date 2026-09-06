"""Read-only bank-aware control-flow candidate report for a Game Boy ROM."""
from pathlib import Path
import argparse, json

OPS = {0xC3: ("JP", 3), 0xC2: ("JP NZ", 3), 0xCA: ("JP Z", 3),
       0xD2: ("JP NC", 3), 0xDA: ("JP C", 3), 0xCD: ("CALL", 3),
       0xC4: ("CALL NZ", 3), 0xCC: ("CALL Z", 3), 0xD4: ("CALL NC", 3),
       0xDC: ("CALL C", 3)}

def target(raw): return raw[1] | (raw[2] << 8)

def main():
    p = argparse.ArgumentParser()
    p.add_argument("rom", type=Path)
    p.add_argument("--out", type=Path, default=Path("docs/rom_analysis/bank_flow_candidates.json"))
    a = p.parse_args(); data = a.rom.read_bytes(); bank_size = 0x4000
    banks = []
    for bank in range(0, len(data), bank_size):
        calls = {}; jumps = []
        chunk = data[bank:bank + bank_size]
        for i in range(len(chunk) - 2):
            op = chunk[i]
            if op not in OPS: continue
            addr = target(chunk[i:i+3]); entry = calls if "CALL" in OPS[op][0] else jumps
            if isinstance(entry, dict): entry[f"0x{addr:04X}"] = entry.get(f"0x{addr:04X}", 0) + 1
            else: entry.append({"source": f"0x{bank+i:06X}", "mnemonic": OPS[op][0], "target": f"0x{addr:04X}"})
        banks.append({"bank": bank // bank_size, "call_targets": dict(sorted(calls.items(), key=lambda x: -x[1])[:40]), "jump_candidates": jumps[:200]})
    result = {"policy": "read_only_bank_aware_candidates", "bank_size": bank_size, "banks": banks}
    a.out.parent.mkdir(parents=True, exist_ok=True); a.out.write_text(json.dumps(result, indent=2), encoding="utf-8")
    print(f"[PASS] bank flow candidates: {len(banks)} banks -> {a.out}")

if __name__ == "__main__": main()

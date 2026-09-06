"""Small, dependency-free candidate disassembler for Game Boy LR35902 bytes.

This is an inspection aid only: it does not claim control-flow correctness and
does not extract or redistribute ROM assets.
"""
from pathlib import Path
import argparse
import json

OPLEN = {0x00: 1, 0x01: 3, 0x02: 1, 0x03: 1, 0x04: 1, 0x05: 1, 0x06: 2,
         0x07: 1, 0x08: 3, 0x09: 1, 0x0A: 1, 0x0B: 1, 0x0C: 1, 0x0D: 1,
         0x0E: 2, 0x0F: 1, 0x10: 2, 0x11: 3, 0x12: 1, 0x13: 1, 0x14: 1,
         0x15: 1, 0x16: 2, 0x17: 1, 0x18: 2, 0x19: 1, 0x1A: 1, 0x1B: 1,
         0x1C: 1, 0x1D: 1, 0x1E: 2, 0x1F: 1, 0x20: 2, 0x21: 3, 0x22: 1,
         0x23: 1, 0x24: 1, 0x25: 1, 0x26: 2, 0x27: 1, 0x28: 2, 0x29: 1,
         0x2A: 1, 0x2B: 1, 0x2C: 1, 0x2D: 1, 0x2E: 2, 0x2F: 1, 0x30: 2,
         0x31: 3, 0x32: 1, 0x33: 1, 0x34: 1, 0x35: 1, 0x36: 2, 0x37: 1,
         0x38: 2, 0x39: 1, 0x3A: 1, 0x3B: 1, 0x3C: 1, 0x3D: 1, 0x3E: 2,
         0x3F: 1, 0xC3: 3, 0xCD: 3, 0xC9: 1, 0xC0: 1, 0xC8: 1, 0xD0: 1,
         0xD8: 1, 0xE0: 2, 0xE2: 1, 0xEA: 3, 0xF0: 2, 0xF2: 1, 0xFA: 3,
         0xCB: 2}

NAMES = {0x00: "NOP", 0xC3: "JP", 0xCD: "CALL", 0xC9: "RET", 0xCB: "PREFIX_CB",
         0xEA: "LD (a16),A", 0xFA: "LD A,(a16)", 0xE0: "LDH (a8),A", 0xF0: "LDH A,(a8)",
         0x18: "JR", 0x20: "JR NZ", 0x28: "JR Z", 0x30: "JR NC", 0x38: "JR C"}

def decode(data, start, count):
    rows = []
    i = start
    end = min(len(data), start + count)
    while i < end:
        op = data[i]
        size = OPLEN.get(op, 1)
        raw = data[i:i + size]
        name = NAMES.get(op, f"OP_{op:02X}")
        operand = " ".join(f"{b:02X}" for b in raw[1:])
        rows.append({"offset": f"0x{i:06X}", "opcode": f"0x{op:02X}", "bytes": raw.hex(" "), "mnemonic": name, "operand_bytes": operand})
        i += size
    return rows

def main():
    ap = argparse.ArgumentParser()
    ap.add_argument("rom", type=Path)
    ap.add_argument("--offset", type=lambda x: int(x, 0), default=0x100)
    ap.add_argument("--count", type=int, default=256)
    ap.add_argument("--out", type=Path, default=Path("docs/rom_analysis/gbz80_candidate_disassembly.json"))
    args = ap.parse_args()
    data = args.rom.read_bytes()
    result = {"policy": "read_only_candidate_decode", "offset": args.offset, "count": args.count,
              "rows": decode(data, args.offset, args.count)}
    args.out.parent.mkdir(parents=True, exist_ok=True)
    args.out.write_text(json.dumps(result, ensure_ascii=False, indent=2), encoding="utf-8")
    print(f"[PASS] decoded {len(result['rows'])} candidate instructions -> {args.out}")

if __name__ == "__main__":
    main()

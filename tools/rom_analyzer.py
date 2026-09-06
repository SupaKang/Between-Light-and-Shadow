"""Read-only, dependency-free Game Boy ROM structure analyzer.

This emits structural candidates only; it does not extract or package original
game code, graphics, text, or maps as game assets.
"""
from __future__ import annotations

import argparse
import hashlib
import json
import math
from collections import Counter
from pathlib import Path


ROM_SIZE_CODES = {0x00: 32 * 1024, 0x01: 64 * 1024, 0x02: 128 * 1024,
                  0x03: 256 * 1024, 0x04: 512 * 1024, 0x05: 1024 * 1024,
                  0x06: 2 * 1024 * 1024, 0x07: 4 * 1024 * 1024}


def entropy(blob: bytes) -> float:
    if not blob:
        return 0.0
    counts = Counter(blob)
    return -sum((n / len(blob)) * math.log2(n / len(blob)) for n in counts.values())


def header(data: bytes) -> dict:
    title = data[0x134:0x144].split(b"\0", 1)[0].decode("ascii", "replace")
    return {
        "title_ascii": title,
        "cgb_flag": f"0x{data[0x143]:02X}",
        "cartridge_type": f"0x{data[0x147]:02X}",
        "rom_size_code": f"0x{data[0x148]:02X}",
        "rom_size_declared_bytes": ROM_SIZE_CODES.get(data[0x148], None),
        "ram_size_code": f"0x{data[0x149]:02X}",
        "header_checksum": f"0x{data[0x14D]:02X}",
        "global_checksum": data[0x14E:0x150].hex().upper(),
    }


def opcode_candidates(bank: bytes) -> dict:
    # Candidate counts, not a full disassembly: data bytes can look like opcodes.
    calls = sum(bank[i] in (0xCD, 0xC4, 0xCC, 0xD4, 0xDC) for i in range(len(bank)))
    jumps = sum(bank[i] in (0xC3, 0xC2, 0xCA, 0xD2, 0xDA, 0x18, 0x20, 0x28, 0x30, 0x38)
                for i in range(len(bank)))
    returns = sum(bank[i] in (0xC9, 0xD9, 0xC0, 0xC8, 0xD0, 0xD8) for i in range(len(bank)))
    io_refs = sum(0xE0 <= bank[i] <= 0xE2 or 0xF0 <= bank[i] <= 0xF3 for i in range(len(bank)))
    return {"call_opcode_candidates": calls, "jump_opcode_candidates": jumps,
            "return_opcode_candidates": returns, "io_opcode_candidates": io_refs}


def pointer_candidates(bank: bytes) -> int:
    # Little-endian addresses in the visible 0x4000-0x7FFF bank window.
    count = 0
    for i in range(0, len(bank) - 1, 2):
        value = bank[i] | (bank[i + 1] << 8)
        if 0x4000 <= value <= 0x7FFF:
            count += 1
    return count


def tile_repetition(bank: bytes) -> dict:
    # 2bpp Game Boy tiles are 16 bytes. Report repetition and zero density only.
    tiles = [bank[i:i + 16] for i in range(0, len(bank) - 15, 16)]
    unique = len(set(tiles))
    return {"tile_blocks_16_bytes": len(tiles), "unique_2bpp_candidates": unique,
            "repeat_ratio": round(1 - unique / len(tiles), 4) if tiles else 0.0,
            "zero_byte_ratio": round(bank.count(0) / len(bank), 4) if bank else 0.0}


def analyze(path: Path, out_dir: Path) -> None:
    data = path.read_bytes()
    if len(data) < 0x150:
        raise ValueError("file is too small to be a Game Boy ROM")
    banks = [data[i:i + 0x4000] for i in range(0, len(data), 0x4000)]
    bank_rows = []
    for index, bank in enumerate(banks):
        code = opcode_candidates(bank)
        row = {"bank": index, "file_offset": index * 0x4000,
               "bytes": len(bank), "entropy_bits": round(entropy(bank), 4),
               "pointer_candidates": pointer_candidates(bank), **code,
               **tile_repetition(bank)}
        bank_rows.append(row)
    fingerprint = {
        "file": str(path), "bytes": len(data), "sha256": hashlib.sha256(data).hexdigest(),
        "sha1": hashlib.sha1(data).hexdigest(), "banks_16k": len(banks),
        "header": header(data),
        "analysis_policy": "read_only_structural_candidates_no_asset_extraction"
    }
    out_dir.mkdir(parents=True, exist_ok=True)
    (out_dir / "rom_fingerprint.json").write_text(json.dumps(fingerprint, ensure_ascii=False, indent=2), encoding="utf-8")
    (out_dir / "rom_bank_map.json").write_text(json.dumps({"banks": bank_rows}, ensure_ascii=False, indent=2), encoding="utf-8")
    lines = ["# ROM 정적 분석 리포트", "", f"- 파일: `{path}`", f"- 크기: {len(data):,} bytes",
             f"- SHA-256: `{fingerprint['sha256']}`", f"- SHA-1: `{fingerprint['sha1']}`",
             f"- 16KiB bank 수: {len(banks)}", "", "주의: 아래 수치는 코드/데이터가 섞인 바이너리에 대한 후보 통계이며 확정 디스어셈블 결과가 아니다.", "", "| Bank | Entropy | Pointer 후보 | CALL 후보 | Jump 후보 | 2bpp 반복률 |", "|---:|---:|---:|---:|---:|---:|"]
    for row in bank_rows:
        lines.append(f"| {row['bank']} | {row['entropy_bits']} | {row['pointer_candidates']} | {row['call_opcode_candidates']} | {row['jump_opcode_candidates']} | {row['repeat_ratio']:.4f} |")
    (out_dir / "ROM_STATIC_ANALYSIS.md").write_text("\n".join(lines) + "\n", encoding="utf-8")
    print(f"[PASS] analyzed {path.name}: {len(banks)} banks")
    print(f"[PASS] reports: {out_dir}")


def main() -> int:
    parser = argparse.ArgumentParser()
    parser.add_argument("rom", type=Path)
    parser.add_argument("--out", type=Path, default=Path("docs/rom_analysis"))
    args = parser.parse_args()
    analyze(args.rom, args.out)
    return 0


if __name__ == "__main__":
    raise SystemExit(main())

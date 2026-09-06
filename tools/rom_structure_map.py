"""Merge read-only analysis artifacts into a compact bank structure map."""
from pathlib import Path
import argparse, json

def load(p): return json.loads(p.read_text(encoding="utf-8"))

def main():
    p=argparse.ArgumentParser(); p.add_argument("--dir",type=Path,default=Path("docs/rom_analysis")); p.add_argument("--out",type=Path,default=Path("docs/rom_analysis/ROM_STRUCTURE_MAP.md")); a=p.parse_args()
    d=a.dir; bank=load(d/"rom_bank_map.json"); gfx=load(d/"graphics_ranges.json"); flow=load(d/"bank_flow_candidates.json")
    gr={i:[] for i in range(128)}
    for r in gfx["ranges"]:
        for b in range(r["bank_start"],r["bank_end"]+1): gr[b].append(r)
    lines=["# 롬 구조 통합 지도","","읽기 전용 후보 분석을 통합한 지도다. 확정 디스어셈블리나 에셋 추출 결과가 아니다.","","| Bank | Entropy | CALL 후보 | JP 후보 | 그래픽 범위 후보 | 분류 |","|---:|---:|---:|---:|---:|---|"]
    for item in bank["banks"]:
        b=item["bank"]; ent=item["entropy_bits"]; calls=item["call_opcode_candidates"]; jumps=item["jump_opcode_candidates"]; gs=len(gr[b])
        if gs and calls < 200: kind="그래픽 우세 후보"
        elif calls >= 500: kind="코드 우세 후보"
        elif gs: kind="혼합 후보"
        else: kind="미분류 후보"
        lines.append(f"| {b} | {ent:.3f} | {calls} | {jumps} | {gs} | {kind} |")
    lines += ["","## 해석 규칙","","- `코드 우세 후보`: CALL 후보가 많지만 데이터 바이트가 명령어처럼 보이는 오탐 가능성이 있다.","- `그래픽 우세 후보`: 2bpp 타일 유사 범위와 낮은 호출 후보가 함께 나타난 영역이다.","- `혼합 후보`: 코드·그래픽·테이블이 섞였을 가능성이 있어 추가 검증이 필요하다.","- 모든 결과는 108의 독립적인 구현을 위한 설계 참고 자료로만 사용한다."]
    a.out.write_text("\n".join(lines)+"\n",encoding="utf-8"); print(f"[PASS] structure map -> {a.out}")
if __name__=="__main__": main()

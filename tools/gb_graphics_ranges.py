"""Aggregate 16-byte tile-likeness into contiguous candidate ranges."""
from pathlib import Path
import argparse, json

def main():
    p=argparse.ArgumentParser(); p.add_argument("rom",type=Path); p.add_argument("--out",type=Path,default=Path("docs/rom_analysis/graphics_ranges.json")); a=p.parse_args()
    d=a.rom.read_bytes(); blocks=[]
    for off in range(0,len(d)-15,16):
        b=d[off:off+16]; nz=sum(x!=0 for x in b)/16; bit=sum(x.bit_count() for x in b)/128
        if .20 <= nz <= .95 and .10 <= bit <= .90: blocks.append(off)
    ranges=[]
    for off in blocks:
        if not ranges or off != ranges[-1][1]+16: ranges.append([off,off])
        else: ranges[-1][1]=off
    rows=[]
    for s,e in ranges:
        tiles=(e-s)//16+1
        if tiles>=8: rows.append({"start":f"0x{s:06X}","end":f"0x{e+15:06X}","bytes":e-s+16,"tiles":tiles,"bank_start":s//0x4000,"bank_end":e//0x4000})
    rows.sort(key=lambda x:(-x["tiles"],x["start"]))
    out={"policy":"read_only_contiguous_graphics_candidates_no_export","ranges":rows[:500],"total_ranges":len(rows)}
    a.out.parent.mkdir(parents=True,exist_ok=True); a.out.write_text(json.dumps(out,indent=2),encoding="utf-8")
    print(f"[PASS] grouped {len(rows)} candidate ranges -> {a.out}")
if __name__=="__main__": main()

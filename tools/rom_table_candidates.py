"""Locate text-like and table-like byte regions without exporting content."""
from pathlib import Path
import argparse,json,string

def main():
 p=argparse.ArgumentParser(); p.add_argument("rom",type=Path); p.add_argument("--out",type=Path,default=Path("docs/rom_analysis/text_table_candidates.json")); a=p.parse_args(); d=a.rom.read_bytes(); texts=[]
 i=0
 while i<len(d):
  j=i
  while j<len(d) and (32<=d[j]<=126 or d[j] in (10,13)): j+=1
  if j-i>=6: texts.append({"start":f"0x{i:06X}","end":f"0x{j-1:06X}","bytes":j-i,"bank":i//0x4000})
  i=max(j,i+1)
 tables=[]
 for off in range(0,len(d)-64,16):
  b=d[off:off+64]; distinct=len(set(b)); zero=b.count(0)
  if distinct<=20 and zero<48: tables.append({"offset":f"0x{off:06X}","bank":off//0x4000,"distinct_bytes":distinct,"zero_bytes":zero})
 result={"policy":"read_only_text_and_table_location_candidates_no_content_export","text_runs":texts[:1000],"text_run_count":len(texts),"table_blocks":tables[:1000],"table_block_count":len(tables)}
 a.out.parent.mkdir(parents=True,exist_ok=True); a.out.write_text(json.dumps(result,indent=2),encoding="utf-8"); print(f"[PASS] text runs={len(texts)}, table blocks={len(tables)} -> {a.out}")
if __name__=="__main__": main()

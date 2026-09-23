#!/usr/bin/env python3
"""Convert an ASCII tile-kind map + object list into a Tiled .tmj (one-off migration and quick blocking)."""
import json, sys
KINDS = "#=BRJD,.TShHdeWG"
map_id, name_ko, indoor, rows_path, objs_path, out = sys.argv[1:7]
rows = [r.rstrip("\n") for r in open(rows_path, encoding="utf-8") if r.strip()]
w, h = len(rows[0]), len(rows)
assert all(len(r) == w for r in rows), "ragged rows"
objs = json.load(open(objs_path, encoding="utf-8"))  # [{"type":"npc","x":5,"y":11,"props":{...}}, ...]
ptype = lambda v: "bool" if isinstance(v, bool) else "int" if isinstance(v, int) else "string"
tmj = {"type": "map", "orientation": "orthogonal", "renderorder": "right-down", "infinite": False,
       "width": w, "height": h, "tilewidth": 16, "tileheight": 16, "version": "1.10", "tiledversion": "1.10.2",
       "properties": [{"name": "id", "type": "string", "value": map_id}, {"name": "name_ko", "type": "string", "value": name_ko},
                      {"name": "indoor", "type": "bool", "value": indoor == "1"}],
       "tilesets": [{"firstgid": 1, "source": "placeholder.tsj"}],
       "layers": [{"type": "tilelayer", "name": "ground", "width": w, "height": h, "x": 0, "y": 0, "opacity": 1, "visible": True,
                   "data": [KINDS.index(c) + 1 for r in rows for c in r]},
                  {"type": "objectgroup", "name": "objects", "opacity": 1, "visible": True, "objects": [
                      {"id": i + 1, "type": o["type"], "name": o["props"].get("name", ""), "x": o["x"] * 16, "y": o["y"] * 16,
                       "width": 16, "height": 16, "visible": True,
                       "properties": [{"name": k, "type": ptype(v), "value": v} for k, v in o["props"].items()]}
                      for i, o in enumerate(objs)]}]}
json.dump(tmj, open(out, "w", encoding="utf-8"), ensure_ascii=False, indent=1)
print("wrote", out, f"{w}x{h}", len(objs), "objects")

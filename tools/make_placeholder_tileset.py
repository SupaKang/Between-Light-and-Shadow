#!/usr/bin/env python3
"""Placeholder Tiled tileset: one 16x16 tile per tile-kind char, labelled. Real graphics come later."""
import json
from PIL import Image, ImageDraw

KINDS = "#=BRJD,.TShHdeWG"
COLORS = {"#": "#2a1e18", "=": "#d4a860", "B": "#24345a", "R": "#6b4a32", "J": "#5a3a28", "D": "#b8322a", ",": "#6a9a44",
          ".": "#cfae7a", "T": "#3c5a2a", "S": "#8a8680", "h": "#c8a050", "H": "#d8c8a0", "d": "#a07850", "e": "#b8322a",
          "W": "#24345a", "G": "#a07850"}
im = Image.new("RGB", (16 * len(KINDS), 16))
d = ImageDraw.Draw(im)
for i, k in enumerate(KINDS):
    d.rectangle([i * 16, 0, i * 16 + 15, 15], fill=COLORS[k], outline="#000000")
    d.text((i * 16 + 5, 2), k, fill="#ffffff")
im.save("assets/tiles/placeholder.png")
ts = {"type": "tileset", "name": "placeholder", "tilewidth": 16, "tileheight": 16, "tilecount": len(KINDS),
      "columns": len(KINDS), "image": "../../assets/tiles/placeholder.png", "imagewidth": 16 * len(KINDS), "imageheight": 16,
      "tiles": [{"id": i, "properties": [{"name": "kind", "type": "string", "value": k}]} for i, k in enumerate(KINDS)]}
json.dump(ts, open("data/maps/placeholder.tsj", "w"), indent=1)
print("placeholder tileset:", len(KINDS), "kinds")

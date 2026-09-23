#!/usr/bin/env python3
"""Convert generated art (magenta background) into src/art_gen.h, in the ghost-story palette.

Per sprite: key out magenta -> crop -> 4x working size with contrast + unsharp -> area downsample
-> map each colour to the 16-colour GHOST palette (black/greens, blood red kept, sallow skin kept)
-> ink feature lines where strong edges sit on the darker side -> hex rows (gfx::Sprite, ramp = art::GHOST).

Sources: the fixed list below (earlier spike art in assets/gen/gba) plus every entry of
assets/prompts/manifest.json whose PNG exists and has a single-image import size.
Usage: python3 tools/import_sprites.py
"""
import colorsys
import json
import os
import re
from PIL import Image, ImageEnhance, ImageFilter

# name, source, crop box fractions (x0, y0, x1, y1), target max (w, h)
SPRITES = [
    ("npc_old_monk_portrait_big", "assets/gen/npc/old_monk_portrait.png", (0, 0, 1, 1), (170, 170)),
    ("portrait_elder", "assets/gen/gba/elder.png", (0.1, 0, 0.9, 1), (96, 96)),
    ("battle_hero_back", "assets/gen/gba/hero_back.png", (0, 0, 1, 1), (128, 128)),
    ("battle_jangseung", "assets/gen/gba/jangseung.png", (0, 0, 0.30, 1), (128, 128)),
]

GHOST = [0x040605, 0x0a120c, 0x12201a, 0x1c3426, 0x2a4c34, 0x3e6e44, 0x62a05a, 0xa8e08a,
         0xd8e8c8, 0x9aa890, 0x5e6a5a, 0x3a1216, 0x7a1c1c, 0xc0b494, 0x86785e, 0x22262c]
HEX = "0123456789abcdef"


def ghost_index(r, g, b):
    h, l, s = colorsys.rgb_to_hls(r / 255, g / 255, b / 255)
    if (h < 0.04 or h > 0.93) and s > 0.35 and l < 0.88:
        return 12 if l > 0.3 else 11                     # blood stays red (saturated reds only)
    if 0.03 < h < 0.13 and s > 0.25 and l > 0.45:
        return 13 if l > 0.62 else 14                    # sallow skin
    y = 0.3 * r + 0.59 * g + 0.11 * b                    # everything else: luminance on the green ramp
    return min(8, int(y / 255 * 9.2))


def is_bg(r, g, b):
    return r > 150 and b > 150 and g < 110 and abs(r - b) < 90


def background_mask(im):
    """Alpha mask: magenta key when the model obeyed the prompt, else flood-fill from the border
    with the corner colour (Stable Diffusion models often ignore the magenta instruction)."""
    p, W, H = im.load(), im.width, im.height
    alpha = Image.new("L", im.size, 255)
    ap = alpha.load()
    corners = [p[0, 0], p[W - 1, 0], p[0, H - 1], p[W - 1, H - 1]]
    if sum(is_bg(*c) for c in corners) >= 3:
        for y in range(H):
            for x in range(W):
                if is_bg(*p[x, y]): ap[x, y] = 0
        return alpha
    ref = tuple(sum(c[i] for c in corners) // 4 for i in range(3))
    close = lambda c: sum((c[i] - ref[i]) ** 2 for i in range(3)) < 80 ** 2  # generated flat backgrounds carry noise
    stack = [(x, y) for x in range(W) for y in (0, H - 1)] + [(x, y) for y in range(H) for x in (0, W - 1)]
    while stack:
        x, y = stack.pop()
        if 0 <= x < W and 0 <= y < H and ap[x, y] and close(p[x, y]):
            ap[x, y] = 0
            stack += [(x + 1, y), (x - 1, y), (x, y + 1), (x, y - 1)]
    return alpha


def convert(path, frac, size, edge_t=36):
    im = Image.open(path).convert("RGB")
    W, H = im.size
    im = im.crop((int(frac[0] * W), int(frac[1] * H), int(frac[2] * W), int(frac[3] * H)))
    alpha = background_mask(im)
    bb = alpha.getbbox()
    return convert_image(im.crop(bb), alpha.crop(bb), size, edge_t)


def nearest_index(r, g, b):
    return min(range(16), key=lambda i: (r - (GHOST[i] >> 16)) ** 2 + (g - (GHOST[i] >> 8 & 255)) ** 2 + (b - (GHOST[i] & 255)) ** 2)


def convert_image(im, alpha, size, edge_t=36, outline=False, lift=1.0, mapping=None):
    # Erode the mask and paint the background black so edge pixels blend toward ink, not the
    # sheet's background colour (otherwise a red/magenta fringe survives the downsample).
    alpha = alpha.filter(ImageFilter.MinFilter(5))
    im = Image.composite(im, Image.new("RGB", im.size, (0, 0, 0)), alpha)
    if lift != 1.0:
        im = ImageEnhance.Brightness(im).enhance(lift)
    k = min(size[0] / im.width, size[1] / im.height)
    tw, th = max(1, round(im.width * k)), max(1, round(im.height * k))
    mid = im.resize((tw * 4, th * 4), Image.LANCZOS)
    mid = ImageEnhance.Contrast(mid).enhance(1.3).filter(ImageFilter.UnsharpMask(radius=3, percent=200, threshold=2))
    edges = mid.convert("L").filter(ImageFilter.GaussianBlur(1.2)).filter(ImageFilter.FIND_EDGES).resize((tw, th), Image.BOX)
    small = mid.resize((tw, th), Image.BOX)
    am = alpha.resize((tw, th), Image.BOX).load()
    q = small.quantize(colors=14, method=Image.Quantize.MEDIANCUT, dither=Image.Dither.NONE).convert("RGB").load()
    ep, lp = edges.load(), small.convert("L").load()
    rows = []
    for y in range(th):
        row = ""
        for x in range(tw):
            if am[x, y] < 128:
                row += "."
                continue
            i = (mapping or ghost_index)(*q[x, y])
            if ep[x, y] > edge_t:
                nb = [lp[x + dx, y + dy] for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)) if 0 <= x + dx < tw and 0 <= y + dy < th]
                if lp[x, y] <= sum(nb) / len(nb):
                    i = 0 if lp[x, y] < 100 else 2
            row += HEX[i]
        rows.append(row)
    if outline:  # 1px ink rim keeps tiny field sprites readable on busy tiles
        g = lambda x, y: rows[y][x] if 0 <= x < tw and 0 <= y < th else "."
        rows = ["".join("0" if rows[y][x] == "." and any(g(x + dx, y + dy) not in ".0" for dx, dy in ((1, 0), (-1, 0), (0, 1), (0, -1)))
                        else rows[y][x] for x in range(tw)) for y in range(th)]
    return rows


def blobs(alpha, min_share=0.2):
    """Bounding boxes of the large connected figures in a sheet, left to right (4x downsampled search)."""
    small = alpha.resize((alpha.width // 4, alpha.height // 4), Image.NEAREST)
    p, W, H = small.load(), small.width, small.height
    seen, found = set(), []
    for y in range(H):
        for x in range(W):
            if p[x, y] and (x, y) not in seen:
                stack, pts = [(x, y)], []
                seen.add((x, y))
                while stack:
                    cx, cy = stack.pop()
                    pts.append((cx, cy))
                    for nx, ny in ((cx + 1, cy), (cx - 1, cy), (cx, cy + 1), (cx, cy - 1)):
                        if 0 <= nx < W and 0 <= ny < H and p[nx, ny] and (nx, ny) not in seen:
                            seen.add((nx, ny)); stack.append((nx, ny))
                xs, ys = [q[0] for q in pts], [q[1] for q in pts]
                found.append((len(pts), (min(xs) * 4, min(ys) * 4, max(xs) * 4 + 4, max(ys) * 4 + 4)))
    big = max(a for a, _ in found)
    return sorted([b for a, b in found if a >= big * min_share], key=lambda b: b[0])


def largest_component(alpha):
    """Keep only the biggest connected figure (drops loose props such as a floating lantern string)."""
    small = alpha.resize((max(1, alpha.width // 2), max(1, alpha.height // 2)), Image.NEAREST)
    p, W, H = small.load(), small.width, small.height
    label, best, best_n, cur = {}, None, 0, 0
    for y in range(H):
        for x in range(W):
            if p[x, y] and (x, y) not in label:
                cur += 1; stack = [(x, y)]; label[(x, y)] = cur; n = 0
                while stack:
                    cx, cy = stack.pop(); n += 1
                    for nx, ny in ((cx + 1, cy), (cx - 1, cy), (cx, cy + 1), (cx, cy - 1)):
                        if 0 <= nx < W and 0 <= ny < H and p[nx, ny] and (nx, ny) not in label:
                            label[(nx, ny)] = cur; stack.append((nx, ny))
                if n > best_n: best, best_n = cur, n
    keep = Image.new("L", small.size, 0)
    kp = keep.load()
    for (x, y), l in label.items():
        if l == best: kp[x, y] = 255
    return Image.composite(alpha, Image.new("L", alpha.size, 0), keep.resize(alpha.size, Image.NEAREST))


def step_frame(rows, lift_left):
    """Walking frame from a standing frame: lift one foot 2px (bottom 6 rows, one half of the body)."""
    h, w = len(rows), len(rows[0])
    grid = [list(r) for r in rows]
    cols = range(0, w // 2) if lift_left else range(w // 2, w)
    for x in cols:
        seg = [rows[y][x] for y in range(h - 6, h)]
        for i, y in enumerate(range(h - 6, h)):
            grid[y][x] = seg[i + 2] if i + 2 < len(seg) else "."
    return ["".join(r) for r in grid]


def faces_left(im, alpha):
    """Side views: the face (skin pixels) sits on the side the character looks toward."""
    p, a = im.load(), alpha.load()
    xs = [x for y in range(im.height // 2) for x in range(im.width)
          if a[x, y] and (lambda h, l, s: 0.02 < h < 0.14 and s > 0.2 and l > 0.45)(*colorsys.rgb_to_hls(*(c / 255 for c in p[x, y])))]
    return bool(xs) and sum(xs) / len(xs) < im.width / 2


def field_frames(fig, fa, view, frame):
    tight = fa.getbbox()
    fig, fa = fig.crop(tight), fa.crop(tight)
    if view == "side" and faces_left(fig, fa):  # the game draws side views facing right and flips for LEFT
        fig, fa = fig.transpose(Image.FLIP_LEFT_RIGHT), fa.transpose(Image.FLIP_LEFT_RIGHT)
    rows = convert_image(fig, fa, (frame[0] - 2, frame[1] - 2), edge_t=90, outline=True, lift=1.15, mapping=nearest_index)
    pad_l = (frame[0] - len(rows[0])) // 2
    rows = ["." * pad_l + r + "." * (frame[0] - len(r) - pad_l) for r in rows]
    rows = ["." * frame[0]] * (frame[1] - len(rows)) + rows              # feet on the bottom row
    return {view: rows, view + "_step1": step_frame(rows, True), view + "_step2": step_frame(rows, False)}


def convert_view(path, view, frame):
    im = Image.open(path).convert("RGB")
    alpha = largest_component(background_mask(im))
    return field_frames(im, alpha, view, frame)


# A missing view is borrowed from these subjects (e.g. the night hero reuses the day hero's back view).
FALLBACK_SUBJECT = {"hero_night": "hero"}


def convert_sheet(path, views, frame):
    im = Image.open(path).convert("RGB")
    alpha = background_mask(im)
    out = {}
    for view, bb in zip(views, blobs(alpha)):
        fig, fa = im.crop(bb), largest_component(alpha.crop(bb))
        tight = fa.getbbox()
        fig, fa = fig.crop(tight), fa.crop(tight)
        if view == "side":  # sheets draw the left-facing view; the game flips it for LEFT
            fig, fa = fig.transpose(Image.FLIP_LEFT_RIGHT), fa.transpose(Image.FLIP_LEFT_RIGHT)
        rows = convert_image(fig, fa, (frame[0] - 2, frame[1] - 2), edge_t=90, outline=True, lift=1.15, mapping=nearest_index)
        pad_l = (frame[0] - len(rows[0])) // 2
        rows = ["." * pad_l + r + "." * (frame[0] - len(r) - pad_l) for r in rows]
        rows = ["." * frame[0]] * (frame[1] - len(rows)) + rows          # feet on the bottom row
        out[view] = rows
        out[view + "_step1"] = step_frame(rows, True)
        out[view + "_step2"] = step_frame(rows, False)
    return out


def main():
    todo, sheets, field = list(SPRITES), [], {}
    if os.path.exists("assets/prompts/manifest.json"):
        for e in json.load(open("assets/prompts/manifest.json", encoding="utf-8"))["entries"]:
            imp = e.get("import") or {}
            if "size" in imp and os.path.exists(e["out"]):
                todo.append((f"{e['category']}_{e['subject']}_{e['type']}", e["out"], (0, 0, 1, 1), tuple(imp["size"])))
            if "views" in imp and os.path.exists(e["out"]):
                sheets.append((f"{e['category']}_{e['subject']}_field", e["out"], imp["views"], imp["frame"]))
            if "view" in imp and os.path.exists(e["out"]):
                field.setdefault((e["category"], e["subject"]), {}).update(convert_view(e["out"], imp["view"], imp["frame"]))
    lines = ["// Generated by tools/import_sprites.py (AI-generated sources, see assets/gen/PROMPTS.md). Do not edit.",
             "#pragma once", '#include "art.h"', "", "namespace art {", ""]
    for name, src, frac, size in todo:
        rows = convert(src, frac, size)
        lines.append(f"inline const char* const {name}_rows[] = {{")
        lines += [f'    "{r}",' for r in rows]
        lines.append("};")
        lines.append(f"inline const gfx::Sprite {name}{{{len(rows[0])}, {len(rows)}, {name}_rows, GHOST}};")
        lines.append("")
        print(f"{name}: {len(rows[0])}x{len(rows)}")
    for base, src, views, frame in sheets:
        for key, rows in convert_sheet(src, views, frame).items():
            name = f"{base}_{key}"
            lines.append(f"inline const char* const {name}_rows[] = {{")
            lines += [f'    "{r}",' for r in rows]
            lines.append("};")
            lines.append(f"inline const gfx::Sprite {name}{{{len(rows[0])}, {len(rows)}, {name}_rows, GHOST}};")
            lines.append("")
        print(f"{base}: {', '.join(views)} (+steps)")
    written = set()
    for (cat, subj), frames in field.items():
        base = FALLBACK_SUBJECT.get(subj)
        for view in ("down", "side", "up"):
            for suffix in ("", "_step1", "_step2"):
                key = view + suffix
                rows = frames.get(key) or (field.get((cat, base), {}).get(key) if base else None) or frames.get("down" + suffix)
                name = f"{cat}_{subj}_field_{key}"
                lines.append(f"inline const char* const {name}_rows[] = {{")
                lines += [f'    "{r}",' for r in rows]
                lines.append("};")
                lines.append(f"inline const gfx::Sprite {name}{{{len(rows[0])}, {len(rows)}, {name}_rows, GHOST}};")
                lines.append("")
                written.add(name)
        print(f"{cat}_{subj}_field: {', '.join(sorted(v for v in frames if '_' not in v))}")
    lines.append("}  // namespace art")
    open("src/art_gen.h", "w").write("\n".join(lines) + "\n")
    if os.path.exists("src/art_legacy.h"):  # regenerated sprites replace the kept legacy copies
        leg = open("src/art_legacy.h").read()
        for name in written:
            leg = re.sub(r"inline const char\* const " + name + r"_rows\[\] = \{.*?\};\ninline const gfx::Sprite " + name + r"\{[^\n]*\};\n",
                         "", leg, flags=re.S)
        open("src/art_legacy.h", "w").write(leg)


main()

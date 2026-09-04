#!/usr/bin/env python3
"""
Palette Quantizer for 108: Yin-Yang Chronicle (108: 음양견문록)
Provides pixel-level color quantization and noise cleanup using CIELAB Delta-E matching.
"""

import sys
import os
import argparse
import numpy as np
from PIL import Image

# ---------------------------------------------------------
# Built-in Game Palettes
# ---------------------------------------------------------

# 1. DMG 4-Shade Monochrome (Classic Game Boy Green)
PALETTE_DMG_4 = [
    (0xE0, 0xF8, 0xD0), # 0: Pale Off-White
    (0x88, 0xC0, 0x70), # 1: Light Olive Green
    (0x34, 0x68, 0x56), # 2: Dark Olive Green
    (0x08, 0x18, 0x20), # 3: Darkest Ink Black
]

# 2. Traditional Joseon Dancheong & Obangsaek 16-Color Palette
PALETTE_DANCHEONG_16 = [
    (0x08, 0x18, 0x20), # 0: Ink Black (먹색) - Also Transparent background
    (0xF4, 0xFA, 0xEE), # 1: Hanji Pure White (백색/백호)
    (0xC8, 0x38, 0x28), # 2: Dancheong Vermilion Red (주홍)
    (0x88, 0x20, 0x20), # 3: Crimson Red (단적)
    (0x28, 0x90, 0x58), # 4: Celadon Forest Green (청록/초록)
    (0x18, 0x50, 0x38), # 5: Deep Pine Green (송록)
    (0xF0, 0xC0, 0x30), # 6: Imperial Golden Yellow (황색)
    (0xB8, 0x80, 0x20), # 7: Ochre / Amber (토황/황토)
    (0x28, 0x68, 0xB0), # 8: Royal Cobalt Blue (청색)
    (0x18, 0x38, 0x68), # 9: Indigo Navy (남색)
    (0x88, 0x48, 0x98), # 10: Mystical Purple (자색)
    (0x50, 0x28, 0x68), # 11: Dark Violet (암자)
    (0xE0, 0x70, 0x20), # 12: Fire Ochre Orange (주황)
    (0xA0, 0xA8, 0x98), # 13: Stone Grey (석회/회청)
    (0x60, 0x68, 0x60), # 14: Slate Dark Grey (먹회)
    (0x70, 0x40, 0x28), # 15: Wood Earth Brown (갈색)
]

# 3. Expanded 32-Color Folklore Palette (Rich Battle Sprites & Portraits)
PALETTE_EXTENDED_32 = PALETTE_DANCHEONG_16 + [
    (0x40, 0x00, 0x00), # 16: Blood Burgundy
    (0xFF, 0x80, 0x80), # 17: Soft Peach Pink
    (0x00, 0x40, 0x40), # 18: Abyssal Sea Teal
    (0x80, 0xE0, 0xD0), # 19: Spirit Mint
    (0xD0, 0xE8, 0x60), # 20: Young Leaf Lime
    (0x60, 0x80, 0x10), # 21: Moss Olive
    (0xFF, 0xE8, 0x90), # 22: Pale Sand Gold
    (0x88, 0x60, 0x40), # 23: Weathered Timber
    (0x70, 0x98, 0xD8), # 24: Sky Blue
    (0x08, 0x18, 0x40), # 25: Deep Void Blue
    (0xD8, 0x90, 0xD0), # 26: Lotus Petal Lilac
    (0x30, 0x10, 0x40), # 27: Shadow Plum
    (0xD8, 0xD0, 0xC0), # 28: Aged Paper Tan
    (0x38, 0x38, 0x38), # 29: Charcoal Ash
    (0xFF, 0xA0, 0x40), # 30: Torchlight Amber
    (0x00, 0xC0, 0xA0)  # 31: Ghostly Cyan
]

PALETTES = {
    'dmg4': PALETTE_DMG_4,
    'dancheong16': PALETTE_DANCHEONG_16,
    'extended32': PALETTE_EXTENDED_32
}

# ---------------------------------------------------------
# RGB to CIELAB Color Conversion for Perceptual Distance
# ---------------------------------------------------------

def rgb_to_lab(rgb_arr):
    """Converts an RGB array (0..255) to CIELAB for accurate human eye delta-E calculation."""
    rgb = rgb_arr.astype(np.float64) / 255.0
    mask = rgb > 0.04045
    rgb = np.where(mask, np.power((rgb + 0.055) / 1.055, 2.4), rgb / 12.92)

    matrix = np.array([
        [0.4124564, 0.3575761, 0.1804375],
        [0.2126729, 0.7151522, 0.0721750],
        [0.0193339, 0.1191920, 0.9503041]
    ]).T
    xyz = np.dot(rgb, matrix)

    xyz_ref = np.array([0.95047, 1.00000, 1.08883])
    norm_xyz = xyz / xyz_ref

    mask_xyz = norm_xyz > 0.008856
    f_xyz = np.where(mask_xyz, np.cbrt(norm_xyz), 7.787 * norm_xyz + 16.0 / 116.0)

    L = 116.0 * f_xyz[..., 1] - 16.0
    a = 500.0 * (f_xyz[..., 0] - f_xyz[..., 1])
    b = 200.0 * (f_xyz[..., 1] - f_xyz[..., 2])

    return np.stack([L, a, b], axis=-1)

# ---------------------------------------------------------
# Core Quantization Engine
# ---------------------------------------------------------

def quantize_image(img, palette_name='dancheong16', alpha_threshold=128, use_cielab=True):
    """
    Quantizes an RGBA PIL image to the designated palette.
    Returns: (quantized_rgba_image, palette_indices_array)
    """
    if palette_name not in PALETTES:
        raise ValueError(f"Unknown palette '{palette_name}'. Available: {list(PALETTES.keys())}")

    palette_rgb = np.array(PALETTES[palette_name], dtype=np.uint8)
    img_rgba = img.convert('RGBA')
    arr = np.array(img_rgba)

    h, w, _ = arr.shape
    alpha = arr[:, :, 3]
    rgb = arr[:, :, :3]

    if use_cielab:
        img_lab = rgb_to_lab(rgb)
        pal_lab = rgb_to_lab(palette_rgb)
        dist = np.linalg.norm(img_lab[:, :, None, :] - pal_lab[None, None, :, :], axis=3)
    else:
        r_weight = 0.299
        g_weight = 0.587
        b_weight = 0.114
        diff = rgb[:, :, None, :].astype(np.float32) - palette_rgb[None, None, :, :].astype(np.float32)
        dist = np.sqrt(
            r_weight * (diff[:, :, :, 0] ** 2) +
            g_weight * (diff[:, :, :, 1] ** 2) +
            b_weight * (diff[:, :, :, 2] ** 2)
        )

    indices = np.argmin(dist, axis=2).astype(np.uint8)

    # Reconstruct quantized RGBA image
    out_arr = np.zeros_like(arr)
    out_arr[:, :, :3] = palette_rgb[indices]
    out_arr[:, :, 3] = np.where(alpha >= alpha_threshold, 255, 0)

    # For pixels where alpha is 0, set index to 0 (background)
    indices = np.where(alpha >= alpha_threshold, indices, 0)

    out_img = Image.fromarray(out_arr, mode='RGBA')
    return out_img, indices

def remove_isolated_pixels(indices, max_iterations=1):
    """Post-processing filter to clean single orphan pixels (noise cleanup)."""
    h, w = indices.shape
    cleaned = indices.copy()
    
    for _ in range(max_iterations):
        for y in range(1, h - 1):
            for x in range(1, w - 1):
                val = cleaned[y, x]
                neighbors = [
                    cleaned[y-1, x], cleaned[y+1, x],
                    cleaned[y, x-1], cleaned[y, x+1]
                ]
                if all(n == neighbors[0] and n != val for n in neighbors):
                    cleaned[y, x] = neighbors[0]
    return cleaned

# ---------------------------------------------------------
# CLI Entry Point
# ---------------------------------------------------------

def main():
    parser = argparse.ArgumentParser(description="Quantize AI-generated sprites to Joseon RPG palettes.")
    parser.add_argument("input", help="Path to input image file (PNG/JPG/BMP).")
    parser.add_argument("-o", "--output", help="Path to output quantized PNG file.", default=None)
    parser.add_argument("-p", "--palette", choices=list(PALETTES.keys()), default="dancheong16",
                        help="Target palette: dmg4, dancheong16, extended32 (default: dancheong16)")
    parser.add_argument("--resize", nargs=2, type=int, metavar=('W', 'H'),
                        help="Target resize dimensions (e.g. --resize 48 48 or --resize 16 24)")
    parser.add_argument("--cleanup", action="store_true", help="Apply orphan pixel cleanup filter.")
    parser.add_argument("--alpha-thresh", type=int, default=128, help="Alpha threshold (0-255).")

    args = parser.parse_args()

    if not os.path.exists(args.input):
        print(f"Error: Input file '{args.input}' not found.")
        sys.exit(1)

    img = Image.open(args.input)
    if args.resize:
        img = img.resize((args.resize[0], args.resize[1]), Image.Resampling.NEAREST)

    print(f"Quantizing '{args.input}' ({img.width}x{img.height}) with palette '{args.palette}'...")
    quant_img, indices = quantize_image(img, palette_name=args.palette, alpha_threshold=args.alpha_thresh)

    if args.cleanup:
        cleaned_indices = remove_isolated_pixels(indices)
        palette_rgb = np.array(PALETTES[args.palette], dtype=np.uint8)
        arr = np.array(quant_img)
        arr[:, :, :3] = palette_rgb[cleaned_indices]
        quant_img = Image.fromarray(arr, mode='RGBA')
        print("  - Applied orphan pixel cleanup filter.")

    out_path = args.output
    if not out_path:
        base, ext = os.path.splitext(args.input)
        out_path = f"{base}_quant_{args.palette}.png"

    quant_img.save(out_path)
    print(f"[Done] Quantized image saved to: {out_path}")

if __name__ == "__main__":
    main()

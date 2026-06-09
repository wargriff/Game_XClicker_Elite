#!/usr/bin/env python3
"""Génère favicon.ico pour le .exe à partir des PNG/SVG brand."""

import os
import struct
import sys

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))
sys.path.insert(0, ROOT)

from config.paths import BRAND_DIR, FAVICON_ICO, FAVICON_PNG, FAVICON_SVG


def _write_minimal_ico(path: str):
    """ICO 16x16 noir + accent magenta minimal (sans dépendance)."""
    # PNG 16x16 embedded in ICO — use Pillow if available
    try:
        from PIL import Image, ImageDraw

        img = Image.new("RGBA", (256, 256), (0, 0, 0, 255))
        draw = ImageDraw.Draw(img)
        # stylized diamond / mask shape
        draw.polygon(
            [(128, 20), (220, 128), (128, 236), (36, 128)],
            fill=(255, 0, 200, 255),
            outline=(0, 255, 255, 255),
        )
        draw.text((72, 200), "SOURIS", fill=(255, 255, 255, 255))
        img.save(path, format="ICO", sizes=[(16, 16), (32, 32), (48, 48), (256, 256)])
        print(f"[ICON] favicon.ico généré → {path}")
        return
    except ImportError:
        pass

    # Fallback: copy PNG if exists
    png_candidates = [
        os.path.join(BRAND_DIR, "web-app-manifest-192x192.png"),
        os.path.join(BRAND_DIR, "favicon-96x96.png"),
        FAVICON_PNG,
    ]
    for png in png_candidates:
        if os.path.exists(png):
            try:
                from PIL import Image
                img = Image.open(png).convert("RGBA")
                img.save(path, format="ICO", sizes=[(16, 16), (32, 32), (48, 48), (256, 256)])
                print(f"[ICON] favicon.ico depuis {png}")
                return
            except ImportError:
                break

    print("[ICON] Installez Pillow: pip install Pillow")
    print("[ICON] Ou copiez manuellement favicon.ico dans assets/brand/")


def main():
    os.makedirs(BRAND_DIR, exist_ok=True)
    if os.path.exists(FAVICON_ICO):
        print(f"[ICON] Déjà présent: {FAVICON_ICO}")
        return 0
    _write_minimal_ico(FAVICON_ICO)
    return 0 if os.path.exists(FAVICON_ICO) else 1


if __name__ == "__main__":
    sys.exit(main())

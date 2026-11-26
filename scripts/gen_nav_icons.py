#!/usr/bin/env python3
"""Rasterise the reference XAPK's NavigationView vector icons to PNG.

Source: .ref_extract/apktool_out/res/drawable/ic_*.xml (Android vector
drawables, viewport 960x960 or 24x24).  Only the simple SVG path commands used
by these icons are implemented: M/m L/l H/h V/v C/c S/s Q/q T/t Z/z.
Curves are flattened into polygons; subpaths are combined with even-odd fill
so holes (e.g. the map folds) stay transparent.
"""
import re
import pathlib
from PIL import Image, ImageDraw, ImageChops

ROOT = pathlib.Path(__file__).resolve().parent.parent
SRC = ROOT / ".ref_extract/apktool_out/res/drawable"
DST = ROOT / "app/assets/navIcons"

ICONS = {
    "ic_map": "ic_map",
    "ic_tune": "ic_tune",
    "ic_person": "ic_person",
    "ic_group_people": "ic_group_people",
    "ic_exit": "ic_exit",
    "ic_list": "ic_list",
    "ic_search": "ic_search",
}

TOKEN = re.compile(r"[MmLlHhVvCcSsQqTtZz]|[-+]?(?:\d*\.\d+|\d+)(?:[eE][-+]?\d+)?")


def parse_path(data):
    tokens = TOKEN.findall(data)
    cmds, i = [], 0
    while i < len(tokens):
        t = tokens[i]
        if t in "MmLlHhVvCcSsQqTtZz":
            cmds.append((t, []))
            i += 1
            continue
        if not cmds:
            i += 1
            continue
        cmds[-1][1].append(float(t))
        i += 1
    return cmds


def flatten(cmds):
    subpaths, cur = [], []
    x = y = 0.0
    sx = sy = 0.0
    last_c = last_q = None
    for cmd, nums in cmds:
        rel = cmd.islower()
        c = cmd.upper()
        k = 0

        def pt():
            nonlocal k
            a, b = nums[k], nums[k + 1]
            k += 2
            return (a + x if rel else a, b + y if rel else b)

        if c == "M":
            x, y = pt()
            sx, sy = x, y
            if cur:
                subpaths.append(cur)
            cur = [(x, y)]
            last_c = last_q = None
        elif c == "L":
            x, y = pt()
            cur.append((x, y))
            last_c = last_q = None
        elif c == "H":
            x = nums[k] + (x if rel else 0)
            k += 1
            cur.append((x, y))
            last_c = last_q = None
        elif c == "V":
            y = nums[k] + (y if rel else 0)
            k += 1
            cur.append((x, y))
            last_c = last_q = None
        elif c == "C":
            p1, p2, p3 = pt(), pt(), pt()
            p0 = (x, y)
            for s in range(1, 13):
                t = s / 12.0
                u = 1 - t
                cur.append((u**3*p0[0] + 3*u*u*t*p1[0] + 3*u*t*t*p2[0] + t**3*p3[0],
                            u**3*p0[1] + 3*u*u*t*p1[1] + 3*u*t*t*p2[1] + t**3*p3[1]))
            x, y = p3
            last_c = (p2, p3)
            last_q = None
        elif c == "S":
            p2, p3 = pt(), pt()
            p1 = (2*x - last_c[0][0], 2*y - last_c[0][1]) if last_c and last_c[1] == (x, y) else (x, y)
            p0 = (x, y)
            for s in range(1, 13):
                t = s / 12.0
                u = 1 - t
                cur.append((u**3*p0[0] + 3*u*u*t*p1[0] + 3*u*t*t*p2[0] + t**3*p3[0],
                            u**3*p0[1] + 3*u*u*t*p1[1] + 3*u*t*t*p2[1] + t**3*p3[1]))
            x, y = p3
            last_c = (p2, p3)
            last_q = None
        elif c == "Q":
            p1, p2 = pt(), pt()
            p0 = (x, y)
            for s in range(1, 13):
                t = s / 12.0
                u = 1 - t
                cur.append((u*u*p0[0] + 2*u*t*p1[0] + t*t*p2[0],
                            u*u*p0[1] + 2*u*t*p1[1] + t*t*p2[1]))
            x, y = p2
            last_q = (p1, p2)
            last_c = None
        elif c == "T":
            p2 = pt()
            p1 = (2*x - last_q[0][0], 2*y - last_q[0][1]) if last_q and last_q[1] == (x, y) else (x, y)
            p0 = (x, y)
            for s in range(1, 13):
                t = s / 12.0
                u = 1 - t
                cur.append((u*u*p0[0] + 2*u*t*p1[0] + t*t*p2[0],
                            u*u*p0[1] + 2*u*t*p1[1] + t*t*p2[1]))
            x, y = p2
            last_q = (p1, p2)
            last_c = None
        elif c == "Z":
            if cur:
                subpaths.append(cur)
            cur = [(sx, sy)]
            x, y = sx, sy
            last_c = last_q = None
    if cur:
        subpaths.append(cur)
    return subpaths


def render(xml_path, size, color):
    xml = xml_path.read_text()
    vp = float(re.search(r'android:viewportWidth="([\d.]+)"', xml).group(1))
    vh = float(re.search(r'android:viewportHeight="([\d.]+)"', xml).group(1))
    scale = size / vp
    ys = size / vh
    ss = int(size / vp)
    big = size * 4
    out = Image.new("L", (big, big), 0)
    for path in re.findall(r'android:pathData="([^"]+)"', xml):
        for sub in flatten(parse_path(path)):
            if len(sub) < 3:
                continue
            layer = Image.new("L", (big, big), 0)
            d = ImageDraw.Draw(layer)
            d.polygon([(px * scale * 4, py * ys * 4) for px, py in sub], fill=255)
            out = ImageChops.difference(out, layer)  # even-odd fill
    out = out.resize((size, size), Image.LANCZOS)
    img = Image.new("RGBA", (size, size), (0, 0, 0, 0))
    solid = Image.new("RGBA", (size, size), color)
    img.paste(solid, (0, 0), out)
    return img


def main():
    DST.mkdir(parents=True, exist_ok=True)
    # NavigationView item icons use colorControlNormal; selected items are
    # tinted primary.  Two sets let the C++ side switch with the app theme.
    palettes = {
        "": ((0x60, 0x6a, 0x60, 255), (0x42, 0x9e, 0x46, 255)),
        "_dark": ((0x9a, 0xa6, 0x9a, 255), (0x7c, 0xc0, 0x7f, 255)),
    }
    for name, src in ICONS.items():
        xml = SRC / (src + ".xml")
        if not xml.exists():
            continue
        for suffix, (normal, selected) in palettes.items():
            render(xml, 48, normal).save(DST / (name + suffix + ".png"))
            render(xml, 48, selected).save(DST / (name + suffix + "_sel.png"))
    print("generated", len(list(DST.glob("*.png"))), "nav icons in", DST)


if __name__ == "__main__":
    main()

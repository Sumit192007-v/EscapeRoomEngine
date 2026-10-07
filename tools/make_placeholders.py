"""Generates simple placeholder PNGs for every image named in levels/*.txt.
Run from the project root:   python tools/make_placeholders.py
Needs:  pip install pillow
Existing files are NOT overwritten, so you can replace placeholders with real art
(keep the same file name) and re-run this safely any time.
"""
import os, re, hashlib
from PIL import Image, ImageDraw, ImageFont

ROOT = os.path.dirname(os.path.dirname(os.path.abspath(__file__)))

def color_for(name):
    h = hashlib.md5(name.encode()).digest()
    return (60 + h[0] % 120, 60 + h[1] % 120, 60 + h[2] % 120)

def font(size):
    for p in (os.path.join(ROOT, "assets/fonts/font.ttf"), "C:/Windows/Fonts/arial.ttf"):
        if os.path.exists(p):
            return ImageFont.truetype(p, size)
    return ImageFont.load_default()

def make(path, w, h, label, bg=False):
    full = os.path.join(ROOT, path)
    if os.path.exists(full):
        return
    os.makedirs(os.path.dirname(full), exist_ok=True)
    c = color_for(path)
    img = Image.new("RGBA", (w, h), c + (255,))
    d = ImageDraw.Draw(img)
    if bg:  # simple vertical gradient + floor line
        for y in range(h):
            k = y / h
            d.line([(0, y), (w, y)], fill=(int(c[0] * (1 - 0.5 * k)), int(c[1] * (1 - 0.5 * k)), int(c[2] * (1 - 0.5 * k)), 255))
        d.rectangle([0, int(h * 0.78), w, h], fill=(25, 22, 20, 255))
    else:
        d.rectangle([0, 0, w - 1, h - 1], outline=(255, 255, 255, 255), width=3)
    size = 40 if bg else max(12, min(24, w // 8))
    f = font(size)
    box = d.textbbox((0, 0), label, font=f)
    d.text(((w - (box[2] - box[0])) // 2, (h - (box[3] - box[1])) // 2), label, fill=(255, 255, 255, 255), font=f)
    img.save(full)
    print("created", path)

def parse(fn):
    secs, cur = [], None
    for line in open(fn, encoding="utf-8"):
        line = line.strip()
        if not line or line.startswith("#"):
            continue
        m = re.match(r"\[(.+)\]", line)
        if m:
            cur = {"_s": m.group(1).lower()}
            secs.append(cur)
        elif "=" in line and cur is not None:
            k, v = line.split("=", 1)
            cur[k.strip()] = v.strip()
    return secs

make("assets/images/menu_bg.png", 1280, 720, "MENU BACKGROUND", bg=True)
for f in sorted(os.listdir(os.path.join(ROOT, "levels"))):
    if not f.startswith("level") or f == "level_list.txt":
        continue
    for s in parse(os.path.join(ROOT, "levels", f)):
        if s["_s"] == "level" and s.get("background"):
            make(s["background"], 1280, 720, s.get("name", "Room"), bg=True)
        elif s["_s"] == "object":
            w, h = int(s["w"]), int(s["h"])
            if s.get("image"):
                make(s["image"], w, h, s.get("name", s["id"]))
            if s.get("inspect_image"):
                make(s["inspect_image"], 240, 180, s.get("name", s["id"]))
            if s.get("reward_image"):
                make(s["reward_image"], 70, 70, s.get("reward_name", "item"))

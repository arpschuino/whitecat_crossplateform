"""Génère gel_list_designer_order.txt : toutes les marques reordonnées par teinte.
- Lee     : familles nommées selon leefilters.com
- GamColor: ordre numérique = ordre colorimétrique (identique au fichier numérique)
- Rosco   : ordre du site emea.rosco.com/fr/products/catalog/roscolux (14 pages)
- Apollo  : tri par teinte (hue calculée depuis RGB)
"""

import re, colorsys

SRC  = r"whitecatbuild\build\white_cat_for_mingw\ressources\gel_list_num_order.txt"
DEST = r"whitecatbuild\build\white_cat_for_mingw\ressources\gel_list_designer_order.txt"

# ── Familles Lee (ordre leefilters.com) ─────────────────────────────────────
# 0=Rose&Pink  1=Lavender&Purple  2=Blue  3=Green
# 4=Yellow&Straw  5=Amber&Orange  6=Red
# 7=CT&Technical  8=Diffusion&Frost  9=White&Neutral  10=Other

LEE_FAMILLE_NAMES = [
    "Rose_&_Pink", "Lavender_&_Purple", "Blue", "Green",
    "Yellow_&_Straw", "Amber_&_Orange", "Red",
    "CT_&_Technical", "Diffusion_&_Frost", "White_&_Neutral", "Other"
]

LEE_FAMILLE_MAP = {
    # Rose & Pink
    2:0, 35:0, 36:0, 39:0, 46:0, 107:0, 108:0, 109:0, 110:0, 111:0,
    113:0, 127:0, 128:0, 148:0, 153:0, 154:0, 157:0, 182:0, 192:0,
    193:0, 194:0, 328:0, 332:0, 345:0, 506:0, 748:0, 779:0, 790:0,
    793:0, 794:0, 795:0, 798:0,
    # Lavender & Purple
    3:1, 48:1, 49:1, 52:1, 53:1, 58:1, 126:1, 136:1, 137:1, 142:1,
    169:1, 170:1, 180:1, 181:1, 199:1, 341:1, 343:1, 344:1, 700:1, 701:1,
    702:1, 703:1, 704:1, 706:1, 707:1, 708:1, 709:1, 797:1, 799:1,
    # Blue
    61:2, 63:2, 68:2, 71:2, 75:2, 79:2, 85:2, 115:2, 116:2, 117:2,
    118:2, 119:2, 120:2, 131:2, 132:2, 140:2, 141:2, 143:2, 144:2,
    161:2, 165:2, 172:2, 174:2, 183:2, 191:2, 195:2, 196:2, 197:2, 198:2,
    352:2, 353:2, 354:2, 363:2, 366:2, 500:2, 501:2, 502:2, 503:2,
    508:2, 525:2, 599:2, 710:2, 711:2, 712:2, 713:2, 714:2, 715:2, 716:2,
    719:2, 721:2, 722:2, 723:2, 724:2, 725:2, 727:2, 729:2,
    # Green
    88:3, 89:3, 90:3, 121:3, 122:3, 124:3, 138:3, 139:3, 189:3, 190:3,
    322:3, 323:3, 325:3, 327:3, 504:3, 505:3, 728:3, 730:3, 731:3,
    733:3, 735:3, 736:3, 738:3, 740:3,
    # Yellow & Straw
    7:4, 10:4, 13:4, 15:4, 100:4, 101:4, 103:4, 159:4, 741:4,
    763:4, 764:4, 765:4, 767:4, 768:4, 770:4,
    # Amber & Orange
    4:5, 9:5, 17:5, 20:5, 21:5, 22:5, 102:5, 104:5, 105:5, 134:5,
    135:5, 147:5, 151:5, 152:5, 156:5, 158:5, 162:5, 176:5, 179:5,
    184:5, 185:5, 186:5, 187:5, 188:5, 507:5, 550:5, 742:5, 746:5,
    773:5, 774:5, 775:5, 776:5, 777:5, 778:5, 780:5,
    # Red
    8:6, 19:6, 24:6, 25:6, 26:6, 27:6, 29:6, 106:6, 164:6, 166:6,
    781:6, 787:6, 789:6,
    # CT & Technical
    200:7, 201:7, 202:7, 203:7, 204:7, 205:7, 206:7, 207:7, 208:7,
    209:7, 210:7, 211:7, 212:7, 213:7, 218:7, 219:7, 223:7, 226:7,
    230:7, 232:7, 236:7, 237:7, 238:7, 241:7, 242:7, 243:7, 244:7,
    245:7, 246:7, 247:7, 248:7, 249:7, 278:7, 279:7, 281:7, 283:7,
    285:7, 286:7, 287:7, 298:7, 299:7, 441:7, 442:7, 443:7, 444:7, 269:7,
    # Diffusion & Frost
    129:8, 216:8, 217:8, 220:8, 221:8, 224:8, 225:8, 228:8, 250:8,
    251:8, 252:8, 253:8, 254:8, 255:8, 256:8, 257:8, 258:8, 400:8,
    402:8, 404:8, 410:8, 414:8, 416:8, 420:8, 450:8, 452:8, 705:8,
    717:8, 718:8, 720:8, 749:8, 750:8, 791:8,
    # White & Neutral
    130:9, 271:9, 600:9, 601:9, 602:9, 603:9, 744:9, 747:9,
}

# ── Ordre Rosco (14 pages de emea.rosco.com/fr/products/catalog/roscolux) ───
ROSCO_ORDER = [
    # Pages 1-4 — CT / CalColor / Storaro / Technical
    3410, 3409, 3408, 3411, 3407, 3444, 3443, 3442, 3441, 3310, 3152, 3405, 3406, 3150,
    4515, 4530, 4560, 4590, 2002, 2003,
    4615, 4630, 4660, 4690, 2001, 4815, 4830, 4860, 4890,
    3318, 3314, 3313, 3308, 3309, 4715, 4730, 4760, 4790, 4915, 4930,
    4960, 4990, 3216, 3208, 3206, 3204, 3203, 3202, 3220,
    4215, 4230, 4260, 4290, 2006, 2007, 2008, 2009, 4307, 4315, 4330,
    4360, 4390, 2005, 3317, 3316, 3315, 3304, 4415, 4430, 4460, 4490, 2004,
    # Pages 4-12 — Couleurs (Bastard Amber → Green)
    0, 1, 2, 302, 3, 303, 4, 304,
    5, 305, 6, 7, 8, 9, 10, 310, 11, 12, 312, 13, 313, 14, 15, 16, 316, 17, 317, 18,
    318, 19, 20, 21, 321, 22, 23, 24, 324, 25, 325, 26, 27,
    30, 31, 331, 32, 332, 33, 333, 34, 35, 36, 336, 37, 337, 38, 339, 39,
    40, 41, 42, 342, 43, 343, 44, 344, 45, 46, 346,
    47, 347, 48, 348, 49, 349, 50, 51, 351, 52, 53, 353, 54, 55, 355,
    56, 356, 57, 357, 58, 358, 59, 359,
    60, 360, 61, 361, 62, 362, 63, 363, 64, 364, 65, 365, 66, 366, 67, 367, 68,
    368, 69, 369, 70, 370, 71, 371, 72, 372, 73, 373, 74, 374, 75, 375,
    76, 376, 77, 377, 78, 378, 79, 80, 81, 381, 82, 382, 83, 383, 84, 384, 85, 385,
    86, 386, 87, 88, 388, 89, 389, 90, 91, 92, 392, 93, 393, 94, 95, 395, 96,
    # Pages 12-14 — Gris / Diffusions / UV
    97, 397, 98, 398, 99, 100, 101, 102, 103, 104, 105, 106, 111, 112, 113, 114,
    115, 116, 117, 118, 119, 120, 121, 122, 124, 125, 126, 127, 132, 140,
    160, 162, 163, 3114,
]
ROSCO_INDEX = {ref: i for i, ref in enumerate(ROSCO_ORDER)}

# ── Utilitaires RGB → hue ────────────────────────────────────────────────────

def rgb_to_hue(r, g, b):
    """Retourne (is_neutral, hue_deg) pour trier par teinte.
    Les neutres (faible saturation) sont mis en fin."""
    if r + g + b == 0:
        return (1, 0)
    h, s, v = colorsys.rgb_to_hsv(r/255, g/255, b/255)
    if s < 0.15:   # quasi-neutre : blanc, gris, diffusion
        return (1, h * 360)
    return (0, h * 360)

# ── Lecture du fichier source ────────────────────────────────────────────────

brands = {"Lee": [], "Rosco": [], "GamColor": [], "Apollo": []}

with open(SRC, encoding="latin-1") as f:
    for line in f:
        line = re.sub(r'\t+', '\t', line.rstrip("\n\r")).strip()
        parts = line.split("\t")
        if len(parts) < 6:
            continue
        brand = parts[0]
        if brand not in brands:
            continue
        try:
            ref = int(parts[1])
            # Certaines lignes ont "240 75.00" dans un seul champ (espace au lieu de tab)
            # On extrait le premier entier de chaque champ RGB
            r = int(re.search(r'\d+', parts[3]).group())
            g = int(re.search(r'\d+', parts[4]).group())
            b = int(re.search(r'\d+', parts[5]).group())
        except (ValueError, IndexError, AttributeError):
            continue
        brands[brand].append((ref, r, g, b, line))

# ── Écriture du fichier designer ─────────────────────────────────────────────

BRAND_ORDER = ["Lee", "Rosco", "GamColor", "Apollo"]

with open(DEST, "w", encoding="latin-1") as out:
    for brand in BRAND_ORDER:
        entries = brands[brand]
        if brand == "Lee":
            # Tri par famille nommée puis par ref dans la famille
            lee_buckets = {i: [] for i in range(len(LEE_FAMILLE_NAMES))}
            for ref, r, g, b, line in entries:
                fam = LEE_FAMILLE_MAP.get(ref, 10)
                lee_buckets[fam].append((ref, line))
            for fam_idx in range(len(LEE_FAMILLE_NAMES)):
                for _, line in sorted(lee_buckets[fam_idx]):
                    out.write(line + "\n")
        elif brand == "GamColor":
            # Ordre numérique = ordre colorimétrique pour GamColor
            for _, _, _, _, line in sorted(entries, key=lambda e: e[0]):
                out.write(line + "\n")
        elif brand == "Rosco":
            # Tri selon l'ordre du site Rosco (14 pages)
            for _, _, _, _, line in sorted(entries, key=lambda e: ROSCO_INDEX.get(e[0], 99999 + e[0])):
                out.write(line + "\n")
        else:
            # Tri par teinte calculée depuis RGB (Apollo)
            for _, _, _, _, line in sorted(entries, key=lambda e: rgb_to_hue(e[1], e[2], e[3])):
                out.write(line + "\n")

# ── Rapport ──────────────────────────────────────────────────────────────────
print(f"Fichier généré : {DEST}")
for brand in BRAND_ORDER:
    print(f"  {brand}: {len(brands[brand])} filtres")

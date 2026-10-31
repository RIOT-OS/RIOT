#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Christopher Büchse
# SPDX-License-Identifier: LGPL-2.1-only

"""
Generate the printer profiles of the Epson ESC/POS driver.

The profiles in contrib/*.yml are copies of the profiles of the
escpos-printer-db (https://github.com/receipt-print-hq/escpos-printer-db),
which is also used by python-escpos. This script resolves the inheritance of
the profiles in the same way as the database does and writes them to
generated/epson_escpos_profiles.c and generated/epson_escpos_profiles.h.

Usage: gen_profiles.py
"""

import datetime
import glob
import io
import os
import re
import sys

import yaml

BASEDIR = os.path.dirname(os.path.abspath(__file__))
PROFILE_DIR = os.path.join(BASEDIR, "contrib")
OUT_DIR = os.path.join(BASEDIR, "generated")
OUT_C = os.path.join(OUT_DIR, "epson_escpos_profiles.c")
OUT_H = os.path.join(OUT_DIR, "epson_escpos_profiles.h")

# base profile that defines all features, only used for inheritance
BASE_PROFILE = "default"

WARNING = """\
/* WARNING: This file has been auto-generated from the YAML files in
 *          contrib/. Do not edit this by hand, but update the profiles.
 *          Finally, run `./gen_profiles.py` in drivers/epson_escpos. */
"""

COPYRIGHT = " * SPDX-FileCopyrightText: {year} RIOT Contributors\n"
COPYRIGHT_RE = re.compile(r"^ \* SPDX-FileCopyrightText: (\d+) RIOT "
                          r"Contributors$", re.MULTILINE)
COPYRIGHT_MIN_YEAR = 2026

LICENSE = ("/*\n"
           " * SPDX-FileCopyrightText: escpos-printer-db contributors\n"
           + COPYRIGHT.format(year=datetime.date.today().year) +
           " * SPDX-License-Identifier: CC-BY-4.0\n"
           " */\n")


def load_profiles():
    """Load all profiles as dict of profile key -> raw profile."""
    profiles = {}
    for path in sorted(glob.glob(os.path.join(PROFILE_DIR, "*.yml"))):
        with open(path, encoding="utf-8") as f:
            profiles.update(yaml.safe_load(f))
    return profiles


def resolve(profiles, key):
    """
    Resolve the inheritance of a profile.

    Same rules as scripts/collate.py of escpos-printer-db: keys of a profile
    override the keys of the inherited profile, only "features" is merged
    item by item and "media" is merged if it has no "width".
    """
    chain = [profiles[key]]
    while "inherits" in chain[-1]:
        parent = chain[-1]["inherits"]
        assert parent in profiles, f"{key}: unknown profile {parent}"
        assert profiles[parent] not in chain, f"{key}: circular inheritance"
        chain.append(profiles[parent])
    chain.reverse()

    profile = {k: v for p in chain for k, v in p.items()}
    profile["features"] = {k: v for p in chain
                           for k, v in p.get("features", {}).items()}
    if "width" not in profile["media"]:
        profile["media"] = {k: v for p in chain
                            for k, v in p.get("media", {}).items()}
    profile.pop("inherits", None)
    return profile


def c_ident(name):
    """Convert a profile key to a C identifier, e.g. TM-T88IV -> tm_t88iv."""
    return re.sub(r"[^a-z0-9]+", "_", name.lower()).strip("_")


def feature_macro(feature):
    """Convert a feature name, e.g. paperFullCut -> ..._PAPER_FULL_CUT."""
    return "EPSON_ESCPOS_FEATURE_" + re.sub(r"(?<!^)(?=[A-Z])", "_",
                                            feature).upper()


def number(value):
    """Return the value or 0 if it is unknown."""
    return value if isinstance(value, int) else 0


def write_header(f, features, profiles):
    f.write(LICENSE)
    f.write("\n")
    f.write("#pragma once\n\n")
    f.write(WARNING)
    f.write("""
/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Printer profiles of the Epson ESC/POS driver
 *
 * Generated from the
 * [ESC/POS printer database](https://github.com/receipt-print-hq/escpos-printer-db).
 */

#include "epson_escpos.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Features of a printer profile
 * @{
 */
""")
    width = max(len(feature_macro(feature)) for feature in features)
    for i, feature in enumerate(features):
        value = f"(1U << {i})"
        f.write(f"#define {feature_macro(feature):<{width}} {value:<10} "
                f"/**< {feature} */\n")
    f.write("/** @} */\n")
    for key, profile in profiles.items():
        f.write(f"""
/**
 * @brief   Profile of the {profile['vendor']} {profile['name']}
 */
extern const epson_escpos_profile_t epson_escpos_profile_{c_ident(key)};
""")
    f.write("""
#ifdef __cplusplus
}
#endif

/** @} */
""")


def write_source(f, features, profiles):
    f.write(LICENSE)
    f.write("\n")
    f.write(WARNING)
    f.write("""
/**
 * @ingroup     drivers_epson_escpos
 * @{
 *
 * @file
 * @brief       Printer profiles of the Epson ESC/POS driver
 *
 * @}
 */

#include "container.h"
#include "epson_escpos_profiles.h"
""")
    for key, profile in profiles.items():
        ident = c_ident(key)
        fonts = profile["fonts"]
        columns = [fonts[k]["columns"] for k in sorted(fonts, key=int)]
        pages = profile["codePages"]
        active = [feature_macro(k) for k in features
                  if profile["features"].get(k)]
        width = profile["media"]["width"]

        f.write(f"\nstatic const uint8_t _{ident}_columns[] = {{ "
                f"{', '.join(str(c) for c in columns)} }};\n")
        f.write(f"\nstatic const epson_escpos_codepage_t _{ident}_codepages[] = {{\n")
        for page in sorted(pages, key=int):
            enc = pages[page]
            enc = "NULL" if enc == "Unknown" else f'"{enc}"'
            f.write(f"    {{ {page}, {enc} }},\n")
        f.write("};\n")
        f.write(f"""
const epson_escpos_profile_t epson_escpos_profile_{ident} = {{
    .name = "{profile["name"]}",
    .vendor = "{profile["vendor"]}",
    .features = {(" |" + chr(10) + "                ").join(active) or "0"},
    .num_fonts = ARRAY_SIZE(_{ident}_columns),
    .columns = _{ident}_columns,
    .width_mm = {number(width.get("mm"))},
    .width_px = {number(width.get("pixels"))},
    .dpi = {number(profile["media"].get("dpi"))},
    .num_codepages = ARRAY_SIZE(_{ident}_codepages),
    .codepages = _{ident}_codepages,
}};
""")


def normalize(content):
    """
    Replace the copyright year by a placeholder.

    Returns None if the copyright is missing or the year is before 2026.
    """
    match = COPYRIGHT_RE.search(content)
    if not match or int(match.group(1)) < COPYRIGHT_MIN_YEAR:
        return None
    return COPYRIGHT_RE.sub(COPYRIGHT.format(year="YEAR").rstrip("\n"),
                            content, count=1)


def check(path, generated):
    """Check if the file at path matches the generated content."""
    try:
        with open(path, encoding="utf-8") as f:
            existing = f.read()
    except FileNotFoundError:
        existing = ""
    if normalize(existing) is None or \
            normalize(existing) != normalize(generated):
        print(f"{os.path.relpath(path, BASEDIR)} is outdated, run "
              f"`./gen_profiles.py` in drivers/epson_escpos", file=sys.stderr)
        return False
    return True


def main():
    raw = load_profiles()
    features = list(raw[BASE_PROFILE]["features"])
    profiles = {key: resolve(raw, key) for key in sorted(raw)
                if key != BASE_PROFILE}
    assert len(features) <= 16, "too many features for uint16_t"
    for key, profile in profiles.items():
        unknown = set(profile["features"]) - set(features)
        assert not unknown, f"{key}: features {unknown} not in {BASE_PROFILE}"

    outputs = {OUT_H: write_header, OUT_C: write_source}
    ok = True
    for path, write in outputs.items():
        buf = io.StringIO()
        write(buf, features, profiles)
        with open(path, "w", encoding="utf-8") as f:
            f.write(buf.getvalue())
    sys.exit(0 if ok else 1)


if __name__ == "__main__":
    main()

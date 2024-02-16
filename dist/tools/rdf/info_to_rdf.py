#!/usr/bin/env python3

"""For all boards, use make to extract from tests/minimal/ the
info-features-provided and other actually application independent details, and
store them in the single argument outfile as an RDF Turtle file"""

import sys
import json
from pathlib import Path
import subprocess
import argparse

import rdflib

def parse_linkersize(size: str) -> int:
    minuend, minus, subtrahend = size.partition('-')
    if minus:
        return parse_linkersize(minuend) - parse_linkersize(subtrahend)

    factor = 1
    if size.endswith("K"):
        factor *= 1024
        size = size[:-1]
    if size.endswith("M"):
        factor *= 1024 * 1024
        size = size[:-1]
    if size.startswith("0x"):
        return int(size[2:], 16) * factor
    else:
        return int(size) * factor

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("outfile", type=Path)
p.add_argument("-v", "--verbose", action="store_true")
args = p.parse_args()

def log(msg: str):
    if args.verbose:
        print(msg, file=sys.stderr)

# We may offer building with a different ns_main later to compare statements
# about different versions of RIOT in a single graph, but for the time being,
# the assumption that what we are working with is the main branch is good
# enough.
ns_main = "https://ns.riot-os.org/by-version/main/"
ns = dict(
        feature=rdflib.Namespace(ns_main + "feature/"),
        board=rdflib.Namespace(ns_main + "board/"),
        cpu=rdflib.Namespace(ns_main + "cpu/"),

        terms=rdflib.Namespace(ns_main + "terms/"),
        )

g = rdflib.Graph()
for (k, v) in ns.items():
    g.bind(k, v)


log("Obtaining list of boards...")
boards = subprocess.check_output([
    "make",
    "--quiet",
    # If you are ever tempted to remove this, try how this runs from `make -C
    # RIOT doc`.
    "--no-print-directory",
    "-C", "tests/minimal/",
    "info-boards"]
    ).decode('ascii').strip().split()

cpus = set()

for board in boards:
    log(f"Processing board {board}")
    # Once we also extract more high-level metadata from the documentation, we
    # may want to revisit the decision where to make this statement.
    g.add((ns['board'][board], rdflib.RDF.type, ns['terms']['Board']))

    infobuildjson = subprocess.check_output([
        "make",
        "--quiet",
        # If you are ever tempted to remove this, try how this runs from `make
        # -C RIOT doc`.
        "--no-print-directory",
        f"BOARD={board}",
        "-C", "tests/minimal/",
        "info-build-json"])

    infobuild = json.loads(infobuildjson)

    for f in infobuild['FEATURES_PROVIDED']:
        g.add((ns['board'][board], ns['terms']['providesFeature'], ns['feature'][f]))

    g.add((ns['board'][board], ns['terms']['cpu'], ns['cpu'][infobuild['CPU']]))
    cpus |= {infobuild['CPU']}

    linkflags = infobuild['LINKFLAGS']

    # The two styles cover the most common cases; ESP, LPC and MSP chips don't have things easily accessible.
    romlength = [parse_linkersize(f.removeprefix('-Wl,--defsym=_rom_length=')) for f in linkflags if f.startswith('-Wl,--defsym=_rom_length=')] + \
        [parse_linkersize(f.removeprefix('-Wl,--defsym=__TEXT_REGION_LENGTH__=')) for f in linkflags if f.startswith('-Wl,--defsym=__TEXT_REGION_LENGTH__=')]
    ramlength = [parse_linkersize(f.removeprefix('-Wl,--defsym=_ram_length=')) for f in linkflags if f.startswith('-Wl,--defsym=_ram_length=')] + \
        [parse_linkersize(f.removeprefix('-Wl,--defsym=__DATA_REGION_LENGTH__=')) for f in linkflags if f.startswith('-Wl,--defsym=__DATA_REGION_LENGTH__=')]

    if romlength:
        romlength, = romlength
        g.add((ns['board'][board], ns['terms']['rom'], rdflib.Literal(romlength)))
    if ramlength:
        ramlength, = ramlength
        g.add((ns['board'][board], ns['terms']['ram'], rdflib.Literal(ramlength)))

    g.add((ns['board'][board], ns['terms']['identifier'], rdflib.Literal(board)))

for cpu in cpus:
    g.add((ns['cpu'][cpu], rdflib.RDF.type, ns['terms']['CPU']))

    g.add((ns['cpu'][cpu], ns['terms']['identifier'], rdflib.Literal(cpu)))

with open(args.outfile, "wb") as outfile:
    outfile.write(b"# This file is generated by `make doc`\n\n")
    g.serialize(outfile, format="turtle", encoding="utf8")

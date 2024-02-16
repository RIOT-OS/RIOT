#!/usr/bin/env python3

"""For all boards and CPUs, based on their names as pre-existing in
doc/rdf/info.ttl, parse their likely Doxygen locations for additional
metadata"""

import re
import argparse
from pathlib import Path

import rdflib

# We may offer building with a different ns_main later to compare statements
# about different versions of RIOT in a single graph, but for the time being,
# the assumption that what we are working with is the main branch is good
# enough.
ns_main = "https://ns.riot-os.org/by-version/main/"
ns = dict(
        terms=rdflib.Namespace(ns_main + "terms/"),
        )

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("outfile", type=Path)
p.add_argument("-v", "--verbose", action="store_true")
args = p.parse_args()

basedata = rdflib.Graph()
basedata.parse("doc/rdf/info.ttl")

outdata = rdflib.Graph()

boards = basedata.subjects(rdflib.RDF.type, ns['terms']['Board'])
cpus = basedata.subjects(rdflib.RDF.type, ns['terms']['CPU'])

# Adjusted from web site _tools/fetch_riot_data.py
board_defgroup = re.compile(r'^.*@defgroup\s+boards_([a-zA-Z0-9_.-]+)\s+(.*)$', flags=re.MULTILINE)
cpu_defgroup = re.compile(r'^.*@defgroup\s+cpu_([a-zA-Z0-9_.-]+)\s+(.*)$', flags=re.MULTILINE)

for board_node in boards:
    board = basedata.value(board_node, ns['terms']['identifier'], any=False).value

    docfile = [Path('boards') / board / 'doc.txt', Path('boards') / board / 'include' / 'board.h']
    docdata = "\n".join(f.open().read() for f in docfile if f.exists())
    if board.startswith('slwstk6000b-'):
        # FIXME see if there's more undocumented
        continue
    found = board_defgroup.findall(docdata)
    if not found:
        raise ValueError(f"Board {board} present but not documented")
    # Ensure there is only one (because we don't yet insist that names match, see below)
    (doxygenname, doxygenlabel), = found
    if doxygenname != board:
        #if doxygenname.replace(('-', '_'), '').lower() == board.replace(('-', '_'), '').lower() or board.startswith('esp32-ethernet-kit') or board == 'esp32-olimex-evb' or board == 'esp32-wrover-kit':
        # FIXME: Should we just make them consistent? Probably easier after the RDM
        pass
        # else:
        #     raise ValueError(f"Board {board} is called {doxygenname} in Doxygen")

    doxygenlabel = doxygenlabel.removesuffix(" board")

    outdata.add((board_node, rdflib.RDFS.label, rdflib.Literal(doxygenlabel, lang="en")))
    outdata.add((board_node, ns['terms']['doc'], rdflib.URIRef(f"https://doc.riot-os.org/group__boards__{doxygenname}.html")))


for cpu_node in cpus:
    cpu = basedata.value(cpu_node, ns['terms']['identifier'], any=False).value

    docfile = [Path('cpu') / cpu / 'doc.txt', Path('cpu') / cpu / 'include' / 'cpu.h', Path('cpu') / cpu / 'include' / 'cpu_conf.h']
    docdata = "\n".join(f.open().read() for f in docfile if f.exists())

    found = cpu_defgroup.findall(docdata)
    matching = [(doxygenname, doxygenlabel) for (doxygenname, doxygenlabel) in found if doxygenname == cpu]
    if len(matching) != 1:
        raise ValueError(f"For CPU {cpu}, found doxygen matches {found}")
    (doxygenname, doxygenlabel), = matching

    outdata.add((cpu_node, rdflib.RDFS.label, rdflib.Literal(doxygenlabel, lang="en")))
    outdata.add((cpu_node, ns['terms']['doc'], rdflib.URIRef(f"https://doc.riot-os.org/group__cpu__{doxygenname}.html")))

with open(args.outfile, "wb") as outfile:
    outfile.write(b"# This file is generated by `make doc`\n\n")
    outdata.serialize(outfile, format="turtle", encoding="utf8")

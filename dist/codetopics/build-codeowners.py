#!/usr/bin/env python3

"""Build a CODEOWNERS file from RDF inputs. It expands any
ont:glob on a skos:Concept that has an ont:codeowner with a
foaf:account on GitHub to the glob notifying the user."""

# It might be possible to express this all in SPARQL

import argparse
from pathlib import Path
import sys

import rdflib
import rdflib.plugins.sparql

p = argparse.ArgumentParser(description=__doc__)
p.add_argument("outfile", help="File with CODEOWNERS syntax to (over)write", type=Path)
p.add_argument("infile", nargs="+", help="Input RDF files", type=Path)

args = p.parse_args()

g = rdflib.Graph()
for f in args.infile:
    g.load(f.open(), format='turtle' if f.suffix == '.ttl' else 'xml')

# Tricky part might be to salvage the ordering of the globs

q = '''
PREFIX foaf: <http://xmlns.com/foaf/0.1/>
PREFIX ont: <file:///home/chrysn/git/RIOT/dist/codetopics/ontology.ttl#>

SELECT ?glob ?username
WHERE {
    ?topic ont:glob ?glob .
    ?topic ont:codeowner ?user .
    ?user foaf:account [ foaf:accountServiceHomepage <https://github.com/>; foaf:accountName ?username ] .
}
'''

with args.outfile.open('w') as outfile:
    for line in g.query(q):
        print(line, file=outfile)

# ... and we have to aggregate the globs to notify the right set of people --
# but do we want that? the trailing Kconfig line "steals" kconfig changes form
# subsys maintainers? Do we *want* that stealing, or should we algorithmically
# recombine these globs to produce a set of non-stealing globs where if A
# subscribes to /foo/bar/ and B subscribes to Kconfig, lines are the
# individuals, but then (after both) a `/foo/bar/**/Kconfig A B`?

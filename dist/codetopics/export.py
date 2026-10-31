#!/usr/bin/env python3

"""Just a quick hack to get a .ttl file with sane prefixes for tools like
https://skos-play.sparna.fr/skos-testing-tool/,
https://skos-play.sparna.fr/play/ or
http://owlgred.lumii.lv/online_visualization"""

import rdflib
import glob

g = rdflib.Graph()
for f in glob.glob('*.ttl'):
    g.load(open(f), format="turtle")

with open('/home/chrysn/Downloads/all.ttl', 'wb') as outfile:
    g.serialize(outfile, format='turtle')

#!/usr/bin/env python3
#
import argparse
import os
import re
import sys
import textwrap
import logging
import kconfiglib
import networkx as nx
from networkx.drawing.nx_agraph import write_dot
from networkx.drawing.nx_agraph import to_agraph



MODULE_PREFIXES = ['MOD']

def add_edges_from_deps(node, deps, graph):
    if isinstance(deps, tuple):
        for d in deps:
            if isinstance(d, kconfiglib.Symbol):
                graph.add_edge(node.item.name, d.name)
            elif isinstance(d, tuple):
                add_edges_from_deps(node, d, graph)
    elif isinstance(deps, kconfiglib.Symbol) and deps.name != 'y':
        graph.add_edge(node.item.name, deps.name)

def main():
    """Entry point"""
    parser = argparse.ArgumentParser(
        description=('Generate a dependency graph from a Kconfig specification.'))
    parser.add_argument('-k', '--kconfig', default='Kconfig',
        help='Kconfig file to read')
    parser.add_argument('-d', '--debug', dest='debug', action='store_true',
        help='Enable debug messages')
    args = parser.parse_args()

    parts = os.path.split(args.kconfig)
    oldwd = os.getcwd()
    os.chdir(parts[0])
    kconfig = kconfiglib.Kconfig(parts[1])
    os.chdir(oldwd)

    # Configure logging
    logLevel = logging.DEBUG if args.debug else logging.INFO
    logging.basicConfig(level=logLevel)

    G = nx.DiGraph()
    modules = []
    for s in kconfig.unique_defined_syms:
        if s.name.split('_')[0] in MODULE_PREFIXES:
            for n in s.nodes:
                add_edges_from_deps(n, n.dep, G)
            modules.append(s)

    A = to_agraph(G)
    A.graph_attr.update(landscape='true',ranksep='4')
    A.layout()
    print(A.to_string())

if __name__=='__main__':
    main()
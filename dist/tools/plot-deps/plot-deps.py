#! /usr/bin/env python

# Copyright (C) 2020 Freie Universitat Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import argparse
import itertools
import os
import re
import subprocess

import matplotlib.pyplot as plt
import networkx as nx


__author__ = "Martine Sophie Lenders"
__email__ = "m.lenders@fu-berlin.de"


DEFAULT_JUMPOFF_POINT = 'examples/hello-world'
MAKE = os.environ.get("MAKE", "make")


def main(jumpoff_point=DEFAULT_JUMPOFF_POINT, outfile=None):
    info_build = parse_info_build(make(jumpoff_point, ["info-build"]))
    deps = parse_deps(info_build)
    DG = nx.DiGraph()
    for modules in deps:
        for dep in deps[modules]:
            # if dependency contains conditionals (A && B) => C
            if len(modules) > 1:
                # permute over all variations of conditionals to mark all
                # parts of the dependencies from all modules that are part of
                # it A =B=> C, B =A=> C
                for ms in itertools.permutations(modules):
                    edge = ms[0], dep
                    cond = sorted(ms[1:])
                    if edge in DG and "cond" in DG[edge] and \
                       DG[edge]["cond"] == cond:
                        # condition is already marked
                        continue
                    DG.add_edge(ms[0], dep, cond=cond, color="green")
            else:
                DG.add_edge(modules[0], dep)
    plt.figure(figsize=(100, 100))
    pos = nx.spring_layout(DG)
    conds = {k: ', '.join(cond)
             for k, cond in nx.get_edge_attributes(DG, "cond").items()}
    colors = [DG[u][v].get('color', 'k') for u, v in DG.edges()]
    nx.draw(DG, pos=pos, with_labels=True, edge_color=colors)
    nx.draw_networkx_edge_labels(DG, pos=pos, edge_labels=conds)
    if outfile is not None:
        plt.savefig(outfile)
    cycles = nx.find_cycle(DG)
    if cycles:
        print("Found a cyclic dependencies:")
        for edge in cycles:
            print(" -", edge[0], "->", edge[1])


def make(directory, targets=None):
    targets = targets or ['all']
    return subprocess.check_output(
        [MAKE, "-C", directory, "--no-print-directory"] + targets
    )


def parse_info_build(info_build):
    current_var = None
    parsing_supp_boards = False
    var_c = re.compile("(?P<name>[0-9A-Z_]+)( \([^\)]+\))?:"
                       "(\s+(?P<value>\S.*))?\s*$")
    value_c = re.compile("\s+(?P<value>\S.*)?\s*$")
    res = {}
    for line in info_build.decode().splitlines():
        if parsing_supp_boards:
            line = line.strip()
            parsing_supp_boards = not line
            res["supported_boards"].append(line)
            continue
        elif line.startswith("supported boards:"):
            res["supported_boards"] = []
            parsing_supp_boards = True
            continue
        elif current_var:
            match = value_c.match(line)
            if match is not None:
                res[current_var].append(match["value"].strip())
                continue
        match = var_c.match(line.strip())
        if match is not None:
            if match["value"] is None:
                current_var = match["name"]
                res[match["name"]] = []
            else:
                res[match["name"]] = re.split("\s+", match["value"].strip())
        else:
            current_var = None
    return res


def parse_deps(info_build, dep_filename=None):
    dep_filename = dep_filename or os.path.join(
        info_build["RIOTBASE"][0],
        "Makefile.dep"
    )
    res = {}
    current_modules = None
    var_pattern = r"(?P<var>\$\((?P<varname>[0-9A-Z_]+)\))"
    include1_c = re.compile(r"^-?include\s+(?P<file>" + var_pattern + r".*)$")
    include2_c = re.compile(r"^-?include\s+\$\((?P<varname>[0-9A-Z_]+):%="
                            r"(?P<filepattern>.*)\)$")
    var_c = re.compile(var_pattern)
    # TODO: drivers, USEPKG
    modules_c = re.compile(r"^ifneq \(\s*,\$\(filter\s+(?P<modules>[^,]+)\s*,"
                           r"\s*\$\(.*USEMODULE\)+$")
    dep_c = re.compile(r"^USE(?P<mode>PKG|MODULE)\s+\+=\s+(?P<dep>\S+)$")
    with open(dep_filename) as dep_file:
        for line in dep_file.readlines():
            line = line.strip()
            if not line or line.startswith('#'):
                # ignore empty lines and comments
                continue
            if current_modules:
                if line == "endif":
                    new_current_modules = []
                    # remove last "and" dependency (reverse enumerate)
                    for i, module in zip(range(len(current_modules) - 1, -1, -1),
                                         reversed(list(current_modules))):
                        module = module[:-1]
                        if module:
                            new_current_modules.append(module)
                    current_modules = new_current_modules
                    continue
                match = modules_c.search(line)
                # add "and" dependency
                if match is not None:
                    for and_module in re.split(r"\s+", match["modules"]):
                        current_modules = [
                            modules + (and_module,)
                            for modules in current_modules
                        ]
                    if len(current_modules[0]) > 2:
                        break
                    continue
                match = dep_c.match(line)
                if match is not None:
                    for module in current_modules:
                        key = tuple(module)
                        value = match["dep"].strip()
                        if match["mode"] == "PKG":
                            value += ":pkg"
                        if key not in res:
                            res[key] = [value]
                        else:
                            res[key].append(value)
                continue
            match = include1_c.match(line)
            if match is not None:
                for value in info_build.get(match["varname"], []):
                    include_file = match["file"].replace(match["var"], value)
                    # first iteration: match still set from include1_c
                    while match:        # replace remaining vars
                        match = var_c.search(include_file)
                        if match is None:
                            continue
                        for value in info_build.get(match["varname"], []):
                            include_file = include_file.replace(match["var"],
                                                                value)
                    if os.path.exists(include_file):
                        deps = parse_deps(info_build, include_file)
                        for module, deps in deps.items():
                            if module not in res:
                                res[module] = deps
                            else:
                                res[module].extend(deps)
                continue
            match = include2_c.search(line)
            if match is not None:
                for value in info_build.get(match["varname"], []):
                    include_file = match["file"].replace('%', value)
                    if os.path.exists(include_file):
                        deps = parse_deps(info_build, include_file)
                        for module, deps in deps.items():
                            res[module].append(deps)
                continue
            match = modules_c.search(line)
            if match is not None:
                current_modules = [(module,) for module
                                   in re.split(r"\s+", match["modules"])]
                continue
            # TODO check more lines with print(dep_filename, line)
    return res


def dir_path(path):
    if os.path.isdir(path):
        return path
    else:
        raise argparse.ArgumentTypeError(
            f'readable_dir:{path} is not a valid path'
        )


if __name__ == '__main__':
    parser = argparse.ArgumentParser()
    parser.add_argument('jumpoff_point', type=dir_path, nargs='?',
                        help='RIOT application to base variables from. '
                             'Default: {}'.format(DEFAULT_JUMPOFF_POINT),
                        default=DEFAULT_JUMPOFF_POINT)
    parser.add_argument('outfile', default=None, nargs='?',
                        help='SVG file to write dependency graph to. '
                             'Default: do not plot to file')
    args = parser.parse_args()
    main(**vars(args))

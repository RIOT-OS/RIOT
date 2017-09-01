#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2017 Freie Universität Berlin
#
# This program is free software: you can redistribute it and/or modify
# it under the terms of the GNU General Public License as published by
# the Free Software Foundation, either version 3 of the License, or
# (at your option) any later version.
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program.  If not, see <http://www.gnu.org/licenses/>.

# Tool for automatically generating interrupt vector tables from CMSIS vendor headers
#
# Author:   Hauke Petersen <hauke.petersen@fu-berlin.de>

import argparse
import sys
import os
import glob
import re
import copy
from operator import itemgetter

cpulist = set()
headermap = dict()

alias = {
    "isr_exti[_\d+]+": "isr_exti"
}

def parse_cpuconf( cpuconf ):
    buf = open(cpuconf, 'r').read()
    path = os.path.dirname(os.path.abspath(cpuconf))

    pat_line = re.compile("(#if|#elif|#ifdef)(( \|\|| \|\| \\\\\n| \\\\\n +\|\|)?"
                          " +(defined\()?CPU_MODEL_[0-9A-Z]+\)?)*"
                          "\n#include \"(.+\.h)\"", re.MULTILINE)
    pat_cpu = re.compile("CPU_MODEL_([0-9A-Z]+)")

    for m in pat_line.finditer(buf):
        header = ("%s/%s" % (path, m.group(5)))

        if header not in headermap:
            headermap[header] = set()

        for c in pat_cpu.finditer(m.group(0)):
            cpu = c.group(1).lower()
            cpulist.add(cpu)
            headermap[header].add(cpu)


def parse_header( file, cpus ):
    filename = os.path.basename(file)
    res = { "cpus": cpus, "vec": {} };
    vec = res["vec"]

    # read through file and find all lines
    with open(file, 'r', encoding = "ISO-8859-1") as f:
         for line in f:
            m = re.match(" +([_0-9A-Za-z]+)_IRQn += (-?\d+),? +(/\*!<|/\*\*<) (.+[a-zA-Z0-9]) +\*/", line)
            if m:
                num = int(m.group(2))
                name = ("isr_%s" % (m.group(1).lower()))
                comment = ("/* [%2i] %s */" % (num, m.group(4)))

                nm = re.match(".+reserved.+", name, re.IGNORECASE)
                if num < 0 or nm:
                    continue

                if num in vec:
                    sys.exit("Error: vector defined twice")
                vec[num] = { "name": name, "comment": comment }

    return res;


def apply_alias( common, specific ):
    for num in common:
        for a in alias:
            m = re.match(a, common[num]["name"])
            if m:
                common[num]["name"] = alias[a]

    for cl in specific:
        for num in cl["vec"]:
            for a in alias:
                m = re.match(a, cl["vec"][num]["name"])
                if m:
                    cl["vec"][num]["name"] = alias[a]


def depstr( cpus, prefix ):
    res = prefix
    i = 0

    # if the list of CPUs contains all the CPUs known, no if-def'ing needed
    if cpulist == cpus:
        return ""

    for cpu in sorted(list(cpus)):
        if (i % 0x2) == 0 and i != 0:
            res += " || \\\n   "
        elif i & 0x1:
            res += " ||"
        res += (" defined(%s)" % (cpu))
        i += 1

    return res + "\n"


def pull_common_vectors( specific ):
    res = dict()

    for i in range(0, 256):
        tmp = None
        match = False

        for cpulist in specific:
            if not i in cpulist["vec"]:
                match = False
                break
            else:
                if not tmp:
                    tmp = cpulist["vec"][i]
                    match = True
                if cpulist["vec"][i] != tmp:
                    match = False
                    break

        if match:
            res[i] = copy.copy(tmp)
            for cpulist in specific:
                del cpulist["vec"][i]

    return res


def generate_table( common, specific ):
    table = "/* CPU specific interrupt vector table */\n"
    table += "ISR_VECTOR(1) const isr_t vector_cpu[CPU_IRQ_NUMOF] = {\n"
    table += "    /* shared vectors for all family members */\n"

    for num in common:
        vec = common[num]
        table += ("    [%2i] = %-26s%s\n" % (num, vec["name"] + ",", vec["comment"]))
        last_num = num

    if len(specific) > 1:
        table += "\n"

    for i, cpulist in enumerate(specific):
        prefix = "#if" if i == 0 else "#elif"
        table += depstr(cpulist["cpus"], prefix)
        for num in cpulist["vec"]:
            vec = cpulist["vec"][num]
            table += ("    [%2i] = %-26s%s\n" % (num, vec["name"] + ",", vec["comment"]))

    if len(specific) > 1:
        table += "#endif\n"
    table += "};\n"
    return table


if __name__ == "__main__":
    # Define some command line args
    p = argparse.ArgumentParser()
    p.add_argument("cpu_conf", help="path to a cpu_conf.h file")
    args = p.parse_args()

    if not os.path.isfile(args.cpu_conf):
        sys.exit("Error: no cpu_conf.h file found")

    # get a list of available header files
    parse_cpuconf(args.cpu_conf)

    # parse each header file
    specific = list();

    for h in headermap.keys():
        specific.append(parse_header(h, headermap[h]))

    # list items by CPU name
    specific = sorted(specific, key=lambda k: sorted(list(k["cpus"]))[0])

    # try to compact vectors to some extend
    common = pull_common_vectors(specific)

    # apply custom aliases
    apply_alias(common, specific)

    # dump vector table
    table = generate_table(common, specific)
    print(table)

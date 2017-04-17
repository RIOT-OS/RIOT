#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2015 Cenk Gündoğan <cnkgndgn@gmail.com>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import random
import json
from subprocess import check_output
import shlex
from itertools import groupby, count

class IOTLABHelper:
    nodesCmd = "experiment-cli info -li"

    def __getLivingNodesFromTestbed(self, site, nodeType):
        nodesStr = check_output(shlex.split(self.nodesCmd), universal_newlines=True)
        nodesJSON = json.loads(nodesStr)['items']
        for siteJSON in nodesJSON:
            if site not in siteJSON:
                continue
            return siteJSON[site][nodeType]['Alive']
        return []

    def __extractNodes(self, inputNodes):
        result = []

        for n in inputNodes.split("+"):
            spl = n.split("-")
            result += list(range(int(spl[0]), int(spl[1]) + 1)) if len(spl) > 1 else [int(n)]

        return sorted(result)

    def __as_range(self, iterable):
        elem = list(iterable)
        return '{0}-{1}'.format(elem[0], elem[-1]) if len(elem) > 1 else '{0}'.format(elem[0])

    def getRandomTestbedNodes(self, sampleSize, site='grenoble', nodeType='m3'):
        livingNodes = self.__getLivingNodesFromTestbed(site, nodeType)
        nodes = self.__extractNodes(livingNodes)

        if sampleSize > len(nodes):
            print("Requested sample size must not exceed the number of distinct nodes")
            return "";

        randomNodes = [ nodes[i] for i in sorted(random.sample(range(len(nodes)), sampleSize)) ]

        return '+'.join(self.__as_range(g) for _, g in \
                        groupby(randomNodes, key = lambda x, c = count(): x - next(c)))

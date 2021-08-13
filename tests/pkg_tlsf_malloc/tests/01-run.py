#!/usr/bin/env python3

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# Tell the lua interpreter running in riot to load some modules and print
# the value of a variable inside that module.

import os
import sys


def test(child):
    # this is the worst test ever.
    child.expect('Overhead: (-?[0-9]+) bytes')

    assert(int(child.match[1]) >= 0)


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(test))

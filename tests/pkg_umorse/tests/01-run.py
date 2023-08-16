#!/usr/bin/env python3

# Copyright (C) 2017 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(u".... . ._.. ._.. ___ / ._. .. ___ _ ___ ...", timeout=30)
    child.expect(u"_ .... .. ... / .. ... / .._ __ ___ ._. ... .", timeout=30)

    child.expect(u".... . ._.. ._.. ___ / ._. .. ___ _ ___ ...", timeout=30)
    child.expect(u"_ .... .. ... / .. ... / .._ __ ___ ._. ... .", timeout=30)

    child.expect_exact("[SUCCESS]", timeout=120)


if __name__ == "__main__":
    sys.exit(run(testfunc))

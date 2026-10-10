#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 HAW Hamburg
# SPDX-License-Identifier: LGPL-2.1-only

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

#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2016 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-FileCopyrightText: 2016 Mathias Tausig <mathias.tausig@fh-campuswien.ac.at>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect('Decoding finished successfully')


if __name__ == "__main__":
    sys.exit(run(testfunc))

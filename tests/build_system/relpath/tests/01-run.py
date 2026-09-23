#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2020 Kaspar Schleiser <kaspar@schleiser.de>
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    # With the default compilation options, the path will be relative to
    # $(RIOTBASE), but with Ccache, this is relative to the application
    # directory. Nonetheless, the path is expected to be relative, so it must
    # not start with a slash.
    child.expect(r"__FILE__: (?!/)\S*main\.c")
    child.expect_exact("[SUCCESS]")


if __name__ == "__main__":
    sys.exit(run(testfunc))

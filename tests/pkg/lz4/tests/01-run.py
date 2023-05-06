#!/usr/bin/env python3

# Copyright (C) 2021 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"Data compressed with success \(ratio: (\d+.\d+)\)\r\n")
    ratio = float(child.match.group(1))
    assert ratio < 1, "No compression (ratio: {})".format(ratio)
    child.expect_exact("Data decompressed with success!")
    child.expect_exact("Validation done, decompressed string:")
    child.expect_exact(
        "Lorem ipsum dolor sit amet, consectetur "
        "adipiscing elit, sed do eiusmod tempor "
        "incididunt ut labore et dolore magna aliqua. "
        "Nisl vel pretium lectus quam id leo. Volutpat "
        "sed cras ornare arcu dui vivamus arcu felis "
        "bibendum. Velit scelerisque in dictum non "
        "consectetur a erat nam. Pretium viverra "
        "suspendisse potenti nullam ac tortor vitae "
        "purus faucibus. Tristique et egestas quis "
        "ipsum suspendisse. At imperdiet dui accumsan "
        "sit amet nulla facilisi. Pulvinar neque "
        "laoreet suspendisse interdum consectetur "
        "libero. Vulputate sapien nec sagittis aliquam "
        "malesuada bibendum arcu vitae. Sed adipiscing "
        "diam donec adipiscing tristique risus nec. "
        "Venenatis tellus in metus vulputate eu "
        "scelerisque. Id faucibus nisl tincidunt eget "
        "nullam non nisi est. Integer feugiat "
        "scelerisque varus morbi enim. Est sit amet "
        "facilisis magna etiam. Venenatis cras sed "
        "felis eget velit aliquet sagittis."
    )


if __name__ == "__main__":
    sys.exit(run(testfunc))

#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2017 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import re
import os
import subprocess
import sys
from testrunner import run


def testfunc(child):
    try:
        child.expect_exact('calling stack corruption function')
        child.expect('.*stack smashing detected.*')
    finally:
        # this tests leaves the samr34-xpro on the IoT-LAB in a broken
        # state. Restore it by flashing the idle firmware and power cycling
        # it. See also https://github.com/iot-lab/iot-lab-gateway/issues/156
        if os.environ.get("BOARD") == "samr34-xpro":
            match = re.match(
                r"samr34-(?P<node>\d*)\.(?P<site>.*)\.iot-lab\.info",
                os.environ.get("IOTLAB_NODE", "")
            )
            if match is not None:
                # return in finally block causes syntax error
                args = ["-l", f"{match['site']},samr34,{match['node']}"]
                if "IOTLAB_EXP_ID" in os.environ:
                    args = ["-i", os.environ["IOTLAB_EXP_ID"]]
                subprocess.run(
                    ["iotlab-node", "--flash-idle"] + args,
                )
                subprocess.run(
                    ["iotlab-node", "--stop"] + args,
                )
                subprocess.run(
                    ["iotlab-node", "--start"] + args,
                )


if __name__ == "__main__":
    sys.exit(run(testfunc))

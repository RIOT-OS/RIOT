#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys

from testrunner import run


USE_ETHOS = int(os.getenv("USE_ETHOS", "1"))
TAP = os.getenv("TAP", "riot0")
BINDIR = os.getenv("BINDIR")


def wait_for_update(child):
    return child.expect([r"riotboot_flashwrite: processing bytes (\d+)-(\d+)",
                         "riotboot_flashwrite: riotboot flashing "
                         "completed successfully"],
                        timeout=10)


def make_notify(client_url, slot, version):
    cmd = [
        "coap-client",
        "-m",
        "post",
        "coap://{}/flashwrite".format(client_url),
        "-f",
        "tests_riotboot_flashwrite-slot{}.{}.riot.bin".format(slot, version),
        "-b",
        "64",
    ]
    subprocess.Popen(cmd, cwd=BINDIR)


def make_reset():
    cmd = [
        'make',
        'reset'
    ]
    assert not subprocess.call(cmd)


def make_riotboot_slots(version):
    cmd = [
        "make",
        "USE_ETHOS={}".format(USE_ETHOS),
        "RIOTBOOT_SKIP_COMPILE=1",
        "APP_VER={}".format(version),
        "riotboot",
    ]
    assert not subprocess.call(cmd)


def testfunc(child):
    """For one board test if specified application is updatable"""

    # Initial Setup and wait for address configuration
    child.expect_exact("main(): This is RIOT!")

    # Get running slot
    child.expect(r"running from slot ([0-1])")
    slot = int(child.match.group(1))
    # get version of currently running image, "Image Version: 0x00000000"
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F:]+)")
    current_app_ver = int(child.match.group("app_ver"), 16)

    if USE_ETHOS == 0:
        # Get device global address
        child.expect(
            r"inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
            "  scope: global  VAL"
        )
        client = "[{}]".format(child.match.group("gladdr").lower())
    else:
        # Get device local address
        client = "[fe80::2%{}]".format(TAP)

    for version in [current_app_ver + 1, current_app_ver + 2]:
        # Wait for nanocoap server to start
        child.expect_exact("Starting nanocoap server instance")
        # Create newer slots bins
        make_riotboot_slots(version)
        # Trigger update process
        make_notify(client, slot ^ 1, version)
        # Wait for update to complete
        while wait_for_update(child) == 0:
            pass
        make_reset()
        # Verify running slot
        child.expect(r"running from slot (\d+)")
        assert slot != int(child.match.group(1)), "TEST FAILED, booted from same slot"
        slot = int(child.match.group(1))

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc))

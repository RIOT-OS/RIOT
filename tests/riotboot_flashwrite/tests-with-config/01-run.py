#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys
import time

from testrunner import run
from testrunner import utils


UPDATING_TIMEOUT = 10

USE_ETHOS = int(os.getenv("USE_ETHOS", "1"))
TAP = os.getenv("TAP", "riot0")
BINDIR = os.getenv("BINDIR")


def wait_for_update(child):
    return child.expect([r"_flashwrite_handler\(\): received data: offset=\d+ "
                         r"len=\d+ blockwise=\d+ more=\d+\r\n",
                         r"_flashwrite_handler\(\): finish\r\n"],
                        timeout=UPDATING_TIMEOUT)


def make_notify(client_url, slot, version):
    cmd = [
        "aiocoap-client",
        "-m",
        "POST",
        "coap://{}/flashwrite".format(client_url),
        "--payload",
        "@tests_riotboot_flashwrite-slot{}.{}.riot.bin".format(slot, version),
        "--payload-initial-szx",
        "2",
    ]
    return subprocess.Popen(cmd, cwd=BINDIR)


def make_riotboot_slots(version):
    cmd = [
        "make",
        "USE_ETHOS={}".format(USE_ETHOS),
        "RIOTBOOT_SKIP_COMPILE=1",
        "APP_VER={}".format(version),
        "riotboot",
    ]
    assert not subprocess.call(cmd)


def get_ipv6_addr(child):
    child.expect_exact('>')
    child.sendline('ifconfig')
    if USE_ETHOS == 0:
        # Get device global address
        child.expect(
            r"inet6 addr: (?P<gladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
            "  scope: global  VAL"
        )
        addr = child.match.group("gladdr").lower()
    else:
        # Get device local address
        child.expect_exact("Link type: wired")
        child.expect(
            r"inet6 addr: (?P<lladdr>[0-9a-fA-F:]+:[A-Fa-f:0-9]+)"
            "  scope: link  VAL"
        )
        addr = "{}%{}".format(child.match.group("lladdr").lower(), TAP)
    return addr


def ping6(client):
    print("pinging node...")
    ping_ok = False
    for _ in range(10):
        try:
            subprocess.check_call(["ping", "-q", "-c1", "-w1", client])
            ping_ok = True
            break
        except subprocess.CalledProcessError:
            pass

    if not ping_ok:
        print("pinging node failed. aborting test.")
        sys.exit(1)
    else:
        print("pinging node succeeded.")
    return ping_ok


def get_reachable_addr(child):
    # Give some time for the network interface to be configured
    time.sleep(1)
    # Get address
    client_addr = get_ipv6_addr(child)
    # Verify address is reachable
    ping6(client_addr)
    return "[{}]".format(client_addr)


def app_version(child):
    utils.test_utils_interactive_sync_shell(child, 5, 1)
    # get version of currently running image
    # "Image Version: 0x00000000"
    child.sendline('riotboot-hdr')
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F:]+)\r\n")
    app_ver = int(child.match.group("app_ver"), 16)
    return app_ver


def running_slot(child):
    utils.test_utils_interactive_sync_shell(child, 5, 1)
    # get version of currently running image
    # "Image Version: 0x00000000"
    child.sendline('current-slot')
    child.expect(r"Running from slot (\d+)\r\n")
    slot = int(child.match.group(1))
    return slot


def testfunc(child):
    # Get current app_ver and slot
    current_app_ver = app_version(child)
    current_slot = running_slot(child)
    # Verify client is reachable and get address
    client = get_reachable_addr(child)

    for version in [current_app_ver + 1, current_app_ver + 2]:
        # Create newer slots bins
        make_riotboot_slots(version)
        # Trigger update process
        make_notify(client, current_slot ^ 1, version)
        child.expect(
            r"riotboot_flashwrite: initializing update to target slot (\d+)\r\n",
        )
        target_slot = int(child.match.group(1))
        # Wait for update to complete
        while wait_for_update(child) == 0:
            pass
        child.sendline('reboot')
        child.expect_exact("Starting the shell")
        # Verify running slot
        current_slot = running_slot(child)
        assert target_slot == current_slot, "BOOTED FROM SAME SLOT"
        # Verify client is reachable and get address
        client = get_reachable_addr(child)

    child.sendline("riotboot-invalidate {}".format(current_slot))
    child.sendline('reboot')
    child.expect_exact("Starting the shell")
    assert running_slot(child) != current_slot, "DID NOT INVALIDATE"

    print("TEST PASSED")


if __name__ == "__main__":
    sys.exit(run(testfunc, echo=True))

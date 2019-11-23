#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import subprocess
import sys
import tempfile
import time

from testrunner import run

# Default test over loopback interface
COAP_HOST = "[fd00:dead:beef::1]"

UPDATING_TIMEOUT = 10
MANIFEST_TIMEOUT = 15

USE_ETHOS = int(os.getenv("USE_ETHOS", "1"))
TAP = os.getenv("TAP", "riot0")
TMPDIR = tempfile.TemporaryDirectory()


def start_aiocoap_fileserver():
    aiocoap_process = subprocess.Popen(
        "exec aiocoap-fileserver %s" % TMPDIR.name, shell=True
    )

    return aiocoap_process


def cleanup(aiocoap_process):
    aiocoap_process.kill()
    TMPDIR.cleanup()


def notify(coap_server, client_url, version=None):
    cmd = [
        "make",
        "suit/notify",
        "SUIT_COAP_SERVER={}".format(coap_server),
        "SUIT_CLIENT={}".format(client_url),
    ]
    if version is not None:
        cmd.append("SUIT_NOTIFY_VERSION={}".format(version))
    assert not subprocess.call(cmd)


def publish(server_dir, server_url, app_ver, latest_name=None):
    cmd = [
        "make",
        "suit/publish",
        "SUIT_COAP_FSROOT={}".format(server_dir),
        "SUIT_COAP_SERVER={}".format(server_url),
        "APP_VER={}".format(app_ver),
        "RIOTBOOT_SKIP_COMPILE=1",
    ]
    if latest_name is not None:
        cmd.append("SUIT_MANIFEST_SIGNED_LATEST={}".format(latest_name))

    assert not subprocess.call(cmd)


def wait_for_update(child):
    return child.expect([r"riotboot_flashwrite: processing bytes (\d+)-(\d+)",
                         "riotboot_flashwrite: riotboot flashing "
                         "completed successfully"],
                        timeout=UPDATING_TIMEOUT)


def get_ipv6_addr(child):
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
    for _i in range(10):
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


def testfunc(child):
    """For one board test if specified application is updatable"""

    # Initial Setup and wait for address configuration
    child.expect_exact("main(): This is RIOT!")

    # get version of currently running image
    # "Image Version: 0x00000000"
    child.expect(r"Image Version: (?P<app_ver>0x[0-9a-fA-F:]+)")
    current_app_ver = int(child.match.group("app_ver"), 16)

    for version in [current_app_ver + 1, current_app_ver + 2]:
        child.expect_exact("suit_coap: started.")
        child.expect_exact("Starting the shell")
        # give some time for the network interface to be configured
        time.sleep(1)
        # Get address, if using ethos it will change on each reboot
        client_addr = get_ipv6_addr(child)
        client = "[{}]".format(client_addr)
        # Wait for suit_coap thread to start
        # Ping6
        ping6(client_addr)
        # Trigger update process, verify it validates manifest correctly
        publish(TMPDIR.name, COAP_HOST, version)
        notify(COAP_HOST, client, version)
        child.expect_exact("suit_coap: trigger received")
        child.expect_exact("suit: verifying manifest signature...")
        child.expect(
            r"riotboot_flashwrite: initializing update to target slot (\d+)",
            timeout=MANIFEST_TIMEOUT,
        )
        target_slot = int(child.match.group(1))
        # Wait for update to complete
        while wait_for_update(child) == 0:
            pass

        # Verify running slot
        child.expect(r"running from slot (\d+)")
        assert target_slot == int(child.match.group(1)), "BOOTED FROM SAME SLOT"

    print("TEST PASSED")


if __name__ == "__main__":
    try:
        res = 1
        aiocoap_process = start_aiocoap_fileserver()
        # TODO: wait for coap port to be available

        res = run(testfunc, echo=True)

    except Exception as e:
        print(e)
    finally:
        cleanup(aiocoap_process)

    sys.exit(res)

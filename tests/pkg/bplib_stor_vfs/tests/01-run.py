#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Simon Grund
# SPDX-License-Identifier: LGPL-2.1-only

import sys

from testrunner import run

TEST_PAYLOAD = "Hello World!"


# This should be set to the minimum number of bytes a bundle takes up on storage.
# Note this value is currently only approximated but good enough
BYTES_IN_STORAGE_PER_BUNDLE = 900

bundles_in_storage = 0

def match_storage_info(child):
    child.expect(r'bundles in storage: (-?\d+), bytes used: (-?\d+), limit: (-?\d+)\s')
    num       = int(child.match.group(1))
    bytes     = int(child.match.group(2))
    bytes_max = int(child.match.group(3))
    return (num, bytes, bytes_max)

def validate_storage_info(child):
    num, bytes, bytes_max = match_storage_info(child)
    assert num >= 0 and bytes >= 0 and bytes_max >= 0
    assert num == bundles_in_storage
    assert bytes >= num * BYTES_IN_STORAGE_PER_BUNDLE
    assert bytes <= bytes_max

def test1(child):
    global bundles_in_storage

    child.expect(r'bplib ingress SUCCESS\s')
    bundles_in_storage += 1
    validate_storage_info(child)

    # TODO Process vfs info to check bundle is actually there

    child.expect(r'bplib egress SUCCESS: ([0-9A-F]+)\s')
    bundle = child.match.group(1)
    bundles_in_storage -= 1
    validate_storage_info(child)

    # The payload should be encoded in the bundle, since no encryption is used
    assert TEST_PAYLOAD.encode().hex().upper() in bundle.upper()



def testfunc(child):
    # General initialization
    child.expect(r'bplib init SUCCESS\s')

    test1(child)

    # TODO things to test:
    # this node -> external (semi done)
    # external -> this node
    # this node -> this node
    # external -> external
    #
    # Always check bundle is saved, index file is present etc, all the things
    # that are specific to the VFS storage. All of the other things should hold
    # in general
    #
    # Other test subjects:
    # Garbage collection works when bundle times out
    #       Also check all the things i.e. in storage counter
    # Bundles that are egressed are actually deleted (!! check on VFS)
    # Custody transfer: Bundles remain on storage after being egressed
    #           But are deleted when the ACK arrives
    # Duplicate bundle received does not break things (hard to test)
    # When the storage bound is reached, no more bundles are stored
    #
    # For all of this transition to shell based tests

    # TODO test finding: Duplicate storage bundle also calls delete, which
    # decrements the counter, leading to wrap around

    


if __name__ == "__main__":
    # Note: because the maintenance thread runs infrequently this timeout has to
    # be quite large
    sys.exit(run(testfunc, timeout=30))

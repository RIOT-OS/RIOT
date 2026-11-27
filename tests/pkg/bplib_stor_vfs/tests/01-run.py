#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Simon Grund
# SPDX-License-Identifier: LGPL-2.1-only

import sys
import time

from testrunner import run

TEST_PAYLOAD = 'Hello World!'
RETRANSMIT_TIME = 5

# ACK bundles. This was captured from a conversation in the UDP example with the
# correct node numbers etc. This ACKs the first sequence number, so the channel
# has to have been just created or the sequence number reset for this to work.
BUNDLE_ACK_TEST2 = '9f8907020182028218c8008202821901900082028200008200001a0036ee' \
            '80425cb286070200014218e142577a860101000149820da1018301008101428b4cff'


# This should be set to the minimum number of bytes a bundle takes up on storage.
# Note this value is currently only approximated but good enough
BYTES_IN_STORAGE_PER_BUNDLE = 840

bundles_in_storage = 0

def match_storage_info(child):
    child.expect(r'Storage usage \[B\]: (-?\d+) / (-?\d+), num stored: (-?\d+)\s')
    bytes     = int(child.match.group(1))
    bytes_max = int(child.match.group(2))
    num       = int(child.match.group(3))
    return (num, bytes, bytes_max)

def verify_storage_info(child):
    child.sendline('bplib info')
    num, bytes, bytes_max = match_storage_info(child)

    assert num >= 0 and bytes >= 0 and bytes_max >= 0
    assert num == bundles_in_storage
    assert bytes >= num * BYTES_IN_STORAGE_PER_BUNDLE
    assert bytes <= bytes_max

def bplib_send_and_verify(child, command):
    """
    Send a bplib configuration command and expects that the query result is the
    value that was just entered.
    """
    setting, value = command.strip().rsplit(' ', 1)
    child.sendline(command)
    child.sendline(setting)
    child.expect_exact(f'Current value: {value}')

def verify_bundle_on_vfs(child, node, service):
    # Note the sizes are also captured but not processed further for now.
    child.sendline('ls /nvm0/bp/idx')
    child.expect(r'([0-9a-f]+)\s(\d+) B\s')
    bundle_id = child.match.group(1)
    child.expect_exact('total 1 files')

    child.sendline(f'ls /nvm0/bp/dat/{hex(node)[2:]}/{hex(service)[2:]}')
    child.expect(r'[0-9a-f]+_{}\s(\d+) B\s'.format(bundle_id))
    child.expect_exact('total 1 files')

def verify_bundle_not_on_vfs(child, node, service):
    # Note the sizes are also captured but not processed further for now.
    child.sendline('ls /nvm0/bp/idx')
    child.expect_exact('total 0 files')

    child.sendline(f'ls /nvm0/bp/dat/{hex(node)[2:]}/{hex(service)[2:]}')
    child.expect_exact('total 0 files')

def test1(child):
    """
    Test a bundle from this node to another node. It will be stored first and
    only egressed when the contact is available. No custody transfer is used.

    tested requirements:
    - shell commands for configuration of channel, config and sending
    - bplib processing, bundle generation, routing and egressing
    - Stored bundle is actually found on storage while no contact is available
    - bplib increments the number and stored bytes correctly
    - After egressing (no CT) the bundle is not present in the storage anymore
    - bplib decrements the number and stored bytes correctly

    pre-/postcondition:
    - contact / channel 0 in removed state
    - storage empty
    """
    global bundles_in_storage

    dest_node, dest_service = 400, 1234

    # TODO maybe simplify the shell so that only one keyword exists, not a variation
    # in the readback

    bplib_send_and_verify(child, 'bplib config node ipn:200.0')

    bplib_send_and_verify(child, 'bplib channel 0 service 100')
    bplib_send_and_verify(child, f'bplib channel 0 dest ipn:{dest_node}.{dest_service}')
    bplib_send_and_verify(child, 'bplib channel 0 report_to dtn:none')

    bplib_send_and_verify(child, 'bplib channel 0 block ct include false')

    bplib_send_and_verify(child, f'bplib channel 0 lifetime {30 * 1000}')

    child.sendline('bplib channel 0 state start')
    child.expect_exact('Success')

    child.sendline(f'bplib send 0 "{TEST_PAYLOAD}"')
    #child.expect_exact('Success') # TODO this fails, no success is currently sent

    # At this point the bundle should be in storage, since the contact is not
    # ready yet. This is compared with bplibs information and vfs.
    time.sleep(1)

    bundles_in_storage += 1
    verify_storage_info(child)

    # Check index file and also bundle data file is present
    verify_bundle_on_vfs(child, dest_node, dest_service)

    # Tell bplib the contact is now ready and initiate egress on the CLA. bplib
    # may take a while because the thing that pulls from the storage is the
    # maintenance thread, not the CLA itself.
    child.sendline('bplib contact 0 state start')
    child.expect_exact('Success')

    child.sendline('mock_egress 0 10000')
    child.expect(r'bplib egress SUCCESS: ([0-9A-F]+)\s')
    bundle = child.match.group(1)

    bundles_in_storage -= 1
    verify_storage_info(child)

    # Now bplib claims it has been deleted, check that is really the case
    verify_bundle_not_on_vfs(child, dest_node, dest_service)

    # The payload should be encoded in the bundle, since no encryption is used.
    # This could be extended much more (e.g. checking the bundle is valid against
    # the specification) but bplib upstream has tests for that.
    assert TEST_PAYLOAD.encode().hex().upper() in bundle.upper()

    child.sendline('bplib channel 0 state remove')
    child.expect_exact('Success')

    child.sendline('bplib contact 0 state teardown')
    child.expect_exact('Success')

def test2(child):
    """
    Test a bundle from this node to another node. It will be stored first and
    only egressed when the contact is available. Custody transfer IS used.

    tested requirements:
    - shell commands for configuration of channel, config and sending
    - bplib processing, bundle generation, routing and egressing
    - Stored bundle is actually found on storage while no contact is available
    - bplib increments the number and stored bytes correctly
    - After one egress cycle the bundle is STILL in storage
    - Retransmissions are spaced at least by the given interval
    - Only after receiving the ACK the bundle will not be egressed again
    - This bundle is also not found on storage anymore
    - bplib decrements the number and stored bytes correctly

    pre-/postcondition:
    - contact / channel 0 in removed state
    - storage empty
    """
    global bundles_in_storage
    
    dest_node, dest_service = 400, 1234

    bplib_send_and_verify(child, 'bplib config node ipn:200.0')

    bplib_send_and_verify(child, 'bplib channel 0 service 100')
    bplib_send_and_verify(child, f'bplib channel 0 dest ipn:{dest_node}.{dest_service}')
    bplib_send_and_verify(child, 'bplib channel 0 report_to dtn:none')

    bplib_send_and_verify(child, 'bplib channel 0 block ct include true')

    bplib_send_and_verify(child, f'bplib channel 0 lifetime {60 * 1000}')

    child.sendline('bplib channel 0 state start')
    child.expect_exact('Success')

    child.sendline(f'bplib send 0 "{TEST_PAYLOAD}"')
    #child.expect_exact('Success') # TODO this fails, no success is currently sent

    # At this point the bundle should be in storage, since the contact is not
    # ready yet. This is compared with bplibs information and vfs.
    time.sleep(1)

    bundles_in_storage += 1
    verify_storage_info(child)

    # Check index file and also bundle data file is present
    verify_bundle_on_vfs(child, dest_node, dest_service)

    # Tell bplib the contact is now ready and initiate egress on the CLA. bplib
    # may take a while because the thing that pulls from the storage is the
    # maintenance thread, not the CLA itself.
    child.sendline('bplib contact 0 state start')
    child.expect_exact('Success')
    t1 = time.time()

    child.sendline('mock_egress 0 10000')
    child.expect(r'bplib egress SUCCESS: ([0-9A-F]+)\s', timeout=15)
    bundle1 = child.match.group(1)

    # This case uses Custody Transfer, so the bundle should remain in storage
    # until an ACK is received. It can be egressed multiple times.

    verify_storage_info(child)
    verify_bundle_on_vfs(child, dest_node, dest_service)

    child.sendline('mock_egress 0 15000')
    child.expect(r'bplib egress SUCCESS: ([0-9A-F]+)\s', timeout=15)
    t2 = time.time()
    assert t2 - t1 >= RETRANSMIT_TIME
    bundle2 = child.match.group(1)

    # Mock send the ACK. After this the bundle should be gone
    child.sendline(f'mock_ingress 0 {BUNDLE_ACK_TEST2}')
    child.expect_exact('bplib ingress SUCCESS')

    time.sleep(1)

    bundles_in_storage -= 1
    verify_storage_info(child)

    # Now bplib claims it has been deleted, check that is really the case
    verify_bundle_not_on_vfs(child, dest_node, dest_service)

    # The payload should be encoded in the bundle, since no encryption is used.
    # This could be extended much more (e.g. checking the bundle is valid against
    # the specification) but bplib upstream has tests for that.
    assert TEST_PAYLOAD.encode().hex().upper() in bundle1.upper()
    assert TEST_PAYLOAD.encode().hex().upper() in bundle2.upper()

    child.sendline('bplib channel 0 state remove')
    child.expect_exact('Success')

    child.sendline('bplib contact 0 state teardown')
    child.expect_exact('Success')

def test3(child):
    """
    Test if the storage limit is respected and if bundles exceeding their lifetime
    are deleted automatically.

    tested requirements:
    - shell commands for configuration of channel, config and sending
    - bplib processing, bundle generation
    - bundles are only stored if there is enough space (BPLIB_MAX_STORED_BUNDLE_BYTES)
    - Stored bundle is actually found on storage while no contact is available
    - bplib increments the number and stored bytes correctly
    - After the lifetime expires the bundle is deleted
    - bplib decrements the number and stored bytes correctly

    pre-/postcondition:
    - contact / channel 0 in removed state
    - storage empty
    """
    global bundles_in_storage
    
    dest_node, dest_service = 400, 1234
    lifetime = 3 * 1000

    bplib_send_and_verify(child, 'bplib config node ipn:200.0')

    bplib_send_and_verify(child, 'bplib channel 0 service 100')
    bplib_send_and_verify(child, f'bplib channel 0 dest ipn:{dest_node}.{dest_service}')
    bplib_send_and_verify(child, 'bplib channel 0 report_to dtn:none')

    bplib_send_and_verify(child, 'bplib channel 0 block ct include false')

    bplib_send_and_verify(child, f'bplib channel 0 lifetime {lifetime}')

    child.sendline('bplib channel 0 state start')
    child.expect_exact('Success')

    child.sendline(f'bplib send 0 "{TEST_PAYLOAD}"')
    #child.expect_exact('Success') # TODO this fails, no success is currently sent
    child.sendline(f'bplib send 0 "{TEST_PAYLOAD}"')
    #child.expect_exact('Success') # TODO this fails, no success is currently sent

    # Now two bundles were ingressed but only one should have been stored, because
    # the size of the storage is limited
    time.sleep(1)

    bundles_in_storage += 1
    verify_storage_info(child)

    # Check index file and also bundle data file is present
    verify_bundle_on_vfs(child, dest_node, dest_service)

    # Now don't start the contact, but wait for the bundle to expire
    time.sleep((lifetime / 1000) * 2)

    bundles_in_storage -= 1
    verify_storage_info(child)

    # Now bplib claims it has been deleted, check that is really the case
    verify_bundle_not_on_vfs(child, dest_node, dest_service)

    child.sendline('bplib channel 0 state remove')
    child.expect_exact('Success')

def testfunc(child):
    # General initialization
    # child.expect(r'bplib init SUCCESS\s')

    test1(child)    # Test storage behavior for non CT bundles
    child.sendline('restart_bplib')
    test2(child)    # Test storage behavior for CT bundles
    child.sendline('restart_bplib')
    # test3(child)    # Test garbage collection for expired bundles
    # TODO uncomment above, GC is not implemented

    # TODO things to test:
    #
    # Custody transfer ACK generation
    # Duplicate bundle received does not break things (hard to test)

    # TODO test findings:
    # - Duplicate storage bundle also calls delete, which
    #   decrements the counter, leading to wrap around
    # - Without the NC defaulting, bplib_stor_vfs_update_retransmission_time(path,
    #   may disable retransmissions. Either default NC or increment by 1. Also prevent
    #   Setting this manually to 1 (guard the NC functions more)
    # - Terminating and starting again should result in the same state. Threads
    #   have a problem.

    


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=10))

#!/usr/bin/env python3
# SPDX-FileCopyrightText: 2026 Jeewoong Kim
# SPDX-License-Identifier: LGPL-2.1-only

"""
Regression test for preserving queued ETHOS DATA when TEXT frame arrives.

The ELF path can be supplied explicitly or obtained from RIOT's ELFFILE
environment variable.
"""
import os
import pty
import re
import sys
import time

try:
    import pexpect
except ImportError:
    print("error: python3-pexpect is required", file=sys.stderr)
    sys.exit(2)


WAIT_FRAMESTART = 0
IN_FRAME = 1

ETHOS_FRAME_TYPE_DATA = 0
ETHOS_FRAME_TYPE_TEXT = 1


DATA_FRAME = bytes([
    0x7E,
    0xAA, 0xBB, 0xCC, 0xDD,
    0x7E,             # frame end (stored in inbuf)
])

TEXT_HEADER = bytes([
    0x7E,
    0x7D,
    0x21,             # ETHOS_FRAME_TYPE_TEXT (0x01) ^ 0x20
])

ESCAPED_DELIM_FRAME = bytes([
    0x7E,
    0xAA,
    0x7D, 0x5E,   # escaped 0x7e
    0xBB,
    0x7E,
])

ESCAPED_ESC_FRAME = bytes([
    0x7E,
    0xAA,
    0x7D, 0x5D,   # escaped 0x7d
    0xBB,
    0x7E,
])

STATUS_RE = re.compile(
    r"ETHOS_TEST pending=(\d+) held=(\d+) state=(\d+) type=(\d+)"
)


def status(child):
    child.sendline("ethos_status")
    child.expect(STATUS_RE)
    return tuple(int(child.match.group(i)) for i in range(1, 5))


def recv_frame(child, expected):
    child.sendline("ethos_recv")
    child.expect_exact(expected)


def main():
    if len(sys.argv) > 2:
        print(f"usage: {sys.argv[0]} [native-elf]", file=sys.stderr)
        return 2

    elf_arg = sys.argv[1] if len(sys.argv) == 2 else os.environ.get("ELFFILE")

    if not elf_arg:
        print("error: ELF path was not provided", file=sys.stderr)
        return 2

    elf = os.path.abspath(elf_arg)

    if not os.path.isfile(elf):
        print(f"error: ELF not found: {elf}", file=sys.stderr)
        return 2

    master_fd, slave_fd = pty.openpty()
    slave_name = os.ttyname(slave_fd)

    print(f"[host] UART PTY: {slave_name}")
    child = pexpect.spawn(
        elf,
        ["-c", slave_name],
        encoding="utf-8",
        timeout=10,
    )
    child.logfile_read = sys.stdout

    try:
        child.expect_exact("Initialization successful - starting the shell now")

        child.sendline("ethos_hold 1")
        child.expect_exact("ETHOS_TEST hold=1")

        # Queue one complete DATA frame while holding RX_COMPLETE.
        os.write(master_fd, DATA_FRAME)
        child.expect(r"ETHOS_TEST RX_HELD 1")

        before = status(child)
        before_pending, before_held, before_state, before_type = before

        print(
            f"[host] before TEXT: pending={before_pending}, "
            f"held={before_held}, state={before_state}, type={before_type}"
        )

        # Send only through the TEXT type marker for a deterministic observation point.
        os.write(master_fd, TEXT_HEADER)

        after = None
        deadline = time.monotonic() + 3.0
        while time.monotonic() < deadline:
            cur = status(child)
            if cur[2] == IN_FRAME and cur[3] == ETHOS_FRAME_TYPE_TEXT:
                after = cur
                break
            time.sleep(0.01)

        if after is None:
            print("FAIL: TEXT header was not observed by the ETHOS parser")
            return 1

        after_pending, after_held, after_state, after_type = after
        print(
            f"[host] after TEXT: pending={after_pending}, "
            f"held={after_held}, state={after_state}, type={after_type}"
        )

        if before_pending <= 0:
            print("FAIL: DATA frame was not queued before TEXT injection")
            return 1

        if after_pending != before_pending:
            print(
                "FAIL: queued DATA was lost after TEXT header "
                f"({before_pending} -> {after_pending})"
            )
            return 1

        print("PASS: queued DATA survived TEXT header")

        # Finish the TEXT frame so the parser returns to WAIT_FRAMESTART
        # before starting the compatibility tests.
        os.write(master_fd, bytes([0x7E]))

        reset = None
        deadline = time.monotonic() + 3.0
        while time.monotonic() < deadline:
            cur = status(child)

            if cur[2] == WAIT_FRAMESTART and cur[3] == ETHOS_FRAME_TYPE_DATA:
                reset = cur
                break

            time.sleep(0.01)

        if reset is None:
            print("FAIL: parser did not return to WAIT_FRAMESTART")
            return 1

        recv_frame(child, "ETHOS_TEST recv=4 data=aabbccdd")

        os.write(master_fd, ESCAPED_DELIM_FRAME)
        child.expect(r"ETHOS_TEST RX_HELD \d+")

        recv_frame(child, "ETHOS_TEST recv=3 data=aa7ebb")

        print("PASS: escaped frame delimiter decoded correctly")

        # Verify that an escaped ESC byte still decodes to 0x7d.
        os.write(master_fd, ESCAPED_ESC_FRAME)
        child.expect(r"ETHOS_TEST RX_HELD \d+")

        recv_frame(child, "ETHOS_TEST recv=3 data=aa7dbb")

        print("PASS: escaped ESC decoded correctly")

        return 0

    finally:
        child.close(force=True)
        os.close(master_fd)
        os.close(slave_fd)


if __name__ == "__main__":
    sys.exit(main())

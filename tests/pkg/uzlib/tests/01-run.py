#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2021 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import sys
from testrunner import run


def testfunc(child):
    child.expect(r"Compressing message \(len: (\d+)\): \"([A-Za-z\., ]+)\"\r\n")
    msg_len = int(child.match.group(1))
    msg = str(child.match.group(2))
    assert msg_len == len(msg), "Wrong msg_len: {} (expected: {})".format(len(msg), msg_len)
    child.expect(r"Message compressed to \d+ raw bytes \(ratio: (\d+.\d+)\)\r\n")
    ratio = float(child.match.group(1))
    assert ratio <= 1, "Ratio is greater than 1 ({})".format(ratio)
    child.expect(r"Decompressed message \(len: (\d+)\): \"([A-Za-z\., ]+)\"\r\n")
    result_msg_len = int(child.match.group(1))
    result_msg = str(child.match.group(2))
    assert result_msg_len == msg_len, "Messages length don't match"
    assert msg == result_msg, "Messages don't match"


if __name__ == "__main__":
    sys.exit(run(testfunc))

# Copyright (C) 2019 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# Author: Juan Carrano <j.carrano@fu-berlin.de>

import os
import sys
import base64
from functools import partial

MAX_LINE = 128


def safe_encode(data):
    """Empty lines will confuse the target, replace them with padding."""
    return base64.b64encode(data).decode('ascii') if data else ""


def test(vectors, child):
    def _safe_expect_exact(s):
        idx = child.expect_exact([s+'\r\n', '{error}\r\n'])
        assert idx == 0
        return idx

    def _safe_sendline(l):
        assert len(l) < MAX_LINE
        _safe_expect_exact('{ready}')
        child.sendline(l)

    for passwd, salt, iters, key in vectors:
        _safe_sendline(passwd)
        _safe_sendline(safe_encode(salt))
        _safe_sendline(str(iters))

        expected_key = base64.b64encode(key).decode('ascii')
        _safe_expect_exact(expected_key)


def main(vectors):
    sys.path.append(os.path.join(os.environ['RIOTTOOLS'], 'testrunner'))
    from testrunner import run
    sys.exit(run(partial(test, vectors)))

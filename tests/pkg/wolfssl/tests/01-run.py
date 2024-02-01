#!/usr/bin/env python3

import os
import sys
from testrunner import run
from testrunner import TIMEOUT as DEFAULT_TIMEOUT


# native is the default platform for this test
BOARD = os.environ.get("BOARD", "native")
# Increase timeout on "real" hardware
# ED25519 takes +160s on samr21-xpro
# ED25519 takes +230s on nucleo-l073rz
# ED25519 takes +500s on nrf51dk
TEST_TIMEOUT = 600 if BOARD not in ['native', 'native64'] else DEFAULT_TIMEOUT
# ECDSA 256 takes +30s on samr21-xpro
# ECDSA 256 takes +40s on nrf51dk
BENCH_TIMEOUT = 40 if BOARD not in ['native', 'native64'] else 20


def _wait_for_test(child):
    return child.expect(["test passed!", "Test complete"],
                        timeout=TEST_TIMEOUT)


def _wait_for_bench(child):
    return child.expect([r"MB\/s", r"ops\/sec",
                         "Benchmark complete",
                         "wolfSSL Benchmark disabled"],
                        timeout=BENCH_TIMEOUT)


def testfunc(child):
    while _wait_for_test(child) == 0:
        pass
    while _wait_for_bench(child) <= 1:
        pass


if __name__ == "__main__":
    sys.exit(run(testfunc))

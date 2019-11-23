#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact("START")
    child.expect_exact("THREADS CREATED")
    child.expect_exact("THREAD nr1 (pid:3) start")
    child.expect_exact("THREAD nr1 (pid:3) end.")
    child.expect_exact("THREAD nr2 (pid:4) start")
    child.expect_exact("THREAD nr3 (pid:5) start")
    child.expect_exact("Got msg from pid 3: \"nr1\"")
    child.expect_exact("THREAD nr2 (pid:4) end.")
    child.expect_exact("Got msg from pid 4: \"nr2\"")
    child.expect_exact("THREAD nr3 (pid:5) end.")
    child.expect_exact("Got msg from pid 5: \"nr3\"")
    child.expect_exact("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))

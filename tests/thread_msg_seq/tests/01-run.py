#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect("START")
    child.expect("THREADS CREATED")
    child.expect("THREAD nr1 \(pid:3\) start")
    child.expect("THREAD nr1 \(pid:3\) end.")
    child.expect("THREAD nr2 \(pid:4\) start")
    child.expect("THREAD nr3 \(pid:5\) start")
    child.expect("Got msg from pid 3: \"nr1\"")
    child.expect("THREAD nr2 \(pid:4\) end.")
    child.expect("Got msg from pid 4: \"nr2\"")
    child.expect("THREAD nr3 \(pid:5\) end.")
    child.expect("Got msg from pid 5: \"nr3\"")
    child.expect("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc))

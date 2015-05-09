#!/usr/bin/env python

from pexpect import spawn, TIMEOUT

DEFAULT_TIMEOUT=5
hwtimers = 0

term = spawn("make term")

term.expect(r"Setting timers:", timeout=DEFAULT_TIMEOUT)

try:
    while True:
        term.expect(r"set callback\s+\d+", timeout=DEFAULT_TIMEOUT)
        hwtimers += 1
except TIMEOUT:
    pass

term.expect(r"All timers set.", timeout=DEFAULT_TIMEOUT)
term.expect(r"callback\s+1", timeout=DEFAULT_TIMEOUT)
print("Got callback 1")

while hwtimers > 1:
    term.expect(r"callback\s+{:d}".format(hwtimers), timeout=DEFAULT_TIMEOUT)
    print("Got callback {:d}".format(hwtimers))
    hwtimers -= 1

# Setting timers:
# term.expect("OK")
if not term.terminate():
    term.terminate(force=True)

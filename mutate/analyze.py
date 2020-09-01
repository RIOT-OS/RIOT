#!/usr/bin/env python3

import re
import sys
from datetime import datetime as dt

TESTLOG = "mutate/logs/test.log"

stdout = open(sys.argv[1]).read()
stderr = open(sys.argv[2]).read()

print(f"analyze script called: {dt.now()}", file=open(TESTLOG, "a"))

err = re.findall(r"/(xtimer_.*?|bench_xtimer)/", stderr)
err = set(err)  # make it unique

# find all test cases name
testcases = re.findall(r"\[START\]: (xtimer_.*?|bench_xtimer)\n", stdout)

# find successful test cases
for test in testcases:
    match = re.search(r"^(?P<status>\[SUCCESS\]): {:s}".format(test), stdout, re.MULTILINE)
    if match and test not in err:
        status = "passed"
    else:
        status = "failed"
        err.discard(test)

    print(f"{status}: {test}")

# print out the rest of failed tests
for test in err:
    print(f"failed: {test}")

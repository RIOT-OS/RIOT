#!/usr/bin/env python3

import sys
from testrunner import run


def _check_test_output(child):
    child.expect('show tls values:')
    for i in range(20):
        if i != 5:
            child.expect('key\[%d\]: \d+, val: %d'
                         % (i, i + 1 if i != 3 else 42))


def testfunc(child):
    child.expect('START')
    child.expect('-= TEST 1 - create 20 tls with sequencial values 0...19 =-')
    _check_test_output(child)
    child.expect('-= TEST 2 - '
                 'delete deliberate key \(key\[5\]:\d+\) =-')
    _check_test_output(child)
    child.expect('-= TEST 3 - create new tls =-')
    _check_test_output(child)
    child.expect('-= TEST 4 - delete all keys =-')
    child.expect('show tls values:')
    child.expect('-= TEST 5 - try delete non-existing key =-')
    child.expect('try to delete returns: 0')
    child.expect('-= TEST 6 - add key and delete without a tls =-')
    child.expect('created key: \d+')
    child.expect('try to delete returns: 0')
    child.expect('-= TEST 7 - add key without tls =-')
    child.expect('created key: \d+')
    child.expect('test_7_val: (0|\(nil\))')
    child.expect('tls tests finished.')
    child.expect('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))

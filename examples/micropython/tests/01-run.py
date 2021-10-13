#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    def get_time():
        child.sendline('utime.time()')
        child.readline()
        res = int(child.readline().rstrip())
        child.expect_exact('>>>')
        return res

    child.expect_exact('boot.py: MicroPython says hello!')
    child.expect_exact('>>>')

    child.sendline('print("echo this! " * 4)')
    child.expect_exact('echo this! echo this! echo this! echo this!')
    child.expect_exact('>>>')

    # test riot.thread_getpid()
    child.sendline('import riot')
    child.sendline('print(riot.thread_getpid())')
    child.expect(r'\d+')
    child.expect_exact('>>>')

    #
    # test xtimer integration
    #

    child.sendline('import utime')
    child.expect_exact('>>>')

    # testing timing over serial using the REPL is very inaccurate, thus
    # we allow a *large* overshoot (100 by default).
    def test_sleep(t, slack=100):
        before = get_time()
        child.sendline('utime.sleep_ms(%s)' % t)
        child.expect_exact('>>>')
        duration = get_time() - before
        print("test_sleep(%s, %s): slept %sms" % (t, slack, duration))
        assert duration > t and duration < (t + slack)
        return duration

    # get overhead from sleeping 0ms, add 10 percent
    slack = int(test_sleep(0, 1000) * 1.1)

    test_sleep(50, slack)
    test_sleep(250, slack)
    test_sleep(500, slack)

    # test setting timers
    child.sendline('import xtimer')
    child.expect_exact('>>>')
    child.sendline('a = 0')
    child.expect_exact('>>>')

    child.sendline('def inc_a(): global a; a+=1')
    child.expect_exact('...')
    child.sendline('')
    child.expect_exact('>>>')

    child.sendline('t = xtimer.xtimer(inc_a)')
    child.expect_exact('>>>')

    before = get_time()

    child.sendline('t.set(500000)')
    child.expect_exact('>>>')

    child.sendline('while a==0: pass')
    child.expect_exact('...')
    child.sendline('')
    child.expect_exact('>>>')

    duration = get_time() - before
    assert duration > 500

    print("[TEST PASSED]")


if __name__ == "__main__":
    sys.exit(run(testfunc))

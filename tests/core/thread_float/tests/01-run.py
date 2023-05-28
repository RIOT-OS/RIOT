#!/usr/bin/env python3

#  Copyright (C) 2021 Otto-von-Guericke-Universität Magdeburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

# @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>

import sys
from testrunner import run

MIN_PRINTS = 5


def assertAlmostEqual(first, second, delta=0.05):
    assert first + delta > second
    assert first - delta < second


def same_computation_as_in_c_prog(thread_num):
    f = 1.0 * thread_num
    for _ in range(10000):
        f = f + 1.0 / f
    return f


def parse_result_line(child):
    child.expect(r"t(\d): (\d{3}\.\d+|[+-]*nan|[+-]inf)\r\n")
    # Note: intentionally keeping the float output as string to also test that
    # printf("%f", ...) prints the exact same char sequence for the same float
    # value each time
    return (int(child.match.group(1)), child.match.group(2))


def testfunc(child):
    child.expect_exact("THREADS CREATED")
    # Threads are started with THREAD_CREATE_WOUT_YIELD, and thus prone starting
    # in a random sequence depending on any interrupts that cause rescheduling
    threads_started = set()
    for _ in range(3):
        child.expect(r"THREAD t(\d) start\r\n")
        thread_num = int(child.match.group(1))
        assert 1 <= thread_num <= 3, "thread number has to be valid"
        assert thread_num not in threads_started, "same thread must not be launched twice"
        threads_started.add(thread_num)

    first_thread, first_result = parse_result_line(child)
    child.expect(r"t(\d): (\d{3}\.\d+)\r\n")
    first_thread = int(child.match.group(1))
    first_result = child.match.group(2)

    # wait for second thread to print, but wait at most 50 messages
    second_thread = None
    for _ in range(50):
        thread, result = parse_result_line(child)
        if thread != first_thread:
            second_thread = thread
            second_result = result
            break

    assert second_thread is not None, "both threads t1 and t3 should print"
    assert first_thread in [1, 3], "only thread t1 and t3 should print"
    assert second_thread in [1, 3], "only thread t1 and t3 should print"
    assertAlmostEqual(float(first_result), same_computation_as_in_c_prog(first_thread))
    assertAlmostEqual(float(second_result), same_computation_as_in_c_prog(second_thread))

    count_first_thread = 0
    count_second_thread = 0

    # wait for both threads to print at least MIN_PRINTS times, but wait at most 100 messages
    for _ in range(100):
        thread, result = parse_result_line(child)
        assert thread in [1, 3], "only thread t1 and t3 should print"

        if thread == first_thread:
            assert result == first_result, "same calculation but different result"
            count_first_thread += 1
        else:
            assert result == second_result, "same calculation but different result"
            count_second_thread += 1

        if (count_first_thread >= MIN_PRINTS) and (count_second_thread >= MIN_PRINTS):
            break

    msg = "Either t1 or t3 printed less than {} times within 100 messages".format(MIN_PRINTS)
    assert (count_first_thread >= MIN_PRINTS) and (count_second_thread >= MIN_PRINTS), msg


if __name__ == "__main__":
    sys.exit(run(testfunc))

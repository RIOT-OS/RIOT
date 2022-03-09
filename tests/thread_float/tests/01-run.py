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


def testfunc(child):
    child.expect_exact("THREADS CREATED")
    child.expect_exact("THREAD t1 start")
    child.expect_exact("THREAD t2 start")
    child.expect_exact("THREAD t3 start")

    child.expect(r"t(\d): (\d{3}\.\d+)\r\n")
    first_thread = int(child.match.group(1))
    # Note: intentionally keeping the float output as string to also test that printf("%f", ...)
    # prints the exact same char sequence for the same float value each time
    first_result = child.match.group(2)

    # wait for second thread to print, but wait at most 50 messages
    second_thread = None
    for _ in range(50):
        child.expect(r"t(\d): (\d{3}\.\d+)\r\n")
        if int(child.match.group(1)) != first_thread:
            second_thread = int(child.match.group(1))
            second_result = child.match.group(2)
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
        child.expect(r"t(\d): (\d{3}\.\d+)\r\n")
        thread = int(child.match.group(1))
        assert thread in [1, 3], "only thread t1 and t3 should print"
        result = child.match.group(2)

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

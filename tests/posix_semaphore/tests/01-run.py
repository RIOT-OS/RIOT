#!/usr/bin/env python3

# Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
from testrunner import run


def test1(term):
    term.expect_exact("######################### TEST1:")
    term.expect_exact("first: sem_init")
    term.expect_exact("first: thread create")
    term.expect_exact("first: thread created")
    term.expect_exact("first: sem_getvalue")
    term.expect_exact("first: sem_getvalue != 0")
    term.expect_exact("first: do yield")
    term.expect_exact("second: sem_trywait")
    term.expect_exact("second: sem_trywait done with == 0")
    term.expect_exact("second: wait for post")
    term.expect_exact("first: done yield")
    term.expect_exact("first: sem_trywait")
    term.expect_exact("first: sem_trywait FAILED")
    term.expect_exact("first: sem_trywait done")
    term.expect_exact("first: sem_post")
    term.expect_exact("second: sem was posted")
    term.expect_exact("second: end")
    term.expect_exact("first: sem_post done")
    term.expect_exact("first: sem_destroy")
    term.expect_exact("first: end")


def test2(term):
    term.expect_exact("######################### TEST2:")
    term.expect_exact("first: sem_init")
    term.expect_exact("first: thread create: 5")
    term.expect_exact("first: thread created: priority 5 (1/5)")
    term.expect_exact("first: thread create: 4")
    term.expect_exact("first: thread created: priority 4 (2/5)")
    term.expect_exact("first: thread create: 3")
    term.expect_exact("first: thread created: priority 3 (3/5)")
    term.expect_exact("first: thread create: 2")
    term.expect_exact("first: thread created: priority 2 (4/5)")
    term.expect_exact("first: thread create: 1")
    term.expect_exact("first: thread created: priority 1 (5/5)")
    term.expect_exact("------------------------------------------")
    term.expect_exact("post no. 0")
    term.expect_exact("Thread 'priority 1' woke up.")
    term.expect_exact("Back in main thread.")
    term.expect_exact("post no. 1")
    term.expect_exact("Thread 'priority 2' woke up.")
    term.expect_exact("Back in main thread.")
    term.expect_exact("post no. 2")
    term.expect_exact("Thread 'priority 3' woke up.")
    term.expect_exact("Back in main thread.")
    term.expect_exact("post no. 3")
    term.expect_exact("Thread 'priority 4' woke up.")
    term.expect_exact("Back in main thread.")
    term.expect_exact("post no. 4")
    term.expect_exact("Thread 'priority 5' woke up.")
    term.expect_exact("Back in main thread.")


def test3(term):
    term.expect_exact("######################### TEST3:")
    term.expect_exact("first: sem_init s1")
    term.expect_exact("first: sem_init s2")
    term.expect_exact("first: create thread 1")
    term.expect_exact("first: create thread 2")
    term.expect_exact("------------------------------------------")
    term.expect_exact("post s1")
    term.expect_exact("Thread 1 woke up after waiting for s1.")
    term.expect_exact("post s2")
    term.expect_exact("Thread 2 woke up after waiting for s2.")
    term.expect_exact("post s2")
    term.expect_exact("Thread 1 woke up after waiting for s2.")
    term.expect_exact("post s1")
    term.expect_exact("Thread 2 woke up after waiting for s1.")


def test4(term):
    term.expect_exact("######################### TEST4:")
    term.expect_exact("first: sem_init s1")
    term.expect_exact("first: wait 1 sec for s1")
    term.expect_exact("first: timed out")
    term.expect(r"first: waited 1\d{6} usec")


def testfunc(child):
    test1(child)
    test2(child)
    test3(child)
    test4(child)
    child.expect("######################### DONE")


if __name__ == "__main__":
    sys.exit(run(testfunc))

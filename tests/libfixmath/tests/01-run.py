#!/usr/bin/env python3

import os
import sys


def expect_unary(child):
    for _ in range(20):
        for op_name in ('abs', 'sq', 'atan', 'exp'):
            child.expect('{}\(-?\d+\.\d+\) = -?\d+\.\d+'.format(op_name))

    for _ in range(20):
        for op_name in ('sin', 'cos', 'tan'):
            child.expect('{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    for _ in range(20):
        for op_name in ('asin', 'acos'):
            child.expect('{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    for _ in range(20):
        for op_name in ('sqrt', 'log', 'log2', 'slog2'):
            child.expect('{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))


def expect_binary(child):
    for _ in range(1500):
        for op_name in ('add', 'sub', 'mul', 'div', 'mod', 'sadd', 'ssub',
                        'smul', 'sdiv', 'min', 'max'):
            child.expect('{}\(-?\d+.\d+\, -?\d+.\d+\) = -?\d+.\d+'
                         .format(op_name))


def testfunc(child):
    child.expect_exact('Unary.')
    expect_unary(child)
    child.expect_exact('Binary.')
    expect_binary(child)
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.path.append(os.path.join(os.environ['RIOTBASE'], 'dist/tools/testrunner'))
    from testrunner import run
    sys.exit(run(testfunc))

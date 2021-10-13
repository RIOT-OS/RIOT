#!/usr/bin/env python3

import sys
from testrunner import run


def expect_unary(child):
    child.expect(r'COUNT: (\d+)\r\n')
    count = int(child.match.group(1))
    assert count > 0
    for _ in range(count):
        for op_name in ('abs', 'sq', 'atan', 'exp'):
            child.expect(r'{}\(-?\d+\.\d+\) = -?\d+\.\d+'.format(op_name))

    child.expect(r'COUNT: (\d+)\r\n')
    count = int(child.match.group(1))
    assert count > 0
    for _ in range(count):
        for op_name in ('sin', 'cos', 'tan'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    child.expect(r'COUNT: (\d+)\r\n')
    count = int(child.match.group(1))
    assert count > 0
    for _ in range(count):
        for op_name in ('asin', 'acos'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))

    child.expect(r'COUNT: (\d+)\r\n')
    count = int(child.match.group(1))
    assert count > 0
    for _ in range(count):
        for op_name in ('sqrt', 'log', 'log2', 'slog2'):
            child.expect(r'{}\(-?\d+.\d+\) = -?\d+.\d+'.format(op_name))


def expect_binary(child):
    child.expect(r'COUNT: (\d+)\r\n')
    count = int(child.match.group(1))
    assert count > 0
    for _ in range(count):
        for op_name in ('add', 'sub', 'mul', 'div', 'mod', 'sadd', 'ssub',
                        'smul', 'sdiv', 'min', 'max'):
            child.expect(r'{}\(-?\d+.\d+\, -?\d+.\d+\) = -?\d+.\d+'
                         .format(op_name))


def testfunc(child):
    child.expect_exact('Unary.')
    expect_unary(child)
    child.expect_exact('Binary.')
    expect_binary(child)
    child.expect_exact('SUCCESS')


if __name__ == "__main__":
    sys.exit(run(testfunc))

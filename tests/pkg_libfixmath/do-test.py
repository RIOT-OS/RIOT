#!/usr/bin/env python3

import math
import operator
import sys


def rem(a, b):
    ret = a % b
    if ret < 0 and a > 0 and b < 0 or \
       ret > 0 and a < 0 and b > 0:
        ret -= b
    return ret


FUNS = {
    'add': operator.add,
    'sub': operator.sub,
    'mul': operator.mul,
    'div': operator.truediv,
    'mod': rem,

    'sadd': operator.add,
    'ssub': operator.sub,
    'smul': operator.mul,
    'sdiv': operator.truediv,

    'min': min,
    'max': max,

    'abs': abs,
    'sqrt': math.sqrt,
    'sq': lambda x: x * x,

    'sin': math.sin,
    'cos': math.cos,
    'tan': math.tan,
    'asin': math.asin,
    'acos': math.acos,
    'atan': math.atan,

    'exp': math.exp,
    'log': math.log,
    'log2': math.log2,
    'slog2': math.log2,
}

ABS_ERROR_LIMIT = 0.011


def main():
    total = 0
    errors = 0

    print('Calculation: abs result != exp result, abs error > limit')

    started = False
    for line in sys.stdin:
        line = line.strip()
        if not started:
            if line == 'Unary.':
                print(line)
                started = True
            continue
        elif line == 'Binary.':
            print(line)
            continue
        elif line == 'Done.':
            print(line)
            break

        total += 1
        try:
            res_locals = {}
            res_locals['input'], res_locals['expected'] = map(str.strip, line.split('='))
            exec('result = {}'.format(res_locals['input']), FUNS, res_locals)

            abs_error = abs(res_locals['result'] - float(res_locals['expected']))
            res_locals['result'] = '{:.4f}'.format(res_locals['result'])
            if abs_error > ABS_ERROR_LIMIT:
                print('{}: {} != {}, {:.4f} > {}'.format(
                    res_locals['input'],
                    res_locals['result'],
                    res_locals['expected'],
                    abs_error,
                    ABS_ERROR_LIMIT))
                errors += 1
        except Exception as exc:
            errors += 1
            print('ERROR: {}: {}'.format(line, exc))

    print('{} calculations passed.'.format(total - errors))
    if errors:
        print('{} calculations had errors.'.format(errors))
        return 1
    else:
        return 0


if __name__ == '__main__':
    sys.exit(main())

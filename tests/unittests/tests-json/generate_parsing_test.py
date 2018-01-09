#!/usr/bin/env python3

import glob
import os
import os.path
import sys


def transform_string_sub(dest_file, iterator):
    for c in iterator:
        if c < ord(b' ') or c >= 0x7f or c == ord(b'"') or c == ord(b'\\'):
            dest_file.write(b'\\%03o' % (c,))
        else:
            dest_file.write(b'%c' % (c,))


def iter_file_as_ints(input_file):
    while True:
        c = input_file.read(1)
        if not c:
            break
        yield ord(c)


def transform_string(dest_file, input_file, test_name, esc_test_name):
    dest_file.write(b'static const char ')
    dest_file.write(esc_test_name)
    dest_file.write(b'[] = "')
    transform_string_sub(dest_file, iter_file_as_ints(input_file))
    dest_file.write(b'";\n')

    dest_file.write(b'static const char ')
    dest_file.write(esc_test_name)
    dest_file.write(b'_name[] = "')
    transform_string_sub(dest_file, test_name)
    dest_file.write(b'";\n')

    dest_file.write(b'\n')


def escape_test_name(test_name, index):
    escaped_str = b''.join(
        b'%c' % (c,) if c in range(ord('a'), ord('z') + 1) or
                        c in range(ord('A'), ord('Z') + 1) or
                        c in range(ord('0'), ord('9') + 1) and index > 0 else
        b'_%02x' % (c,)
        for index, c in enumerate(test_name)
    )
    return b'json_parsing_test_%s_%d' % (escaped_str, index)


def main(dest_path='./generated-parsing-tests.c',
         source_path='./JSONTestSuite',
         cwd=os.path.dirname(os.path.realpath(__file__))):
    test_names = []
    cwd = os.path.abspath(cwd)
    with open(os.path.join(cwd, dest_path), 'wb') as dest_file:
        # License:
        dest_file.write(b'/* This file was generated using the tests in '
                        b'JSON Parsing Test Suite,\n')
        dest_file.write(b' * which has the following license information:\n')
        dest_file.write(b'\n')
        with open(os.path.join(cwd, source_path, './LICENSE'), 'rb') as license:
            dest_file.write(license.read())
        dest_file.write(b'\n')
        dest_file.write(b' */\n')
        dest_file.write(b'\n')
        dest_file.write(b'/* Upstream version: ???????? */\n')
        dest_file.write(b'#error "PLEASE EDIT UPSTEAM VERSION HASH MANUALLY AND REMOVE THIS LINE!"\n')
        dest_file.write(b'\n')

        # Includes etc.:
        dest_file.write(b'#include "tests-json.h"\n')
        dest_file.write(b'\n')
        dest_file.write(b'#include <stddef.h>\n')
        dest_file.write(b'\n')
        dest_file.write(b'#pragma GCC diagnostic ignored '
                        b'"-Woverlength-strings"\n')
        dest_file.write(b'\n')

        # Strings:
        for index, json_name in enumerate(sorted(glob.glob(
                os.path.join(cwd, source_path, './test_parsing/*.json')))):
            test_name = os.path.basename(json_name)
            test_name = os.path.splitext(test_name)[0]
            test_name = test_name.encode('ascii')
            esc_test_name = escape_test_name(test_name, index)

            with open(json_name, 'rb') as input_file:
                transform_string(dest_file, input_file, test_name,
                                 esc_test_name)
            test_names.append(esc_test_name)

        # Strings in an array:
        dest_file.write(b'const json_parsing_test_t '
                        b'tests_json_parsing_tests[] = {\n')
        for esc_test_name in test_names:
            dest_file.write(b'    ')        # single tab
            dest_file.write(b'{\n')
            dest_file.write(b'        ')    # double tab
            dest_file.write(esc_test_name)
            dest_file.write(b',\n')
            dest_file.write(b'        ')    # double tab
            dest_file.write(esc_test_name)
            dest_file.write(b'_name,\n')
            dest_file.write(b'        ')    # double tab
            dest_file.write(b'sizeof(')
            dest_file.write(esc_test_name)
            dest_file.write(b') - 1\n')
            dest_file.write(b'    ')        # single tab
            dest_file.write(b'},\n')
        dest_file.write(b'    ')        # single tab
        dest_file.write(b'{ NULL, NULL, 0 }\n')
        dest_file.write(b'};\n')
        dest_file.write(b'\n')

        dest_file.write(b'const size_t '
                        b'tests_json_parsing_tests_count = %d;\n' %
                         (len(test_names),))


if __name__ == '__main__':
    os.chdir(os.path.dirname(os.path.realpath(__file__)))
    result = main(*sys.argv[1:])
    sys.exit(0 if result else 1)

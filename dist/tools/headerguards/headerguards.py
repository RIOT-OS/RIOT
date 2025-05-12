#!/usr/bin/env python3

import os
import sys
import difflib
from io import BytesIO, TextIOWrapper

_in = "/-."
_out = "___"

transtab = str.maketrans(_in, _out)


def path_to_guardname(filepath):
    res = filepath.upper().translate(transtab)
    if res.startswith("_"):
        res = "PRIV" + res
    return res


def get_guard_name(filepath):
    parts = filepath.split(os.sep)
    start = 0
    found = False
    for i, part in enumerate(parts):
        if part == "include":
            found = True
            start = i + 1
            break

    if not found:
        start = len(parts) - 1

    return path_to_guardname(os.path.join(*parts[start:]))


def fix_headerguard(filename):
    supposed = get_guard_name(filename)
    with open(filename, "r", encoding='utf-8', errors='ignore') as f:
        inlines = f.readlines()

    tmp = TextIOWrapper(BytesIO(), encoding="utf-8", errors="ignore")
    tmp.seek(0)

    guard_found = 0
    pragma_once_found = 0
    include_next_found = 0
    guard_name = ""
    ifstack = 0
    for line in inlines:
        if line.startswith("#pragma once"):
            pragma_once_found += 1
        elif line.lstrip().startswith("#pragma once"):
            # check for lines that have leading whitespaces and add a correction
            pragma_once_found += 1
            line = "#pragma once\n"
        if guard_found == 0:
            if pragma_once_found == 0 and line.startswith("#ifndef"):
                guard_found += 1
                guard_name = line[8:].rstrip()
                line = "#ifndef %s\n" % (supposed)
            elif pragma_once_found != 0 and line.startswith("#ifndef %s" % supposed):
                # check if there is pragma once *and* headerguards, but avoid false
                # positives by narrowing in on the to-be-expected guard
                guard_found += 1
                guard_name = line[8:].rstrip()
                line = ""
        elif guard_found == 1:
            if line.startswith("#define") and line[8:].rstrip() == guard_name:
                line = "#define %s\n" % (supposed)
                guard_found += 1
            else:
                break
        elif guard_found == 2:
            if line.startswith("#if"):
                ifstack += 1
            elif line.startswith("#endif"):
                if ifstack > 0:
                    ifstack -= 1
                else:
                    guard_found += 1
                    line = "#endif /* %s */\n" % supposed
            elif line.startswith("#include_next"):
                include_next_found = 1

        tmp.write(line)

    tmp.seek(0)
    if (pragma_once_found == 0 and guard_found == 3) or \
       (pragma_once_found == 1 and guard_found == 0):
        # Valid cases:
        #  - no #pragma once, classic headerguards (#ifndef, #define, #endif)
        #  - one #pragma once, no classic headerguards
        if include_next_found == 0:
            for line in difflib.unified_diff(inlines, tmp.readlines(),
                                             "%s" % filename, "%s" % filename):
                sys.stdout.write(line)
    elif (pragma_once_found == 0 and guard_found == 0):
        print("%s: no header guards found" % filename, file=sys.stderr)
        return False
    elif (pragma_once_found == 1 and guard_found != 0):
        print("%s: #pragma once and classic header guards used in the same file" %
              filename, file=sys.stderr)
        return False
    else:
        print("%s: broken header guard" % filename, file=sys.stderr)
        return False


if __name__ == "__main__":
    error = False
    for filename in sys.argv[1:]:
        if fix_headerguard(filename) is False:
            error = True

    if error:
        sys.exit(1)

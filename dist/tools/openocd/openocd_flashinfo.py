#!/usr/bin/env python3
"""
Utility to parse the output of OpenOCD's "flash list" command
"""
import argparse
import sys

NUMERIC_FIELDS = {"base", "size", "bus_width", "chip_width"}


def parse_flash_info(lines):
    """
    Read output of OpenOCD's "flash list" command given in lines into a list
    of dictionaries

    :param lines: Output of "flash list" lines

    :return: [{"name": "nrf52.flash", "base": 0, ...},
              {"name": "nrf52.uicr", ...}, ...]
    """
    tokens = []
    for line in lines:
        for word in line.split():
            if word.startswith('{') and len(word) > 1:
                tokens += ["{", word[1:]]
            elif word.endswith('}') and len(word) > 1:
                tokens += [word[:-1], "}"]
            else:
                tokens.append(word)

    idx = 0
    result = []
    while idx < len(tokens):
        entry = {}
        while idx < len(tokens) and tokens[idx] != "{":
            idx += 1
        idx += 1
        while idx < len(tokens) and tokens[idx] != "}":
            if idx + 1 >= len(tokens) or tokens[idx + 1] == "}":
                break
            key = tokens[idx]
            value = tokens[idx + 1]
            if key in NUMERIC_FIELDS:
                value = int(value, 0)
            entry[key] = value
            idx += 2
        if idx < len(tokens) and tokens[idx] == "}":
            result.append(entry)

    return result


if __name__ == "__main__":
    parser = argparse.ArgumentParser(
            description="Parse OpenOCD's \"flash list\" output")
    parser.add_argument("--field", default="base", type=str,
                        help="Field to extract (default \"base\")")
    parser.add_argument("--idx", default=0, type=int,
                        help="Index of the bank to extract info from " +
                             "(default 0)")
    args = parser.parse_args()
    info = parse_flash_info(sys.stdin)
    if args.idx < 0 or args.idx >= len(info):
        sys.exit("flash bank index out of range")
    value = info[args.idx][args.field]
    if args.field in NUMERIC_FIELDS:
        print(f"0x{value:08x}")
    else:
        print(value)

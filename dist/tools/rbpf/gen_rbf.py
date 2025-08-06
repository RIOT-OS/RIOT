#!/usr/bin/env python3
# -*- coding: utf-8 -*-
# vim:fenc=utf-8

# Copyright (C) 2021 Inria
# Copyright (C) 2021 Koen Zandberg <koen@bergzand.net>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import argparse
import logging
from rbpf import rbf, instructions


def test_instr(arguments):
    instruction = bytes.fromhex("0f02000100000000")
    instr = instructions.from_bytes(instruction)
    print(instr.full_print())


def dump(arguments):
    rbf_content = arguments.file.read()
    rbf_o = rbf.RBF.from_rbf(rbf_content)
    rbf_o.dump(compressed=arguments.compress)


def generate(arguments):
    rbf_o = rbf.RBF.from_elf(arguments.input)
    if arguments.compress:
        data = rbf_o.format_compressed()
    else:
        data = rbf_o.format()
    arguments.output.write(data)


if __name__ == "__main__":
    parser = argparse.ArgumentParser("RIOT BPF format utility")
    parser.add_argument(
        "--verbose", "-v", help="Verbose output", action="store_true", default=False
    )
    parser.add_argument(
        "--debug", "-d", help="All debug output", action="store_true", default=False
    )

    subparsers = parser.add_subparsers(help="sub commands")

    parser_dump = subparsers.add_parser("dump")
    parser_dump.set_defaults(func=dump)
    parser_dump.add_argument("--compress", "-c", action="store_true", default=False)
    parser_dump.add_argument(
        "file", type=argparse.FileType("rb"), help="RBF file to dump"
    )

    parser_test = subparsers.add_parser("test")
    parser_test.set_defaults(func=test_instr)

    parser_gen = subparsers.add_parser("generate")
    parser_gen.set_defaults(func=generate)
    parser_gen.add_argument("--compress", "-c", action="store_true", default=False)
    parser_gen.add_argument(
        "input", type=argparse.FileType("rb"), help="ELF file to read"
    )
    parser_gen.add_argument(
        "output", type=argparse.FileType("wb"), help="RBF file to write"
    )

    args = parser.parse_args()

    logging.basicConfig(format="%(message)s")
    logger = logging.getLogger()
    if args.debug:
        logger.setLevel(logging.DEBUG)
    elif args.verbose:
        logger.setLevel(logging.INFO)
    else:
        logger.setLevel(logging.WARNING)

    args.func(args)

#!/usr/bin/env python3

# Copyright (C) 2020 iosabi
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
NXP Vector table checksum generator for RAW binary files.

Computes and verifies the Vector table checksum entry for the NXP QN908x
family. These MCUs need a specific checksum value in the Vector table in order
to boot.
"""

import argparse
import logging
import struct


class NXPChecksumError(Exception):
    """Generic tool error."""


# Number of uint32_t entries that the checksum covers in the QN908X family.
# The computed checksum is stored right after these values.
CHECKSUM_ENTRIES = 7


def fix_checksum(binary_name, dry_run):
    with open(binary_name, 'rb' if dry_run else 'rb+') as fobj:

        checksum_fmt = '<%dI' % (CHECKSUM_ENTRIES + 1)
        checksum_data_size = struct.calcsize(checksum_fmt)
        data = fobj.read(checksum_data_size)

        if len(data) < checksum_data_size:
            raise NXPChecksumError(
                'File %s is too small, it should have at least %d bytes' % (
                    binary_name, checksum_data_size))

        values = list(struct.unpack(checksum_fmt, data))
        checksum = (-sum(values[:-1])) % (1 << 32)
        if checksum != values[-1]:
            logging.info(
                'Computed checksum 0x%.8x doesn\'t match the value in the file '
                '(0x%.8x), updating...', checksum, values[-1])
            if not dry_run:
                fobj.seek(checksum_data_size - 4)
                fobj.write(struct.pack('<I', checksum))
            else:
                logging.info('Checksum not updated [DRY-RUN].')
            logging.info('Checksum updated. Done.')
        else:
            logging.info('Checksum in file is already valid. Done.')


def main_func():
    parser = argparse.ArgumentParser(description=__doc__)

    parser.add_argument('--dry-run', action='store_true', default=False)
    parser.add_argument('--quiet', action='store_true', default=False)
    parser.add_argument(
        'bin', help='Path to RAW binary to check/update', type=str)

    args = parser.parse_args()
    logging.basicConfig(
        format='%(filename)s: %(message)s',
        level=logging.ERROR if args.quiet else logging.INFO)

    fix_checksum(args.bin, args.dry_run)


if __name__ == '__main__':
    main_func()

#!/usr/bin/env python3

#
# Copyright (C) 2019 Inria
#               2019 FU Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

import os
import hashlib
import json
import uuid
import argparse

from suit_manifest_encoder_04 import compile_to_suit


def str2int(x):
    if x.startswith("0x"):
        return int(x, 16)
    else:
        return int(x)


def sha256_from_file(filepath):
    sha256 = hashlib.sha256()
    sha256.update(open(filepath, "rb").read())
    return sha256.digest()


def parse_arguments():
    parser = argparse.ArgumentParser(
        formatter_class=argparse.ArgumentDefaultsHelpFormatter)
    parser.add_argument('--template', '-t', help='Manifest template file path')
    parser.add_argument('--urlroot', '-u', help='')
    parser.add_argument('--offsets', '-O', help='')
    parser.add_argument('--seqnr', '-s',
                        help='Sequence number of the manifest')
    parser.add_argument('--output', '-o', nargs='?',
                        help='Manifest output binary file path')
    parser.add_argument('--uuid-vendor', '-V',
                        help='Manifest vendor uuid')
    parser.add_argument('--uuid-class', '-C',
                        help='Manifest class uuid')
    parser.add_argument('slotfiles', nargs=2,
                        help='The list of slot file paths')
    return parser.parse_args()


def main(args):
    uuid_vendor = uuid.uuid5(uuid.NAMESPACE_DNS, args.uuid_vendor)
    uuid_class = uuid.uuid5(uuid_vendor, args.uuid_class)
    with open(args.template, 'r') as f:
        template = json.load(f)

    template["sequence-number"] = int(args.seqnr)
    template["conditions"] = [
            {"condition-vendor-id": uuid_vendor.hex},
            {"condition-class-id": uuid_class.hex},
        ]

    offsets = [str2int(offset) for offset in args.offsets.split(",")]

    for slot, slotfile in enumerate(args.slotfiles):
        filename = slotfile
        size = os.path.getsize(filename)
        uri = os.path.join(args.urlroot, os.path.basename(filename))
        offset = offsets[slot]

        _image_slot = template["components"][0]["images"][slot]
        _image_slot.update({
            "file": filename,
            "uri": uri,
            "size": size,
            "digest": sha256_from_file(slotfile),
            })

        _image_slot["conditions"][0]["condition-component-offset"] = offset
        _image_slot["file"] = filename

    result = compile_to_suit(template)
    if args.output is not None:
        with open(args.output, 'wb') as f:
            f.write(result)
    else:
        print(result)


if __name__ == "__main__":
    _args = parse_arguments()
    main(_args)

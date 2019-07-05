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

import click

from suit_manifest_encoder_04 import compile_to_suit


def str2int(x):
    if x.startswith("0x"):
        return int(x, 16)
    else:
        return x


def sha256_from_file(filepath):
    sha256 = hashlib.sha256()
    sha256.update(open(filepath, "rb").read())
    return sha256.digest()


@click.command()
@click.option("--template", "-t", required=True, type=click.File())
@click.option("--urlroot", "-u", required=True, type=click.STRING)
@click.option("--offsets", "-O", required=True, type=click.STRING)
@click.option("--seqnr", "-s", required=True, type=click.INT)
@click.option("--output", "-o", type=click.File(mode="wb"))
@click.option("--uuid-vendor", "-V", required=True)
@click.option("--uuid-class", "-C",  required=True)
@click.option("--keyfile", "-K",  required=False, type=click.File())
@click.argument("slotfiles", nargs=2, type=click.Path())
def main(template, urlroot, offsets, slotfiles, output, seqnr, uuid_vendor,
         uuid_class, keyfile):

    uuid_vendor = uuid.uuid5(uuid.NAMESPACE_DNS, uuid_vendor)
    uuid_class = uuid.uuid5(uuid_vendor, uuid_class)
    template = json.load(template)
    slotfiles = list(slotfiles)

    template["sequence-number"] = seqnr
    template["conditions"] = [
            {"condition-vendor-id": uuid_vendor.hex},
            {"condition-class-id": uuid_class.hex},
        ]

    offsets = offsets.split(",")
    offsets = [str2int(x) for x in offsets]

    for slot, slotfile in enumerate(slotfiles):
        filename = slotfile
        size = os.path.getsize(filename)
        uri = os.path.join(urlroot, os.path.basename(filename))
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
    if output:
        output.write(result)
    else:
        print(result)


if __name__ == "__main__":
    main()

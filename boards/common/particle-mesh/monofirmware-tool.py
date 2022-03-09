#!/usr/bin/env python3

"""Verify that a pair of .bin/.elf has the particle_monofirmware_module_info at
the right place. If there is a particle_monofirmware_checksum symbol, verify
that the module_info says the firmware ends at it, and populate the
checksum. Otherwise, append the checksum and set the end-of-firmware field
accordingly."""

import os
import sys
import zlib
import subprocess
import argparse

expected_romstart = 0x30000
expected_moduleinfo_position = expected_romstart + 0x200
expected_moduleinfo_size = 0x18
# offsetof(module_end_address, module_info_t)
end_field_offset = 4
end_field_start = expected_moduleinfo_position - expected_romstart + end_field_offset
end_field_end = end_field_start + 4


def check(condition, message):
    if not condition:
        print(message, file=sys.stderr)
        sys.exit(1)


p = argparse.ArgumentParser(description=__doc__)
p.add_argument("elf_in", help="Program in ELF form")
p.add_argument("bin_in", help="objdump'd ROM binary of elf_in")
p.add_argument("bin_out", help="File to write the modified bin to")
args = p.parse_args()

symbols = subprocess.check_output([
    os.environ.get('NM', 'nm'),
    "--format=posix",
    "--print-size",
    "--defined-only",
    args.elf_in,
    ])
symbols = [line.split(' ') for line in symbols.decode('ascii').split('\n') if line]
position = {s[0]: int(s[2], 16) for s in symbols}
size = {s[0]: int(s[3], 16) for s in symbols if s[3]}
symtype = {s[0]: s[1] for s in symbols}

romstart = min(pos for (name, pos) in position.items() if symtype[name] not in '?A')

check(romstart == expected_romstart, "Defined symbols do not start at %#x" % expected_romstart)

check(position.get('particle_monofirmware_module_info') == expected_moduleinfo_position,
      """Struct particle_monofirmware_module_info not found at %#x but on %#x,
      adjust the size of particle_monofirmware_padding to match""" %
      (expected_moduleinfo_position, position.get('particle_monofirmware_module_info')))
check(size.get('particle_monofirmware_module_info') == expected_moduleinfo_size,
      "Struct particle_monofirmware_module_info not of expected size %d" % expected_moduleinfo_size)

binary = bytearray(open(args.bin_in, 'rb').read())

indicated_start = int.from_bytes(binary[expected_moduleinfo_position - romstart:][:4], 'little')
check(romstart == indicated_start,
      "particle_monofirmware_module_info.module_start does not point to the module start")

if 'particle_monofirmware_checksum' in position:
    print("monifirmware-tool: Checksum symbol found, populating it.")
    check(size.get('particle_monofirmware_checksum') == 4, "Checksum not of expected size 4")

    checksum_until = position['particle_monofirmware_checksum'] - romstart

    indicated_end = int.from_bytes(binary[end_field_start:end_field_end], 'little')
    check(indicated_end == position['particle_monofirmware_checksum'],
          "particle_monofirmware_module_info.module_end does not point to particle_monofirmware_checksum")
else:
    print("monifirmware-tool: No checksum symbol found, appending the checksum to the binary.")
    checksum_until = len(binary)

    # Populating the length: When the whole binary (including the rom
    # initialization block) is checksummed, the end address can't be populated
    # from C because that would require evaluating `&_etext + (&_erelocate -
    # &_srelocate)` in advance
    binary[end_field_start:end_field_end] = (checksum_until + romstart).to_bytes(4, 'little')

checksum = zlib.crc32(binary[:checksum_until]).to_bytes(4, 'big')
binary[checksum_until:checksum_until + 4] = checksum

with open(args.bin_out, "wb") as o:
    o.write(binary)

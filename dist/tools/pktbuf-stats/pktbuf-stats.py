#! /usr/bin/env python3
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#
# @author   Martine Lenders <m.lenders@fu-berlin.de>

"""
Script to parse the output of the `pktbuf` (provided by the `gnrc_pktbuf_cmd`
pseudo-module) command and the `gnrc_pktbuf_stats()` function.
"""

import argparse
import collections
import pprint
import re
import subprocess
import os
import socket
import struct
import sys

NETTYPES = {}
PKTSNIP_STRUCT = {
        "name": "gnrc_pktsnip_t",
        "endianness": "<"
    }
NETTYPE_STRUCTS = {
    "GNRC_NETTYPE_NETIF": {
        "name": "gnrc_netif_hdr_t",
        "endianness": "<",
        "subparser": 'gnrc_netif_parser'
    },
    "GNRC_NETTYPE_IPV6": {
        "name": "ipv6_hdr_t",
        "endianness": "!",
        "subparser": "ipv6_hdr_parser"
    },
    "GNRC_NETTYPE_IPV6_EXT": {
        "name": "ipv6_ext_t",
        "endianness": "!",
    },
    "GNRC_NETTYPE_ICMPV6": {
        "name": "icmpv6_hdr_t",
        "endianness": "!",
    },
    "GNRC_NETTYPE_UDP": {
        "name": "udp_hdr_t",
        "endianness": "!",
    },
}
S = {
    1: "B",
    2: "H",
    4: "L",
    8: "Q",
}
PROTNUMS = {n: name[8:] for name, n in vars(socket).items()
            if name.startswith("IPPROTO")}


class NoDebugSymbolsError(Exception):
    """
    Error to convey that no debugging symbols were found in the provided ELF
    file
    """
    pass


class LineNotFound(Exception):
    """
    Error to convey that GDB output was not found
    """
    pass


class InconsistentPktbuf(Exception):
    """
    Error to indicate inconsistencies in the packet buffer stats
    """
    pass


def _exec_gdb(elffile, commands):
    ex = ["gdb", elffile]
    for c in commands:
        ex.extend(["-ex", c])
    ex.extend(["-ex", "quit"])
    p = subprocess.Popen(ex, stderr=subprocess.DEVNULL, stdout=subprocess.PIPE,
                         env={"CLANG": "en_US"})
    return p.stdout


def _check_debug_symbols(line):
    if re.search(r"No debugging symbols found in ", line):
        raise NoDebugSymbolsError(line.strip().strip("()"))


def _parse_struct_str(members_str, members=None, level=0):
    in_member_name = re.search(r"^\s+\w", members_str) is not None
    for char in members_str:
        if char == '{':
            level += 1
            if level == 1:
                in_member_name = True
        elif char == '}':
            level -= 1
        elif level == 1:
            if in_member_name and re.match(r"\w", char):
                if members is None:
                    members = [char]
                else:
                    members[-1] += char
            elif char == ',':
                members.append('')
                in_member_name = True
            elif re.match(r"\s", char) is None:
                in_member_name = False
    return members, level


def _parse_struct_print(line):
    m = re.match(r"^\$1 = (.*)?$", line)
    if m is not None:
        return _parse_struct_str(m.group(1))
    return None, 0


def get_struct_size(elffile, struct):
    """
    Determines the size in bytes of a struct from a given ELF file.

    Parameters:
        elffile (string-like): Path to an ELF file.
        struct (string-like): Type name of a struct.

    Returns:
        int: Size of the struct in bytes.

    Raises:
        LineNotFound: When the line with the size of the struct can not be
        found in the GDB output.
    """
    for line in _exec_gdb(elffile,
                          ["print sizeof(*(({}*)_pktbuf))"
                           .format(struct)]).readlines():
        line = line.decode()
        _check_debug_symbols(line)
        m = re.match(r"^\$1 = (\d+)$", line)
        if m is not None:
            return int(m.group(1))
    raise LineNotFound("Unable to find line of size of {}"
                       .format(struct))


def get_struct_members(elffile, struct):
    """
    Determines the members of a struct from a given ELF file.

    Parameters:
        elffile (string-like): Path to an ELF file.
        struct (string-like): Type name of a struct.

    Returns:
        list: Names of the members of the struct as list of strings.

    Raises:
        LineNotFound: When the output with the members of the struct can not be
        found in the GDB output.
    """
    level = 0
    struct_members = None
    for line in _exec_gdb(elffile,
                          ["print/d *({}*)_pktbuf"
                           .format(struct)]).readlines():
        assert (level > 0) or (struct_members is None)
        line = line.decode()
        _check_debug_symbols(line)
        if level > 0:
            struct_members, level = _parse_struct_str(
                line, struct_members, level
            )
        else:
            struct_members, level = _parse_struct_print(line)
        if struct_members is not None and (level == 0):
            return struct_members
    raise LineNotFound("Unable to find line of {} members".format(struct))


def get_struct_member_size(elffile, struct, member):
    """
    Determines the size in bytes of a member of a struct from a given ELF file.

    Parameters:
        elffile (string-like): Path to an ELF file.
        struct (string-like): Type name of a struct.
        member (string-like): Name of a member of the given struct.

    Returns:
        int: Size of the member of the struct in bytes.

    Raises:
        LineNotFound: When the line with the size of the member of the struct
        can not be found in the GDB output.
    """
    for line in _exec_gdb(elffile,
                          ["print sizeof((({}*)_pktbuf)->{})"
                           .format(struct, member)]).readlines():
        line = line.decode()
        _check_debug_symbols(line)
        m = re.match(r"^\$1 = (\d+)$", line)
        if m is not None:
            return int(m.group(1))
    raise LineNotFound("Unable to find line of size of {} in {}"
                       .format(member, struct))


def get_struct(elffile, struct_name):
    """
    Gets struct definition dictionary of a struct from a given ELF file.

    Parameters:
        elffile (string-like): Path to an ELF file.
        struct_name (string-like): Type name of a struct.

    Returns:
        dict: Dictionary containing
            - "name": the type name of the struct as a string.
            - "endianness": Endianness of the struct in `struct` notation as
              string.
            - "subparser" (optional): Name of a subparser function as string,
              to parse the byte string to human readable output.
            - "members": OrderedDict of the members of the struct with the name
              of the member as key and the size of the member in bytes as
              value. The order of the keys is in accordance with the member in
              memory.
            - "size": Total size of the struct (including padding) in bytes.

    Raises:
        LineNotFound: When lines regarding the struct can not be found in the
        GDB output
        NotImplementedError: If no base information for the struct can be
        found. Extend NETTYPE_STRUCTS in this case accordingly.
    """
    global PKTSNIP_STRUCT, NETTYPE_STRUCTS
    d = None
    if struct_name == PKTSNIP_STRUCT["name"]:
        d = PKTSNIP_STRUCT
    else:
        for v in NETTYPE_STRUCTS.values():
            if struct_name == v["name"]:
                d = v
    if d is None:
        raise NotImplementedError("Parsing of struct {} not implemented"
                                  .format(struct_name))
    if "members" not in d:
        d["members"] = collections.OrderedDict([
            (m, get_struct_member_size(elffile, struct_name, m))
            for m in get_struct_members(elffile, struct_name)
        ])
        d["size"] = get_struct_size(elffile, struct_name)
    return d


def to_nettype(elffile, number):
    """
    Gets a gnrc_nettype name by its number.

    Parameters:
        elffile (string-like): Path to an ELF file.
        number (number-like): Integer representation of a gnrc_nettype.

    Returns:
        str: Name of the gnrc_nettype associated with the given number.

    Raises:
        LineNotFound: If the nettype can not be found in the GDB output.
    """
    if number in NETTYPES:
        return NETTYPES[number]
    for line in _exec_gdb(elffile,
                          ["print (gnrc_nettype_t){:d}"
                           .format(number)]).readlines():
        line = line.decode()
        _check_debug_symbols(line)
        m = re.match(r"^\$1 = ([0-9A-Z_]+)$", line)
        if m is not None:
            NETTYPES[number] = m.group(1)
            return m.group(1)
    raise LineNotFound("Unable to find line of nettype")


def parse_struct(struct_dict, byts):
    """
    Parses a struct from a given bytes chunk

    Parameters:
        struct_dict (dict): a struct descriptor dictionary as returned by
        get_struct()
        byts (dict): bytes to parse as a chunk descriptor dictionary as
        provided in the result of parse_hexdump()

    Returns:
        dict: dictionary containing the name of the struct members as keys
        and their value as value. The "padding" key contains a byte string of
        the data padding the struct.
    """
    if struct_dict["size"] > len(byts["data"]):
        return None
    size = sum(s for s in struct_dict["members"].values())
    vals = struct.unpack(
        struct_dict["endianness"] +
        "".join(S.get(size, '{}s'.format(size))
                for size in struct_dict["members"].values()),
        byts["data"][:size]
    )
    res = {k: vals[i] for i, k in enumerate(struct_dict["members"])}
    res["padding"] = byts["data"][size:byts["size"]]
    return res


def parse_hexdump(dump):
    """
    Generator to parse the packet buffer dump into a processable dictionary

    Parameters:
        dump (string): The output of the `gnrcp_pktbuf_stats()` function / the
        `pktbuf` command.
        Outputs of multiples invocation are possible to be parsed. They each
        will be outputted one by one when iterating over the generator.

    Returns:
        dict: A dictionary describing the state of the packet buffer:
            - "line" (int): The summary line of the packet buffer.
            - "first_byte" (int): address of the first byte of the packet
              buffer.
            - "last_byte" (int): address of the last byte of the packet buffer
            - "size" (int): size in bytes of the packet buffer.
            - "last_byte_used" (int): the maximum number of byte used by the
              packet buffer at that point in time.
            - "segments" (list): list of segment dictionaries marked in the
              packet buffer. There are two types of segments "unused" and
              "chunk". "unused" segments are not in use and have the following
              members:
                - "type" (str): Marking it as "unused" segment.
                - "name" (str): Human-readable name for the unused segment.
                - "start" (int): Start address of the segment.
                - "size" (int): Size in bytes of the segment.
                - "next" (int, NoneType): Start address of the next "unused"
                  segment in the packet buffer. `None` if there is no next
                  "unused" segment.
              "chunk" segments are in use and have the following members:
                - "type" (str): Marking it as "chunk" segment.
                - "name" (str): Human-readable name for the chunk.
                - "start" (int): Start address of the segment.
                - "size" (int): Size in bytes of the segment.
                - "content" (list): List of dictionaries of the content of the
                  chunk. May contain raw data ("type": "raw") that can be
                  parsed with parse_struct().
    """
    pktbuf = {"segments": []}
    number_of_bytes = 0
    current_bytes = None
    chunk_bytes = 0
    for line in dump.readlines():
        if "size" not in pktbuf:
            m = re.search(r"packet buffer: first byte: 0x([0-9A-Fa-f]+), "
                          r"last byte: 0x([0-9A-Fa-f]+) \(size: +(\d+)\)",
                          line)
            if m is not None:
                pktbuf["line"] = line.strip()
                pktbuf["first_byte"] = int(m.group(1), base=16)
                pktbuf["last_byte"] = int(m.group(2), base=16)
                pktbuf["size"] = int(m.group(3))
        else:
            if number_of_bytes >= pktbuf["size"]:
                res = pktbuf
                pktbuf = {"segments": []}
                number_of_bytes = 0
                yield res
            m = re.search(r"  position of last byte used: (\d+)", line)
            if m is not None:
                pktbuf["last_byte_used"] = int(m.group(1))
                continue
            m = re.search(r"~ unused: 0x([0-9A-Fa-f]+) "
                          r"\(next: (0x([0-9A-Fa-f]+)|\(nil\)), "
                          r"size: +(\d+)\) ~", line)
            if m is not None:
                nxt = m.group(3)
                size = int(m.group(4))
                start = int(m.group(1), base=16)
                pktbuf["segments"].append({
                    "type": "unused",
                    "name": "unused 0x{:x}".format(start),
                    "start": start,
                    "size": size,
                    "next": int(nxt, base=16) if nxt is not None else None,
                })
                number_of_bytes += size
                continue
            if current_bytes is None:
                m = re.search(r"=+ chunk +(\d+) "
                              r"\(0x([0-9A-Fa-f]+) size: +(\d+)\) =+", line)
                if m is not None:
                    current_bytes = bytearray(b"")
                    chunk_bytes = int(m.group(3))
                    start = int(m.group(2), base=16)
                    pktbuf["segments"].append({
                        "type": "chunk",
                        "name": "chunk {}".format(m.group(1)),
                        "start": start,
                        "size": chunk_bytes,
                        "content": [{"data": current_bytes, "type": "raw",
                                     "size": chunk_bytes, "start": start}],
                    })
                    number_of_bytes += chunk_bytes
                    continue
            if (len(pktbuf["segments"])) and \
               (pktbuf["segments"][-1]["type"] == "chunk"):
                m = re.search(r"[0-9A-Fa-f]{8}(.*)$", line)
                if m is not None:
                    byts = bytes.fromhex(
                        "".join(re.findall("  ([0-9A-Fa-f]{2})", m.group(1)))
                    )
                    current_bytes.extend(byts)
                    chunk_bytes -= len(byts)
                    if not chunk_bytes:
                        current_bytes = None
                    continue
    if number_of_bytes:
        yield pktbuf


def empty_pktbuf(pktbuf):
    """
    Checks if the packet buffer is empty.

    Parameters:
        pktbuf (dict): packet buffer descriptor as returned by parse_hexdump().

    Returns:
        True, if packet buffer is empty.
        False, if packet buffer is not empty.
    """
    return (len(pktbuf["segments"]) == 1) and \
           (pktbuf["first_byte"] == pktbuf["segments"][0]["start"]) and \
           ((pktbuf["last_byte"] - pktbuf["first_byte"]) ==
            pktbuf["segments"][0]["size"])


def in_pktbuf(pktbuf, addr):
    """
    Checks if a given address is in the packet buffer.

    Parameters:
        pktbuf (dict): packet buffer descriptor as returned by parse_hexdump().
        addr (int): Address to check.

    Returns:
        True, if the address is in the packet buffer.
        False, if the address is not in the packet buffer.
    """
    return addr is not None and \
        (pktbuf["first_byte"] <= addr <
         (pktbuf["first_byte"] + pktbuf["size"]))


def in_segment(segment, addr):
    """
    Checks if a given address is in the packet buffer segment.

    Parameters:
        segment (dict): packet buffer segment descriptor as they exist in the
        result of parse_hexdump().
        addr (int): Address to check.

    Returns:
        True, if the address is in the packet buffer segment.
        False, if the address is not in the packet buffer segment.
    """
    return addr is not None and \
        (segment["start"] <= addr < (segment["start"] + segment["size"]))


def _is_file(f):
    if not os.path.exists(f):
        raise ValueError("File {} does not exist".format(f))
    return f


def _round_up_to_8(number):
    return int(((number + 7) // 8) * 8)


def _tidyup_pktsnip(elffile, pktsnip):
    size = sum(s for s in PKTSNIP_STRUCT["members"].values())
    pktsnip["type"] = to_nettype(elffile, pktsnip["type"])
    pktsnip["next"] = {"addr": pktsnip["next"]}
    pktsnip["data"] = {"addr": pktsnip["data"]}
    pktsnip["padding"] = pktsnip["padding"][
        :(_round_up_to_8(PKTSNIP_STRUCT["size"]) - size)
    ]


def identify_pktsnip(elffile, content, pktbuf):
    """
    Tries to identify a pktsnip from a given chunk content descriptor.

    Parameters:
        elffile (str): Path to an ELF file.
        content (dict): chunk content descriptor as returned in the result of
        parse_hexdump().
        pktbuf (dict): packet buffer descriptor as returned by parse_hexdump().

    Returns:
        dict: a dictionary describing the packet snip struct as returned by
        parse_struct() and brought into a human readable form.
        NoneType: None when no pktsnip can be identified.
    """
    pktsnip = None
    idx = None
    # find first fitting raw content marker
    for i, c in enumerate(content):
        if (c["size"] >= PKTSNIP_STRUCT["size"]) and (c["type"] == "raw"):
            idx = i
            break
    if idx is None:
        return None
    while (pktsnip is None) and (idx < len(content)):
        c = content[idx]
        pktsnip = parse_struct(PKTSNIP_STRUCT, c)
        if (pktsnip is not None) and \
           ((pktsnip["next"] == 0) or in_pktbuf(pktbuf, pktsnip["next"])) and \
           ((pktsnip["data"] == 0) or in_pktbuf(pktbuf, pktsnip["data"])):
            size = _round_up_to_8(PKTSNIP_STRUCT["size"])
            new_data = c["data"][size:]
            new_size = c["size"] - size
            c["type"] = "gnrc_pktsnip"
            c["data"] = pktsnip
            c["size"] = size
            _tidyup_pktsnip(elffile, pktsnip)
            if not len(new_data):
                # end of content
                break
        else:
            pktsnip = None
            size = 8
            new_data = c["data"][size:]
            new_size = c["size"] - size
            if not len(new_data):
                # end of content
                break
            elif (idx > 0) and (content[idx - 1]["type"] == "raw"):
                prev = idx - 1
                # merge new raw with previous raw
                content[prev]["data"] += c["data"][:size]
                content[prev]["size"] += size
                c["start"] = content[prev]["start"] + content[prev]["size"]
                c["data"] = new_data
                c["size"] -= size
                continue
            else:
                c["data"] = new_data
                c["size"] = size
        content.insert(idx + 1, {
            "data": new_data,
            "size": new_size,
            "type": "raw",
            "start": c["start"] + size,
        })
        idx += 1
    return pktsnip


def identify_struct(elffile, content, pktsnip):
    """
    Tries to identify a struct in the given content via the provided pktsnip.

    Parameters:
        elffile: Path to an ELF file.
        content (dict): chunk content descriptor as returned in the result of
        parse_hexdump().
        pktsnip: Packet snip descriptor, pointing its data pointer to content.

    Returns:
        dict: struct descriptor as returned by parse_struct() or by a provided
        sub-parser for that struct in NETTYPE_STRUCTS.
    """
    global NETTYPE_STRUCTS
    nettype = pktsnip["type"]
    struct_dict = get_struct(elffile, NETTYPE_STRUCTS[nettype]["name"])
    idx = None
    offset = pktsnip["data"]["addr"] - content[0]["start"]
    # find first fitting raw content marker
    for i, c in enumerate(content):
        if (c["size"] >= struct_dict["size"]) and (c["type"] == "raw") and \
           ((c["start"] + c["size"]) > pktsnip["data"]["addr"]):
            idx = i
            offset = pktsnip["data"]["addr"] - content[idx]["start"]
            break
        offset -= c["size"]
        if (offset < 0):
            # something is broken or already parsed => ignore this
            return
    if idx is None:
        # something is broken or already parsed => ignore this
        return
    c = content[idx]
    if c["type"] not in ["raw", nettype.lower()]:
        raise InconsistentPktbuf("pointer 0x{:x} points to two distinct types"
                                 .format(pktsnip["data"]["addr"]))
    size = _round_up_to_8(pktsnip["size"])
    if offset > 0:
        new_data = c["data"][offset:]
        new_size = c["size"] - offset
        new_start = c["start"] + offset
        content.insert(idx, {
            "start": c["start"],
            "type": "raw",
            "data": c["data"][:offset],
            "size": offset,
        })
        c["data"] = new_data
        c["size"] = new_size
        c["start"] = new_start
    if size < c["size"]:
        new_data = c["data"][size:]
        new_size = c["size"] - size
        new_start = c["start"] + size
        c["data"] = c["data"][:size]
        c["size"] = size
        content.insert(idx + 1, {
            "start": new_start,
            "type": "raw",
            "data": new_data,
            "size": new_size,
        })
    c["type"] = nettype.lower()
    if "subparser" in struct_dict:
        subparser = struct_dict["subparser"]
        if isinstance(subparser, str):
            subparser = globals()[subparser]
        c["data"] = subparser(parse_struct(struct_dict, c))
    else:
        c["data"] = parse_struct(struct_dict, c)


def main():
    global PKTSNIP_STRUCT
    args_parser = argparse.ArgumentParser(
             description="Analyze `pktbuf` command output"
        )
    args_parser.add_argument("elffile", type=_is_file,
                             help="The elffile of the application `pktbuf` "
                                  "was executed in")
    args_parser.add_argument("dump", type=argparse.FileType("r"),
                             default=sys.stdin,
                             help="Output of one or more `pktbuf` executions "
                                  "(default: stdin)")
    args = args_parser.parse_args()
    get_struct(args.elffile, PKTSNIP_STRUCT["name"])
    for i, pktbuf in enumerate(parse_hexdump(args.dump), 1):
        if empty_pktbuf(pktbuf):
            print("pktbuf output {} shows an empty pktbuf".format(i))
            pprint.pprint(pktbuf)
            continue
        pktsnip = None
        while True:
            for nr, segment in enumerate(pktbuf["segments"]):
                if segment["type"] != "chunk":
                    continue
                pktsnip = identify_pktsnip(args.elffile, segment["content"],
                                           pktbuf)
                if pktsnip is not None:
                    break
            if pktsnip is None:
                break
            for nr, segment in enumerate(pktbuf["segments"]):
                if in_segment(segment, pktsnip["next"]["addr"]):
                    pktsnip["next"]["name"] = {
                        "segment": segment["name"],
                        "offset": pktsnip["next"]["addr"] - segment["start"]
                    }
                if in_segment(segment, pktsnip["data"]["addr"]):
                    if segment["type"] == "chunk":
                        identify_struct(args.elffile, segment["content"],
                                        pktsnip)
                    pktsnip["data"]["name"] = {
                        "segment": segment["name"],
                        "offset": pktsnip["data"]["addr"] - segment["start"]
                    }
        print("pktbuf output {}".format(i))
        pprint.pprint(pktbuf)


# Subparsers
def gnrc_netif_parser(data_dict):
    """
    Sub-parser for GNRC netif headers.
    """
    src_len = data_dict["src_l2addr_len"]
    dst_len = data_dict["dst_l2addr_len"]
    if (src_len > 0) and (len(data_dict["padding"]) >= src_len):
        data_dict["src_l2addr"] = struct.unpack(
            "!{}s".format(src_len),
            data_dict["padding"][:src_len]
        )
        data_dict["padding"] = data_dict["padding"][src_len:]
    else:
        data_dict["src_l2addr"] = b""
    if (dst_len > 0) and (len(data_dict["padding"]) >= dst_len):
        data_dict["dst_l2addr"] = struct.unpack(
            "!{}s".format(dst_len),
            data_dict["padding"][:dst_len]
        )
        data_dict["padding"] = data_dict["padding"][dst_len:]
    else:
        data_dict["dst_l2addr"] = b""
    return data_dict


def ipv6_hdr_parser(data_dict):
    """
    Sub-parser for IPv6 headers.
    """
    data_dict["nh"] = PROTNUMS.get(data_dict["nh"], data_dict["nh"])
    data_dict["version"] = data_dict["v_tc_fl"] >> 28
    data_dict["tc"] = {"ecn": (data_dict["v_tc_fl"] & 0x0c000000) >> 26,
                       "dscp": (data_dict["v_tc_fl"] & 0x03f00000) >> 20}
    data_dict["fl"] = data_dict["v_tc_fl"] & 0x000fffff
    del data_dict["v_tc_fl"]
    return data_dict


if __name__ == "__main__":
    main()

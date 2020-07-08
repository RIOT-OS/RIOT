# Copyright (C) 2019-20 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
GNRC-related shell interactions

Defines GNRC-related shell command interactions
"""

import re

from riotctrl.shell import ShellInteraction, ShellInteractionParser


# ==== Parsers ====

class GNRCICMPv6EchoParser(ShellInteractionParser):
    def __init__(self):
        self.c_reply = re.compile(r"\d+ bytes from "
                                  r"(?P<source>[0-9a-f:]+(%\S+)?): "
                                  r"icmp_seq=(?P<seq>\d+) ttl=(?P<ttl>\d+)"
                                  r"( rssi=(?P<rssi>-?\d+) dBm)?"
                                  r"( time=(?P<rtt>\d+.\d+) ms)?"
                                  r"(?P<dup> \(DUP\))?")
        self.c_stats = re.compile(r"(?P<tx>\d+) packets transmitted, "
                                  r"(?P<rx>\d+) packets received, "
                                  r"((?P<dup>\d+) duplicates, )?"
                                  r"(?P<packet_loss>\d+)% packet loss")
        self.c_rtts = re.compile(r"round-trip min/avg/max = (?P<min>\d+.\d+)/"
                                 r"(?P<avg>\d+.\d+)/(?P<max>\d+.\d+) ms")

    @staticmethod
    def _add_reply(res, reply):
        reply["seq"] = int(reply["seq"])
        reply["ttl"] = int(reply["ttl"])
        if reply.get("rtt") is not None:
            reply["rtt"] = float(reply["rtt"])
        else:
            reply.pop("rtt", None)
        if reply.get("dup"):
            reply["dup"] = True
        else:
            reply.pop("dup", None)
        if reply.get("rssi") is not None:
            reply["rssi"] = int(reply["rssi"])
        else:
            reply.pop("rssi", None)
        if "replies" in res:
            res["replies"].append(reply)
        else:
            res["replies"] = [reply]

    @staticmethod
    def _set_stats(res, stats):
        stats["packet_loss"] = int(stats["packet_loss"])
        stats["rx"] = int(stats["rx"])
        stats["tx"] = int(stats["tx"])
        if stats.get("dup") is not None:
            stats["dup"] = int(stats["dup"])
        else:
            stats.pop("dup")
        res["stats"] = stats

    @staticmethod
    def _set_rtts(res, rtts):
        rtts["min"] = float(rtts["min"])
        rtts["avg"] = float(rtts["avg"])
        rtts["max"] = float(rtts["max"])
        res["rtts"] = rtts

    def parse(self, cmd_output):
        """
        Parses output of GNRCIPv6NIB::nib_neigh_show()

        >>> parser = GNRCICMPv6EchoParser()
        >>> res = parser.parse(
        ...     "12 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=0 ttl=64 rssi=-34 dBm time=8.839 ms\\n"
        ...     "12 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=1 ttl=64 rssi=-34 dBm time=6.925 ms\\n"
        ...     "12 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=2 ttl=64 rssi=-34 dBm time=7.885 ms\\n"
        ...     "--- fe80::385d:f965:106b:1114 PING statistics ---\\n"
        ...     "3 packets transmitted, 3 packets received, 0% packet loss\\n"
        ...     "round-trip min/avg/max = 6.925/7.883/8.839 ms\\n")
        >>> sorted(res)
        ['replies', 'rtts', 'stats']
        >>> len(res["replies"])
        3
        >>> sorted(res["replies"][0])
        ['rssi', 'rtt', 'seq', 'source', 'ttl']
        >>> res["replies"][0]["source"]
        'fe80::385d:f965:106b:1114%6'
        >>> res["replies"][0]["seq"]
        0
        >>> res["replies"][0]["ttl"]
        64
        >>> res["replies"][0]["rssi"]
        -34
        >>> res["replies"][0]["rtt"]
        8.839
        >>> sorted(res["stats"])
        ['packet_loss', 'rx', 'tx']
        >>> res["stats"]["tx"]
        3
        >>> res["stats"]["rx"]
        3
        >>> res["stats"]["packet_loss"]
        0
        >>> sorted(res["rtts"])
        ['avg', 'max', 'min']
        >>> res["rtts"]["min"]
        6.925
        >>> res["rtts"]["avg"]
        7.883
        >>> res["rtts"]["max"]
        8.839
        """
        res = {}
        for line in cmd_output.splitlines():
            if "stats" not in res:  # If final stats were not found yet
                m = self.c_reply.match(line)
                if m is not None:
                    self._add_reply(res, m.groupdict())
                    continue
                m = self.c_stats.match(line)
                if m is not None:
                    self._set_stats(res, m.groupdict())
                    continue
            else:
                m = self.c_rtts.match(line)
                if m is not None:
                    self._set_rtts(res, m.groupdict())
        return res


class GNRCPktbufStatsResults(dict):
    def is_empty(self):
        """
        Returns true if the packet buffer stats indicate that the packet buffer
        is empty
        """
        if "first_byte" not in self or \
           "first_unused" not in self or \
           "start" not in self["first_unused"] or \
           "size" not in self["first_unused"]:
            raise ValueError("{} has no items 'first_byte' or 'first_unused' "
                             "or 'first_unused' has no items 'start' or 'size'"
                             .format(self))
        else:
            return (self["first_byte"] == self["first_unused"]["start"]) and \
                   (self["size"] == self["first_unused"]["size"])

    def fullest_capacity(self):
        """
        Returns the packet buffer usage at its fullest capacity when the
        command was called
        """
        if "last_byte_used" not in self or "size" not in self:
            raise ValueError("{} has no items 'last_byte_used' or 'size'"
                             .format(self))
        else:
            return self["last_byte_used"] / self["size"]


class GNRCPktbufStatsParser(ShellInteractionParser):
    def __init__(self):
        self.c_init1 = re.compile(
            r"packet buffer: first byte: 0x(?P<first_byte>[0-9A-Fa-f]+), "
            r"last byte: 0x(?P<last_byte>[0-9A-Fa-f]+) "
            r"\(size: +(?P<size>\d+)\)"
        )
        self.c_init2 = re.compile(r"  position of last byte used: (\d+)")
        self.c_unused = re.compile(
            r"~ unused: 0x(?P<start>[0-9A-Fa-f]+) "
            # flake reports r'\(', r'\)' as invalid escape sequence
            # false positively
            r"\(next: ""(0x(?P<next>[0-9A-Fa-f]+)|\(nil\)), "   # noqa W605
            r"size: +(?P<size>\d+)\) ~"
        )

    @staticmethod
    def _init_res(first_byte, last_byte, size):
        return GNRCPktbufStatsResults((
            ("first_byte", int(first_byte, base=16)),
            ("last_byte", int(last_byte, base=16)),
            ("size", int(size)),
        ))

    @staticmethod
    def _set_last_byte_used(res, last_byte_used):
        res["last_byte_used"] = int(last_byte_used)

    @staticmethod
    def _set_first_unused(res, first_unused):
        res["first_unused"] = {
            "start": int(first_unused["start"], base=16)
        }
        if first_unused["next"] is not None:
            res["first_unused"]["next"] = int(first_unused["next"], base=16)
        if "next" not in res["first_unused"] or \
           not res["first_unused"]["next"]:
            res["first_unused"]["next"] = None
        res["first_unused"]["size"] = int(first_unused["size"])

    def parse(self, cmd_output):
        """
        Parses output of GNRCPktbufStats::pktbuf_stats()

        >>> parser = GNRCPktbufStatsParser()
        >>> res = parser.parse(
        ...     "packet buffer: first byte: 0x20001f54, last byte: 0x20003f54 "
        ...         "(size: 8192)\\n"
        ...     "  position of last byte used: 312\\n"
        ...     "~ unused: 0x20001f54 (next: (nil), size: 8192) ~\\n")
        >>> sorted(res)
        ['first_byte', 'first_unused', 'last_byte', 'last_byte_used', 'size']
        >>> res.is_empty()
        True
        >>> hex(res["first_byte"])
        '0x20001f54'
        >>> hex(res["last_byte"])
        '0x20003f54'
        >>> res["size"]
        8192
        >>> res["last_byte_used"]
        312
        >>> res.fullest_capacity() <= (res["last_byte_used"] / res["size"])
        True
        >>> sorted(res["first_unused"])
        ['next', 'size', 'start']
        >>> hex(res["first_unused"]["start"])
        '0x20001f54'
        >>> res["first_unused"]["next"]     # has no next according to dump
        >>> res["first_unused"]["size"]
        8192
        """
        res = None
        for line in cmd_output.splitlines():
            if res is None:
                m = self.c_init1.match(line)
                if m is not None:
                    res = self._init_res(**m.groupdict())
                # no sense in further parsing if we did not find the first line
                # yet. If we found it just continue parsing with next line
                continue
            elif "last_byte_used" not in res:
                m = self.c_init2.match(line)
                if m is not None:
                    self._set_last_byte_used(res, m.group(1))
                    continue
            elif "first_unused" not in res:
                m = self.c_unused.match(line)
                if m is not None:
                    self._set_first_unused(res, m.groupdict())
        if res is not None and "last_byte_used" not in res:
            # Could not parse second line of header => something went wrong
            return None
        else:
            # Just return res (might be also None if first line of header was
            # not found)
            return res


# ==== ShellInteractions ====

class GNRCICMPv6Echo(ShellInteraction):
    @ShellInteraction.check_term
    def ping6(self, hostname, count=3, interval=1000, packet_size=4,
              hop_limit=None, timeout=1000, async_=False):
        cmd = "ping6 {hostname} -c {count} -i {interval} " \
              "-s {packet_size} -W {timeout}" \
              .format(hostname=hostname, count=count, interval=interval,
                      packet_size=packet_size, timeout=timeout)

        if hop_limit is not None:
            cmd += " -h {hop_limit}".format(hop_limit=hop_limit)

        # wait a second longer than all pings
        cmd_timeout = ((timeout / 1000) * count) + 1
        return self.cmd(cmd, timeout=cmd_timeout, async_=async_)


class GNRCPktbufStats(ShellInteraction):
    @ShellInteraction.check_term
    def pktbuf_stats(self, timeout=-1, async_=False):
        return self.cmd("pktbuf", timeout=timeout, async_=async_)

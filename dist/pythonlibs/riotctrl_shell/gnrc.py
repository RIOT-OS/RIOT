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
        self.c_reply = re.compile(
            r"\d+ bytes from "
            r"(?P<source>[0-9a-f:]+(%\S+)?): "
            r"icmp_seq=(?P<seq>\d+) ttl=(?P<ttl>\d+)"
            r"( corrupted at offset (?P<corrupted>\d+))?"
            r"( truncated by (?P<truncated>\d+) byte)?"
            r"( rssi=(?P<rssi>-?\d+) dBm)?"
            r"( time=(?P<rtt>\d+.\d+) ms)?"
            r"(?P<dup> \(DUP\))?"
        )
        self.c_stats = re.compile(
            r"(?P<tx>\d+) packets transmitted, "
            r"(?P<rx>\d+) packets received, "
            r"((?P<dup>\d+) duplicates, )?"
            r"(?P<packet_loss>\d+)% packet loss"
        )
        self.c_rtts = re.compile(
            r"round-trip min/avg/max = (?P<min>\d+.\d+)/"
            r"(?P<avg>\d+.\d+)/(?P<max>\d+.\d+) ms"
        )

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
        if reply.get("truncated") is not None:
            reply["truncated"] = int(reply["truncated"])
        if reply.get("corrupted") is not None:
            reply["corrupted"] = int(reply["corrupted"])
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
        Parses output of GNRCICMPv6Echo::ping6()

        >>> parser = GNRCICMPv6EchoParser()
        >>> res = parser.parse(
        ...     "12 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=0 ttl=64 rssi=-34 dBm time=8.839 ms\\n"
        ...     "6 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=1 ttl=64 truncated by 6 byte "
        ...         "rssi=-34 dBm time=6.925 ms\\n"
        ...     "12 bytes from fe80::385d:f965:106b:1114%6: "
        ...         "icmp_seq=2 ttl=64 corrupted at offset 7 "
        ...         "rssi=-34 dBm time=7.885 ms\\n"
        ...     "--- fe80::385d:f965:106b:1114 PING statistics ---\\n"
        ...     "3 packets transmitted, 3 packets received, 0% packet loss\\n"
        ...     "round-trip min/avg/max = 6.925/7.883/8.839 ms\\n")
        >>> sorted(res)
        ['replies', 'rtts', 'stats']
        >>> len(res["replies"])
        3
        >>> sorted(res["replies"][0])
        ['corrupted', 'rssi', 'rtt', 'seq', 'source', 'truncated', 'ttl']
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
        >>> res["replies"][0]["corrupted"]
        >>> res["replies"][0]["truncated"]
        >>> res["replies"][1]["truncated"]
        6
        >>> res["replies"][2]["corrupted"]
        7
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


class GNRCIPv6NIBNeighShowParser(ShellInteractionParser):
    def __init__(self):
        self.c_neigh = re.compile(
            r"(?P<ipv6_addr>[0-9a-f:]+)\s+"
            r"dev\s+#(?P<iface>\d+)\s+"
            r"lladdr\s+(?P<l2addr>[0-9A-F:]+)?"
            r"(\s+(?P<router>router))?"
            r"(\s+((?P<nud_state>[A-Z_]+)|-))?"
            r"(\s+(?P<ar_state>[A-Z_]+))?$"
        )

    def parse(self, cmd_output):
        """
        Parses output of GNRCIPv6NIB::nib_neigh_show()

        >>> parser = GNRCIPv6NIBNeighShowParser()
        >>> res = parser.parse("2001:db8::1 dev #5 lladdr AB:CD:EF:01:23:45 "
        ...                    "router REACHABLE REGISTERED\\n"
        ...                    "2001:db8::2 dev #5 lladdr  -")
        >>> len(res)
        2
        >>> sorted(res[0])
        ['ar_state', 'iface', 'ipv6_addr', 'l2addr', 'nud_state', 'router']
        >>> res[0]["ipv6_addr"]
        '2001:db8::1'
        >>> res[0]["iface"]
        5
        >>> res[0]["l2addr"]
        'AB:CD:EF:01:23:45'
        >>> res[0]["router"]
        True
        >>> res[0]["nud_state"]
        'REACHABLE'
        >>> res[0]["ar_state"]
        'REGISTERED'
        >>> sorted(res[1])
        ['iface', 'ipv6_addr']
        >>> res[1]["ipv6_addr"]
        '2001:db8::2'
        >>> res[1]["iface"]
        5
        """
        res = []
        for line in cmd_output.splitlines():
            m = self.c_neigh.search(line)
            if m is not None:
                res.append({k: v for k, v in m.groupdict().items() if v is not None})
                res[-1]["iface"] = int(res[-1]["iface"])
                if "router" in res[-1]:
                    res[-1]["router"] = True
        return res


class GNRCIPv6NIBPrefixShowParser(ShellInteractionParser):
    def __init__(self):
        self.c_prefix = re.compile(
            r"(?P<prefix>[0-9a-f:]+)/"
            r"(?P<prefix_len>\d+)\s+"
            r"dev\s+#(?P<iface>\d+)"
            r"(\s+expires (?P<valid_sec>\d+) sec)?"
            r"(\s+deprecates (?P<pref_sec>\d+) sec)?$"
        )

    def parse(self, cmd_output):
        """
        Parses output of GNRCIPv6NIB::nib_prefix_show()

        >>> parser = GNRCIPv6NIBPrefixShowParser()
        >>> res = parser.parse("2001:db8::/64 dev #5  expires 4999 sec "
        ...                        "deprecates 3999 sec\\n"
        ...                    "2001:db8:1::/64 dev #6")
        >>> len(res)
        2
        >>> sorted(res[0])
        ['iface', 'pref_sec', 'prefix', 'prefix_len', 'valid_sec']
        >>> res[0]["prefix"]
        '2001:db8::'
        >>> res[0]["prefix_len"]
        64
        >>> sorted(res[1])
        ['iface', 'prefix', 'prefix_len']
        """
        res = []
        for line in cmd_output.splitlines():
            m = self.c_prefix.search(line)
            if m is not None:
                pfx = {k: v for k, v in m.groupdict().items() if v is not None}
                pfx["prefix_len"] = int(pfx["prefix_len"])
                pfx["iface"] = int(pfx["iface"])
                if "valid_sec" in pfx:
                    pfx["valid_sec"] = int(pfx["valid_sec"])
                if "pref_sec" in pfx:
                    pfx["pref_sec"] = int(pfx["pref_sec"])
                res.append(pfx)
        return res


class GNRCIPv6NIBRouteShowParser(ShellInteractionParser):
    def __init__(self):
        self.c_route = re.compile(
            r"(?P<route>default(?P<primary>\*)?|"
            r"(?P<prefix>[0-9a-f:]+)/"
            r"(?P<prefix_len>\d+))\s+"
            r"(via\s+(?P<next_hop>[0-9a-f:]+)\s+)?"
            r"dev\s+#(?P<iface>\d+)$"
        )

    def parse(self, cmd_output):
        """
        Parses output of GNRCIPv6NIB::nib_route_show()

        >>> parser = GNRCIPv6NIBRouteShowParser()
        >>> res = parser.parse("2001:db8::/64 dev #5\\n"
        ...                    "2001:db8:1::/64 via fe80::1 dev #5\\n"
        ...                    "default via fe80::2 dev #5\\n"
        ...                    "default* via fe80::3 dev #6\\n")
        >>> len(res)
        4
        >>> sorted(res[0])
        ['iface', 'route']
        >>> sorted(res[0]["route"])
        ['prefix', 'prefix_len']
        >>> res[0]["route"]["prefix"]
        '2001:db8::'
        >>> res[0]["route"]["prefix_len"]
        64
        >>> res[0]["iface"]
        5
        >>> sorted(res[1])
        ['iface', 'next_hop', 'route']
        >>> res[1]["route"]["prefix"]
        '2001:db8:1::'
        >>> res[1]["route"]["prefix_len"]
        64
        >>> res[1]["next_hop"]
        'fe80::1'
        >>> res[1]["iface"]
        5
        >>> sorted(res[2])
        ['iface', 'next_hop', 'route']
        >>> sorted(res[2]["route"])
        ['default']
        >>> res[2]["route"]["default"]
        True
        >>> res[2]["next_hop"]
        'fe80::2'
        >>> res[2]["iface"]
        5
        >>> sorted(res[3])
        ['iface', 'next_hop', 'route']
        >>> sorted(res[3]["route"])
        ['default', 'primary']
        >>> res[3]["route"]["default"]
        True
        >>> res[3]["route"]["primary"]
        True
        >>> res[3]["next_hop"]
        'fe80::3'
        >>> res[3]["iface"]
        6
        """
        res = []
        for line in cmd_output.splitlines():
            m = self.c_route.search(line)
            if m is not None:
                fte = {k: v for k, v in m.groupdict().items() if v is not None}
                fte["iface"] = int(fte["iface"])
                if (
                    "prefix" in fte
                    and fte["prefix"] is not None
                    and "prefix_len" in fte
                    and fte["prefix_len"] is not None
                ):
                    fte["route"] = {
                        "prefix": fte["prefix"],
                        "prefix_len": int(fte["prefix_len"]),
                    }
                elif fte["route"].startswith("default"):
                    fte["route"] = {"default": True}
                else:
                    raise ValueError("Unexpected route value {}".format(fte))
                if "primary" in fte and fte["primary"] is not None:
                    fte["route"]["primary"] = True
                if fte.get("next_hop") is None:
                    fte.pop("next_hop", None)
                fte.pop("prefix", None)
                fte.pop("prefix_len", None)
                fte.pop("primary", None)
                res.append(fte)
        return res


class GNRCIPv6NIBABRShowParser(ShellInteractionParser):
    def __init__(self):
        self.c_abr = re.compile(
            r"(?P<addr>[0-9a-f:]+)\s+"
            r"v(?P<version>\d+)\s+expires\s+"
            r"(?P<valid_min>\d+)min$"
        )

    def parse(self, cmd_output):
        """
        Parses output of GNRCIPv6NIB::nib_abr_show()

        >>> parser = GNRCIPv6NIBABRShowParser()
        >>> res = parser.parse("2001:db8::abcd:ef01 v43 expires 1400min")
        >>> len(res)
        1
        >>> sorted(res[0])
        ['addr', 'valid_min', 'version']
        >>> res[0]["addr"]
        '2001:db8::abcd:ef01'
        >>> res[0]["version"]
        43
        >>> res[0]["valid_min"]
        1400
        """
        res = []
        for line in cmd_output.splitlines():
            m = self.c_abr.search(line)
            if m is not None:
                abr = m.groupdict()
                abr["version"] = int(abr["version"])
                abr["valid_min"] = int(abr["valid_min"])
                res.append(abr)
        return res


class GNRCPktbufStatsResults(dict):
    def is_empty(self):
        """
        Returns true if the packet buffer stats indicate that the packet buffer
        is empty
        """
        if (
            "first_byte" not in self
            or "first_unused" not in self
            or "start" not in self["first_unused"]
            or "size" not in self["first_unused"]
        ):
            raise ValueError(
                "{} has no items 'first_byte' or 'first_unused' "
                "or 'first_unused' has no items 'start' or 'size'".format(self)
            )
        else:
            return (self["first_byte"] == self["first_unused"]["start"]) and (
                self["size"] == self["first_unused"]["size"]
            )

    def fullest_capacity(self):
        """
        Returns the packet buffer usage at its fullest capacity when the
        command was called
        """
        if "last_byte_used" not in self or "size" not in self:
            raise ValueError("{} has no items 'last_byte_used' or 'size'".format(self))
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
            r"\(next: (0x(?P<next>[0-9A-Fa-f]+)|\(nil\)), "
            r"size: +(?P<size>\d+)\) ~"
        )

    @staticmethod
    def _init_res(first_byte, last_byte, size):
        return GNRCPktbufStatsResults(
            (
                ("first_byte", int(first_byte, base=16)),
                ("last_byte", int(last_byte, base=16)),
                ("size", int(size)),
            )
        )

    @staticmethod
    def _set_last_byte_used(res, last_byte_used):
        res["last_byte_used"] = int(last_byte_used)

    @staticmethod
    def _set_first_unused(res, first_unused):
        res["first_unused"] = {"start": int(first_unused["start"], base=16)}
        if first_unused["next"] is not None:
            res["first_unused"]["next"] = int(first_unused["next"], base=16)
        if "next" not in res["first_unused"] or not res["first_unused"]["next"]:
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
    def ping6(
        self,
        hostname,
        count=3,
        interval=1000,
        packet_size=4,
        hop_limit=None,
        timeout=1000,
        async_=False,
    ):
        cmd = (
            "ping {hostname} -c {count} -i {interval} "
            "-s {packet_size} -W {timeout}".format(
                hostname=hostname,
                count=count,
                interval=interval,
                packet_size=packet_size,
                timeout=timeout,
            )
        )

        if hop_limit is not None:
            cmd += " -h {hop_limit}".format(hop_limit=hop_limit)

        # wait a second longer than all pings
        cmd_timeout = ((timeout / 1000) * count) + 1
        return self.cmd(cmd, timeout=cmd_timeout, async_=async_)


class GNRCIPv6NIB(ShellInteraction):
    NEIGH = "neigh"
    PREFIX = "prefix"
    ROUTE = "route"
    ABR = "abr"

    @ShellInteraction.check_term
    def nib_cmd(self, cmd, args=None, timeout=-1, async_=False):
        return self.cmd(self._create_cmd(cmd, args), timeout=timeout, async_=async_)

    def nib_neigh_show(self, iface=None, timeout=-1, async_=False):
        return self._nib_show(self.NEIGH, iface, timeout, async_)

    def nib_neigh_add(self, iface, ipv6_addr, l2addr=None, timeout=-1, async_=False):
        args = [iface, ipv6_addr]
        if l2addr:
            args.append(l2addr)
        return self._nib_add(self.NEIGH, args, timeout, async_)

    def nib_neigh_del(self, iface, ipv6_addr, timeout=-1, async_=False):
        return self._nib_del(self.NEIGH, iface, ipv6_addr, timeout, async_)

    def nib_prefix_show(self, iface=None, timeout=-1, async_=False):
        return self._nib_show(self.PREFIX, iface, timeout, async_)

    def nib_prefix_add(
        self, iface, prefix, valid_sec=None, pref_sec=None, timeout=-1, async_=False
    ):
        if valid_sec is None and pref_sec is not None:
            raise ValueError("pref_sec provided with no valid_sec")
        args = [iface, prefix]
        if valid_sec:
            args.append(int(valid_sec))
        if pref_sec:
            args.append(int(pref_sec))
        return self._nib_add(self.PREFIX, args, timeout, async_)

    def nib_prefix_del(self, iface, prefix, timeout=-1, async_=False):
        return self._nib_del(self.PREFIX, iface, prefix, timeout, async_)

    def nib_route_show(self, iface=None, timeout=-1, async_=False):
        return self._nib_show(self.ROUTE, iface, timeout, async_)

    def nib_route_add(
        self, iface, prefix, next_hop, ltime_sec=None, timeout=-1, async_=False
    ):
        args = [iface, prefix, next_hop]
        if ltime_sec:
            args.append(int(ltime_sec))
        return self._nib_add(self.ROUTE, args, timeout, async_)

    def nib_route_del(self, iface, prefix, timeout=-1, async_=False):
        return self._nib_del(self.ROUTE, iface, prefix, timeout, async_)

    def nib_abr_show(self, timeout=-1, async_=False):
        return self._nib_show(self.ABR, timeout=timeout, async_=async_)

    def nib_abr_add(self, ipv6_addr, timeout=-1, async_=False):
        args = [ipv6_addr]
        return self._nib_add(self.ABR, args, timeout, async_)

    def nib_abr_del(self, ipv6_addr, timeout=-1, async_=False):
        args = ["del", ipv6_addr]
        return self._nib_error_cmd(self.ABR, args, timeout, async_)

    @staticmethod
    def _create_cmd(cmd, args=None):
        cmd_str = "nib {cmd}".format(cmd=cmd)
        if args is not None:
            cmd_str += " {args}".format(args=" ".join(str(a) for a in args))
        return cmd_str

    @ShellInteraction.check_term
    def _nib_error_cmd(self, cmd, args=None, timeout=-1, async_=False):
        cmd_str = self._create_cmd(cmd, args)
        res = self.cmd(cmd_str, timeout=timeout, async_=async_)
        # nib manipulation commands only show command string on success
        if res.strip() != cmd_str:
            raise RuntimeError(repr(res.strip()) + "!=" + repr(cmd_str))
        return res

    def _nib_show(self, view, iface=None, timeout=-1, async_=False):
        args = ["show"]
        if iface:
            args.append(iface)
        return self.nib_cmd(view, args, timeout, async_)

    def _nib_add(self, view, args, timeout=-1, async_=False):
        args.insert(0, "add")
        return self._nib_error_cmd(view, args, timeout, async_)

    def _nib_del(self, view, iface, item, timeout=-1, async_=False):
        args = ["del", iface, item]
        return self._nib_error_cmd(view, args, timeout, async_)


class GNRCPktbufStats(ShellInteraction):
    @ShellInteraction.check_term
    def pktbuf_stats(self, timeout=-1, async_=False):
        return self.cmd("pktbuf", timeout=timeout, async_=async_)

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
    @staticmethod
    def _add_reply(res, reply):
        reply["seq"] = int(reply["seq"])
        reply["ttl"] = int(reply["ttl"])
        reply["rtt"] = float(reply["rtt"])
        if reply.get("rssi") is not None:
            reply["rssi"] = int(reply["rssi"])
        if "replies" in res:
            res["replies"].append(reply)
        else:
            res["replies"] = [reply]

    @staticmethod
    def _set_stats(res, stats):
        stats["packet_loss"] = int(stats["packet_loss"])
        stats["rx"] = int(stats["rx"])
        stats["tx"] = int(stats["tx"])
        res["stats"] = stats

    @staticmethod
    def _set_rtts(res, rtts):
        rtts["min"] = float(rtts["min"])
        rtts["avg"] = float(rtts["avg"])
        rtts["max"] = float(rtts["max"])
        res["rtts"] = rtts

    def parse(self, cmd_output):
        res = {}
        c_reply = re.compile(r"\d+ bytes from (?P<source>[0-9a-f:]+): "
                             r"icmp_seq=(?P<seq>\d+) ttl=(?P<ttl>\d+) "
                             r"(rssi=(?P<rssi>-?\d+) dBm )?"
                             r"time=(?P<rtt>\d+.\d+) ms")
        c_stats = re.compile(r"(?P<tx>\d+) packets transmitted, "
                             r"(?P<rx>\d+) packets received, "
                             r"(?P<packet_loss>\d+)% packet loss")
        c_rtts = re.compile(r"round-trip min/avg/max = (?P<min>\d+.\d+)/"
                            r"(?P<avg>\d+.\d+)/(?P<max>\d+.\d+) ms")
        for line in cmd_output.splitlines():
            if "stats" not in res:  # If final stats were not found yet
                m = c_reply.match(line)
                if m is not None:
                    self._add_reply(res, m.groupdict())
                    continue
                m = c_stats.match(line)
                if m is not None:
                    self._set_stats(res, m.groupdict())
                    continue
            else:
                m = c_rtts.match(line)
                if m is not None:
                    self._set_rtts(res, m.groupdict())
                    return res
        # Unable to parse RTTs, so something went wrong.
        return None


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
        c_init1 = re.compile(r"packet buffer: "
                             r"first byte: 0x(?P<first_byte>[0-9A-Fa-f]+), "
                             r"last byte: 0x(?P<last_byte>[0-9A-Fa-f]+) "
                             r"\(size: +(?P<size>\d+)\)")
        c_init2 = re.compile(r"  position of last byte used: (\d+)")
        c_unused = re.compile(r"~ unused: 0x(?P<start>[0-9A-Fa-f]+) "
                              r"\(next: (0x(?P<next>[0-9A-Fa-f]+)|\(nil\)), "
                              r"size: +(?P<size>\d+)\) ~")
        res = None
        for line in cmd_output.splitlines():
            if res is None:
                m = c_init1.match(line)
                if m is not None:
                    res = self._init_res(**m.groupdict())
                # no sense in further parsing if we did not find the first line
                # yet. If we found it just continue parsing with next line
                continue
            elif "last_byte_used" not in res:
                m = c_init2.match(line)
                if m is not None:
                    self._set_last_byte_used(res, m.group(1))
                    continue
            elif "first_unused" not in res:
                m = c_unused.match(line)
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

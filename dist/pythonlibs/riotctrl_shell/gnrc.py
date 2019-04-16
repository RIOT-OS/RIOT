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

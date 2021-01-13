# Copyright (C) 2019-20 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re

from riotctrl.shell import ShellInteraction, ShellInteractionParser


# ==== Parsers ====

class ConfigPhyParser(ShellInteractionParser):
    def __init__(self):
        self.c_change = re.compile(r"Success! Channel: (?P<channel>\d+) was selected")

    @staticmethod
    def _add_channel(res, reply):
        reply["channel"] = int(reply["channel"])
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
            m = self.c_change.match(line)
            if m is not None:
                self._add_reply(res, m.groupdict())
                continue
            m = self.c_stats.match(line)
            if m is not None:
                self._set_stats(res, m.groupdict())
                continue
        return res

# ==== ShellInteractions ====

class Config_phy(ShellInteraction):
    def config_phy(self, channel, tx_pow, timeout=None, async_=False):
        cmd = "config_phy {channel} {tx_pow}" \
              .format(channel=channel, tx_pow=tx_pow)
        
        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")
        
        if "Success" not in res:
            raise RuntimeError(res)

        if str(channel) not in res:
            raise RuntimeError(res)

class Print_addr(ShellInteraction):
    def print_addr(self, timeout=None, async_=False):
        cmd = "print_addr" \
              .format()

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

        count = 0
        for line in res.splitlines():
            count+=1
            if count > 2:
                break
            pass
        addr = line
        return addr

class Txtsnd(ShellInteraction):
    def txtsnd(self, long_addr, timeout=None, async_=False):
        cmd = "txtsnd {long_addr}" \
              .format(long_addr=long_addr)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

        if "succeeded" not in res and "busy" not in res:
            raise RuntimeError(res)

    def tx_mode(self, command, timeout=None, async_=False):
        cmd = "tx_mode {command}" \
                .format(command=command)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

        if "Ok" not in res:
            raise RuntimeError(res)

    def check_last_packet(self, long_addr, channel, timeout=None, async_=False):
        cmd = "check_last_packet {long_addr} {channel}" \
                .format(long_addr=long_addr, channel=channel)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

        if "Success" not in res:
            raise RuntimeError(res)
    
    def reply(self, timeout=None, async_=False):
        cmd = "reply" \
                .format()

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

        if "Success" not in res:
            raise RuntimeError(res)

    def spam(self, long_addr, len, number_of_packets, time_betweeen_packets, timeout=None, async_=False):
        cmd = "spam {long_addr} {len} {number_of_packets} {time_betweeen_packets}" \
              .format(long_addr=long_addr, len=len, number_of_packets=number_of_packets, time_betweeen_packets=time_betweeen_packets)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

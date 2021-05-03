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

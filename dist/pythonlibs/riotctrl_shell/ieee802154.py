# Copyright (C) 2019-20 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import re

from riotctrl.shell import ShellInteraction

# ==== ShellInteractions ====

class Config_phy(ShellInteraction):
    def config_phy(self, phy_mode, channel, tx_pow, timeout=None, async_=False):
        cmd = "config_phy {phy_mode} {channel} {tx_pow}" \
              .format(phy_mode=phy_mode, channel=channel, tx_pow=tx_pow)
        
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

    def txtsnd(self, long_addr, number_of_packets, time_betweeen_packets, timeout=None, async_=False):
        cmd = "txtsnd {long_addr} {number_of_packets} {time_betweeen_packets}" \
              .format(long_addr=long_addr, number_of_packets=number_of_packets, time_betweeen_packets=time_betweeen_packets)

        try:
            res = self.cmd(cmd, timeout=timeout, async_=async_)
        except Exception as e:
            print(str(e))
            print("Exception")

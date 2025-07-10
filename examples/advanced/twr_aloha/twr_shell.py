# SPDX-FileCopyrightText: 2021 Inria
# SPDX-License-Identifier: LGPL-2.1-only

"""
twr-aloha shell interactions
"""

import re

from riotctrl.shell import ShellInteraction, ShellInteractionParser


class TwrIfconfigParser(ShellInteractionParser):
    iface_c = re.compile(r"Iface\s+(?P<name>\d+)\s")
    hwaddr_c = re.compile(r"HWaddr:\s+(?P<name>[0-9a-fA-F:]+)\s")
    hwaddr64_c = re.compile(r"Long HWaddr:\s+(?P<name>[0-9a-fA-F:]+)")
    panid_c = re.compile(r"NID:\s+(?P<name>[0-9a-fA-F:]+)")
    channel_c = re.compile(r"Channel:\s+(?P<name>[0-9]+)")

    def parse(self, cmd_output):
        netif = {
            "netif": None,
            "hwaddr": None,
            "hwaddr64": None,
            "panid": None,
            "channel": None,
        }
        for line in cmd_output.splitlines():
            m = self.iface_c.search(line)
            if m is not None:
                netif["netif"] = m.group("name")
            m = self.hwaddr_c.search(line)
            if m is not None:
                netif["hwaddr"] = m.group("name")
            m = self.hwaddr64_c.search(line)
            if m is not None:
                netif["hwaddr64"] = m.group("name")
            m = self.panid_c.search(line)
            if m is not None:
                netif["panid"] = m.group("name")
            m = self.channel_c.search(line)
            if m is not None:
                netif["channel"] = m.group("name")
        return netif


class TwrCmd(ShellInteraction):
    @ShellInteraction.check_term
    def twr_cmd(self, args=None, timeout=-1, async_=False):
        cmd = "twr"
        if args is not None:
            cmd += " {args}".format(args=" ".join(str(a) for a in args))
        return self.cmd(cmd, timeout=timeout, async_=False)

    def twr_listen(self, on=True, timeout=-1, async_=False):
        return self.twr_cmd(
            args=("lst", "on" if on else "off"), timeout=timeout, async_=async_
        )

    def twr_req(self, addr="ff:ff", count=1, interval=1000, proto="ss", timeout=-1, async_=False):
        return self.twr_cmd(
            args=("req", f"{addr}", f"-c {count}", f"-p {proto}", f"-i {interval}"),
            timeout=timeout,
            async_=async_,
        )

    @ShellInteraction.check_term
    def ifconfig(self, timeout=-1, async_=False):
        return self.cmd("ifconfig", timeout, async_)

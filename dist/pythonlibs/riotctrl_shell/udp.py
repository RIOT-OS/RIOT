# SPDX-FileCopyrightText: 2020 Freie Universität Berlin
# SPDX-FileCopyrightText: 2026 TU Dresden
# SPDX-License-Identifier: LGPL-2.1-only

"""
UDP-related shell interactions

Defines UDP-related shell command interactions
"""

import math
import re
from typing import Optional, Union

import pexpect

from riotctrl.shell import ShellInteraction, ShellInteractionParser

__author__ = "Martine S. Lenders <martine.lenders@tu-dresden.de>"

# ==== Parsers ====


class UDPClientSendParser(ShellInteractionParser):
    """
    Shell interaction parser for `shell_cmd_udp`
    """

    def __init__(self):
        self.success_c = re.compile(
            r"Success:\s+sen[td]\s+"
            r"(?P<payload_len>\d+)\s+"
            r"byte(\(s\))?\s+(over\s+UDP\s+)?to\s+"
            r"\[(?P<dst>[0-9a-f:]+(%\S+)?)\]:"
            r"(?P<dport>\d+)$"
        )

    def parse(self, cmd_output: str):
        """
        Parses output of UDP::udp_client_send()

        >>> parser = UDPClientSendParser()
        >>> res = parser.parse("Success: send 12 byte to [fe80::1%2]:1337  \\n"
        ...                    "Success: sent 5 byte(s) to [abcd::2]:52\\n")
        >>> len(res)
        2
        >>> sorted(res[0])
        ['dport', 'dst', 'payload_len']
        >>> sorted(res[1])
        ['dport', 'dst', 'payload_len']
        >>> res[0]["payload_len"], res[1]["payload_len"]
        (12, 5)
        >>> res[0]["dst"], res[1]["dst"]
        ('fe80::1%2', 'abcd::2')
        >>> res[0]["dport"], res[1]["dport"]
        (1337, 52)
        >>> res = parser.parse("Just some non-sense output...\\n"
        ...                    "what happens if we have an error?\\n")
        >>> res
        []
        """
        res = []
        for line in cmd_output.splitlines():
            m = self.success_c.search(line.strip())
            if m is not None:
                msg = m.groupdict()
                msg["payload_len"] = int(msg["payload_len"])
                msg["dport"] = int(msg["dport"])
                res.append(msg)
        return res


# ==== ShellInteractions ====


class UDPClient(ShellInteraction):
    """
    Shell interaction for `shell_cmd_udp`
    """

    ADDR_PORT_PAIR_FMT = "[{addr}]:{port}"

    @ShellInteraction.check_term
    def udp_client_send(
        self,
        dest_addr: str,
        port: Union[str, int],
        payload: Union[str, bytes],
        count: Optional[int] = None,
        delay_us: Optional[int] = None,
        async_: bool = False,
    ):
        flags = ""
        delay_str = ""
        if count is not None:
            if delay_us is None or delay_us == 0:
                # wait 1 sec per message (+ .1 sec slack)
                timeout = count * 1.1
                delay_str = f" {count}"
            else:
                # wait .5 sec more per message
                timeout = math.ceil((delay_us * count) / 1000000) + (5 * (count / 10))
                delay_str = f" {count} {delay_us:d}"
        else:
            # wait 1 sec for output
            timeout = 1
        if isinstance(payload, bytes):
            flags = "-b "
            payload = payload.hex()
        addr_port = self.ADDR_PORT_PAIR_FMT.format(addr=dest_addr, port=port)
        res = self.cmd(
            f'udp send {flags}{addr_port} "{payload}"{delay_str}',
            timeout=timeout,
            async_=async_,
        )
        if "error:" in res.lower() or "usage:" in res:
            raise RuntimeError(res)
        return res


class UDPServer(ShellInteraction):
    """
    Shell interaction for `shell_cmd_udp_server`
    """

    @ShellInteraction.check_term
    def udp_server_start(
        self, port: Union[int, str], timeout: int = -1, async_: bool = False
    ):
        res = self.cmd(f"udp server start {port}", timeout=timeout, async_=async_)
        if "Success:" not in res:
            raise RuntimeError(res)
        return res

    @ShellInteraction.check_term
    def udp_server_stop(self, timeout: int = -1, async_: bool = False):
        return self.cmd("udp server stop", timeout=timeout, async_=async_)

    def udp_server_check_output(self, count: int = 1, delay_us: int = 1000000):
        packets_lost = 0
        if delay_us > 0:
            timeout = (delay_us / 1000000) * 10
        else:
            timeout = 10
        for _ in range(count):
            exp = self.riotctrl.term.expect(
                [
                    pexpect.TIMEOUT,
                    # TODO: Remove, when tests/gnrc_udp uses `shell_cmd_udp_server`
                    r"Packets received:\s+\d",
                    r"Received UDP data from \[[0-9a-f:]+%.+\]:\d+",
                ],
                timeout=timeout,
            )
            if not exp:  # expect timed out
                packets_lost += 1
        return (packets_lost / count) * 100

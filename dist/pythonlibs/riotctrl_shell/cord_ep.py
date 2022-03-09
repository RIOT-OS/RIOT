# Copyright (C) 2020 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
CORD EP-related shell interactions

Defines CORD EP-related shell command interactions
"""

import re

from riotctrl.shell import ShellInteraction, ShellInteractionParser


# ==== Parsers ====


class CordEpRegistrationInfoParser(ShellInteractionParser):
    def __init__(self):
        self.comps = {
            "rdaddr": re.compile(
                r"RD address:\s+"
                r"(?P<rdaddr>coaps?://"
                r"\[[0-9a-f:]+(%\S+)?\](:\d+)?)"
            ),
            "epname": re.compile(r"ep name:\s+(?P<epname>.+)$"),
            "ltime": re.compile(r"lifetime:\s+(?P<ltime>\d+)s$"),
            "regif": re.compile(r"reg if:\s+(?P<regif>\S+)$"),
            "location": re.compile(r"location:\s+(?P<location>\S+)$"),
        }

    def parse(self, cmd_output):
        """
        Parses output of CordEp::cord_ep_info() and CordEp::cord_ep_register()

        >>> parser = CordEpRegistrationInfoParser()
        >>> res = parser.parse("CoAP RD connection status:\\n"
        ...                "RD address:"
        ...                " coap://[fe80::48d5:9eff:fe98:6b74]:5683\\n"
        ...                "   ep name: RIOT-760D2323760D2323\\n"
        ...                "  lifetime: 60s\\n"
        ...                "    reg if: /resourcedirectory\\n"
        ...                "  location: /reg/1/")
        >>> res['rdaddr']
        'coap://[fe80::48d5:9eff:fe98:6b74]:5683'
        >>> res['epname']
        'RIOT-760D2323760D2323'
        >>> res['ltime']
        60
        >>> res['regif']
        '/resourcedirectory'
        >>> res = parser.parse("CoAP RD connection status:\\n"
        ...             "RD address:"
        ...             " coaps://[fe80::48d5:9eff:fe98:6b74%iface0]:5684\\n"
        ...             "   ep name: RIOT-760D2323760D2323\\n"
        ...             "  lifetime: 60s\\n"
        ...             "    reg if: /resourcedirectory\\n"
        ...             "  location: /reg/1/")
        >>> res['rdaddr']
        'coaps://[fe80::48d5:9eff:fe98:6b74%iface0]:5684'
        """
        res = {}
        for line in cmd_output.splitlines():
            for key, comp in self.comps.items():
                m = comp.search(line)
                if m is not None:
                    try:
                        res[key] = int(m.group(key))
                    except ValueError:
                        res[key] = m.group(key)
        return res


class CordEpDiscoverParser(ShellInteractionParser):
    def __init__(self):
        self.comp = re.compile(r"the registration interface is" r"\s+'(?P<regif>\S+)'$")

    def parse(self, cmd_output):
        """
        Parses output of CordEp::cord_ep_discover()

        >>> parser = CordEpDiscoverParser()
        >>> parser.parse(
        ...     "the registration interface is '/resourcedirectory'")
        '/resourcedirectory'
        """
        for line in cmd_output.splitlines():
            m = self.comp.search(line)
            if m is not None:
                return m.group("regif")
        return None


# ==== ShellInteractions ====


class CordEp(ShellInteraction):
    REGISTER = "register"
    DISCOVER = "discover"
    UPDATE = "update"
    REMOVE = "remove"
    INFO = "info"

    @ShellInteraction.check_term
    def cord_ep_cmd(self, cmd, args=None, timeout=-1, async_=False):
        cmd_str = "cord_ep {cmd}".format(cmd=cmd)
        if args is not None:
            cmd_str += " {args}".format(args=" ".join(str(a) for a in args))
        res = self.cmd(cmd_str, timeout=timeout, async_=async_)
        if ("error:" in res) or ("usage:" in res):
            raise RuntimeError(res)
        return res

    def cord_ep_info(self, timeout=-1, async_=False):
        return self.cord_ep_cmd(self.INFO, None, timeout, async_)

    def cord_ep_register(self, uri, regif=None, timeout=-1, async_=False):
        args = [uri]
        if regif is not None:
            args.append(regif)
        return self.cord_ep_cmd(self.REGISTER, args, timeout, async_)

    def cord_ep_discover(self, uri, timeout=-1, async_=False):
        return self.cord_ep_cmd(self.DISCOVER, (uri,), timeout, async_)

    def cord_ep_update(self, timeout=-1, async_=False):
        return self.cord_ep_cmd(self.UPDATE, None, timeout, async_)

    def cord_ep_remove(self, timeout=-1, async_=False):
        return self.cord_ep_cmd(self.REMOVE, None, timeout, async_)

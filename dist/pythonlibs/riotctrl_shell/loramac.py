# Copyright (C) 2021 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
Shell interactions related to Semtech Loramac package

Defines shell command interactions related to Semtech Loramac package
"""

import re

from riotctrl.shell import ShellInteraction

# ==== Parsers ====


class LoramacHelpParser():
    def __init__(self):
        self.eeprom_c = re.compile(r'Usage: loramac \<([\w+\|?]+)\>')

    def has_eeprom(self, cmd_output):
        for line in cmd_output.splitlines():
            m = self.eeprom_c.search(line)
            if m is not None:
                if 'save' in m.group(1):
                    return True
        return False


class LoramacUpLinkCounterParser():
    def __init__(self):
        self.uplink_c = re.compile(r'Uplink Counter: (\d+)')

    def uplink_count(self, cmd_output):
        for line in cmd_output.splitlines():
            m = self.uplink_c.search(line)
            if m is not None:
                return m.group(1)
        raise RuntimeError

# ==== ShellInteractions ====


class Loramac(ShellInteraction):
    @ShellInteraction.check_term
    def loramac_cmd(self, args=None, timeout=-1, async_=False):
        cmd = "loramac"
        if args is not None:
            cmd += " {args}".format(args=" ".join(str(a) for a in args))
        return self.cmd(cmd, timeout=timeout, async_=False)

    def loramac_join(self, mode, timeout=-1, async_=False):
        res = self.loramac_cmd(args=("join", mode),
                               timeout=timeout, async_=async_)
        if "succeeded" in res:
            return res
        raise RuntimeError(res)

    def loramac_tx(self, payload, cnf=False, port=2, timeout=-1, async_=False):
        payload = '\"' + payload + '\"'
        res = self.loramac_cmd(
            args=("tx", payload, 'cnf' if cnf else "uncnf", port),
            timeout=timeout, async_=async_
            )
        if "success" in res:
            return res
        raise RuntimeError(res)

    def loramac_set(self, key, value, timeout=-1, async_=False):
        res = self.loramac_cmd(args=("set", key, value),
                               timeout=timeout, async_=async_)
        if "Usage" in res:
            raise RuntimeError(res)
        return res

    def loramac_get(self, key, timeout=-1, async_=False):
        res = self.loramac_cmd(args=("get", key),
                               timeout=timeout, async_=async_)
        if "Usage" in res:
            raise RuntimeError(res)
        return res

    def loramac_eeprom_save(self, timeout=-1, async_=False):
        return self.loramac_cmd(args=("save",), timeout=timeout, async_=async_)

    def loramac_eeprom_erase(self, timeout=-1, async_=False):
        return self.loramac_cmd(args=("erase",), timeout=timeout,
                                async_=async_)

    def loramac_help(self, timeout=-1, async_=False):
        return self.loramac_cmd(args=("help",), timeout=timeout, async_=async_)

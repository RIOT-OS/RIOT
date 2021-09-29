# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""
Shell interactions related to network interfaces

Defines shell command interactions related to network interfaces
"""

import re

from riotctrl.shell import ShellInteraction, ShellInteractionParser


# ==== Parsers ====


class IfconfigListParser(ShellInteractionParser):
    def __init__(self):
        self.iface_c = re.compile(r"Iface\s+(?P<name>\S+)\s")
        # option values are repetitions of at least one non white space
        # separated by at most one whitespace
        # e.g. for MCS: 1 (BPSK, rate 1/2, 2x frequency repetition)  MTU :1280
        # "1 (BPSK, rate 1/2, 2x frequency repetition)" belongs to the option
        # value, "MTU" does not
        self.option_c = re.compile(r"^(?P<option>[^:]+):\s?" r"(?P<value>\S+(\s\S+)*)$")
        # options are evaluated before flags, so all options that don't contain
        # colons are flags
        self.flag_c = re.compile(r"^(?P<flag>[^:]+)$")
        self.ipv6_c = re.compile(
            r"inet6 (?P<type>addr|group): "
            r"(?P<addr>[0-9a-f:]+)(\s+"
            r"scope:\s+(?P<scope>\S+)"
            r"(?P<anycast>\s+\[anycast\])?\s+"
            r"(?P<state>\S+))?$"
        )
        self.bl_header_c = re.compile(
            r"(?P<mode>White|Black)-listed " r"link layer addresses:"
        )
        self.bl_c = re.compile(r"\d+: (?P<addr>[0-9a-f]{2}(:[0-9a-f]{2})*)$")

    def parse(self, cmd_output):
        """
        Parses output of Ifconfig::ifconfig_list()

        See tests (tests/test_netif_list_parse.py) for further possible items:

        >>> parser = IfconfigListParser()
        >>> res = parser.parse("Iface WP_01  HWAddr: ab:cd  6LO  PROMISC\\n"
        ...                    "Iface ET_01  HWaddr: 01:23:45:67:89:AB\\n")
        >>> len(res)
        2
        >>> res["WP_01"]["hwaddr"]
        'ab:cd'
        >>> sorted(res["WP_01"]["flags"])
        ['6LO', 'PROMISC']
        >>> res["ET_01"]["hwaddr"]
        '01:23:45:67:89:AB'
        """
        netifs = None
        current = None
        parse_ipv6 = False
        parse_blacklist = False
        stats_parser = IfconfigStatsParser()
        offset = 0
        for line in cmd_output.splitlines():
            m = self.iface_c.search(line)
            if m is not None:
                name = m.group("name")
                if netifs is None:
                    netifs = {}
                current = netifs[name] = {}
                # Go ahead in line to not confuse options parser
                line = line[m.end() :]
                offset += m.end() + 1
            if current is not None:
                # XXX checking for IPv4 address might also go here
                if "ipv6_addrs" not in current:
                    parse_ipv6 = self.ipv6_c.search(line) is not None
                    if not parse_ipv6 and not parse_blacklist:
                        self._parse_netif_option(current, line)
                if parse_ipv6:
                    parse_ipv6 = self._parse_ipv6(current, line)
                elif parse_blacklist:
                    m = self.bl_c.search(line)
                    if m is not None:
                        if "blacklist" in current:
                            current["blacklist"].append(m.group("addr"))
                        else:
                            current["whitelist"].append(m.group("addr"))
                    else:
                        parse_blacklist = False
                elif (
                    not parse_blacklist
                    and "blacklist" not in current
                    and "whitelist" not in current
                ):
                    m = self.bl_header_c.search(line)
                    if m is not None:
                        if m.group("mode") == "Black":
                            current["blacklist"] = []
                        else:
                            current["whitelist"] = []
                        parse_blacklist = True
                m = stats_parser.header_c.search(line)
                if m is not None:
                    stats = stats_parser.parse(cmd_output[offset:])
                    if stats is not None:
                        current["stats"] = stats
                        # assume stats to be always last
                        current = None
                        parse_blacklist = False
                        parse_ipv6 = False
            offset += len(line)
        return netifs

    @staticmethod
    def _snake_case(option):
        """
        Converts all option names parsed by _parse_netif_option() to snake_case

        >>> IfconfigListParser._snake_case("Max. Retrans.")
        'max_retrans'
        """
        return re.sub(r"\W+", "_", option.strip().lower()).strip("_")

    @staticmethod
    def _convert_value(value_str):
        """
        Tries to converts an option value parsed by _parse_netif_option() to
        int or float if possible

        >>> IfconfigListParser._convert_value("12345")
        12345
        >>> IfconfigListParser._convert_value("0xf")
        15
        >>> IfconfigListParser._convert_value("3.14")
        3.14
        >>> IfconfigListParser._convert_value("3.14ispi")
        '3.14ispi'
        """
        try:
            # try to convert to int
            return int(value_str)
        except ValueError:
            try:
                # next try to convert to int from hex
                if value_str.startswith("0x"):
                    return int(value_str[2:], base=16)
                else:
                    return int(value_str, base=16)
            except ValueError:
                try:
                    # next try to convert to float
                    return float(value_str)
                except ValueError:
                    # lastly just use the string
                    return value_str

    def _parse_netif_option(self, netif, line):
        """Parses all the options found before the IP address listing"""
        # remove potential content before line like logging tag, date etc.
        line = line.split("          ")[-1]
        # options and flags are separated by two spaces
        for token in line.strip().split("  "):
            # ensure there are no whitespaces at start or end => output bug
            assert token == token.strip()
            m = self.option_c.search(token)
            if m is not None:
                option = self._snake_case(m.group("option"))
                value_str = m.group("value").strip()
                netif[option] = self._convert_value(value_str)
            m = self.flag_c.search(token)
            if m is not None:
                flag = m.group("flag")
                if "flags" in netif:
                    netif["flags"].append(flag)
                else:
                    netif["flags"] = [flag]

    def _parse_ipv6(self, netif, line):
        """
        Parses IPv6 unicast, anycast, and multicast addresses
        """
        m = self.ipv6_c.search(line)
        if m is not None:
            addr = m.groupdict()
            typ = addr.pop("type")
            if typ == "addr":  # unicast address
                # reformat anycast item if existent
                if addr.get("anycast") is None:
                    addr.pop("anycast", None)
                else:
                    addr["anycast"] = True
                if "ipv6_addrs" in netif:
                    netif["ipv6_addrs"].append(addr)
                else:
                    netif["ipv6_addrs"] = [addr]
            else:  # multicast address
                for key in set(addr):
                    # remove empty matches
                    if addr[key] is None:
                        del addr[key]
                if "ipv6_groups" in netif:
                    netif["ipv6_groups"].append(addr)
                else:
                    netif["ipv6_groups"] = [addr]
            return True
        return False


class IfconfigStatsParser(ShellInteractionParser):
    def __init__(self):
        self.header_c = re.compile(r"Statistics for (?P<module>.+)$")
        self.rx_c = re.compile(
            r"RX packets\s+(?P<packets>\d+)\s+" r"bytes\s+(?P<bytes>\d+)$"
        )
        self.tx_c = re.compile(
            r"TX packets\s+(?P<packets>\d+)\s+"
            r"\(Multicast:\s+(?P<multicast>\d+)\)\s+"
            r"bytes\s+(?P<bytes>\d+)$"
        )
        self.tx_err_c = re.compile(
            r"TX succeeded\s+(?P<succeeded>\d+)\s+" r"errors\s+(?P<errors>\d+)$"
        )

    def parse(self, cmd_output):
        """
        Parses output of Ifconfig::ifconfig_stats or the statistics part of
        Ifconfig::ifconfig_list.

        :param cmd_output(str): output of Ifconfig::ifconfig_stats or
                                Ifconfig::ifconfig_list
        :return: dictionary with one entry per statistics module, each module
                 containing an entry 'rx' and 'tx' with respective statistics

        >>> parser = IfconfigStatsParser()
        >>> res = parser.parse(
        ...         "Statistics for IPv6\\n"
        ...         "  RX packets 14  bytes 1104\\n"
        ...         "  TX packets 3 (Multicast: 1)  bytes 192\\n"
        ...         "  TX succeeded 3 errors 0\\n")
        >>> sorted(res)
        ['IPv6']
        >>> sorted(res["IPv6"])
        ['rx', 'tx']
        >>> sorted(res["IPv6"]["rx"])
        ['bytes', 'packets']
        >>> sorted(res["IPv6"]["tx"])
        ['bytes', 'errors', 'multicast', 'packets', 'succeeded']
        >>> res["IPv6"]["rx"]["bytes"]
        1104
        """
        stats = None
        current = None
        for line in cmd_output.splitlines():
            line = line.strip()
            m = self.header_c.search(line)
            if m is not None:
                module = m.group("module")
                if stats is None:
                    stats = {}
                current = stats[module] = {}
            if current is not None:
                if "rx" not in current:
                    m = self.rx_c.search(line)
                    if m is not None:
                        current["rx"] = {k: int(v) for k, v in m.groupdict().items()}
                elif "tx" not in current:
                    m = self.tx_c.search(line)
                    if m is not None:
                        current["tx"] = {k: int(v) for k, v in m.groupdict().items()}
                elif "tx" in current:
                    m = self.tx_err_c.search(line)
                    if m is not None:
                        current["tx"].update(
                            {k: int(v) for k, v in m.groupdict().items()}
                        )
        return stats


# ==== ShellInteractions ====


class Ifconfig(ShellInteraction):
    def ifconfig_list(self, netif=None, timeout=-1, async_=False):
        return self.ifconfig_cmd(netif=netif, timeout=timeout, async_=async_)

    @ShellInteraction.check_term
    def ifconfig_cmd(self, netif=None, args=None, timeout=-1, async_=False):
        cmd = "ifconfig"
        if netif is not None:
            cmd += " {netif}".format(netif=netif)
        if args is not None:
            if netif is None:
                raise ValueError("netif required when args are provided")
            cmd += " {args}".format(args=" ".join(str(a) for a in args))
        return self.cmd(cmd, timeout=timeout, async_=False)

    def ifconfig_help(self, netif, timeout=-1, async_=False):
        return self.ifconfig_cmd(
            netif=netif, args=("help",), timeout=timeout, async_=async_
        )

    def ifconfig_set(self, netif, key, value, timeout=-1, async_=False):
        return self._ifconfig_success_cmd(
            netif=netif, args=("set", key, value), timeout=timeout, async_=async_
        )

    def ifconfig_up(self, netif, timeout=-1, async_=False):
        self._ifconfig_error_cmd(
            netif=netif, args=("up",), timeout=timeout, async_=async_
        )

    def ifconfig_down(self, netif, timeout=-1, async_=False):
        self._ifconfig_error_cmd(
            netif=netif, args=("down",), timeout=timeout, async_=async_
        )

    def ifconfig_add(self, netif, addr, anycast=False, timeout=-1, async_=False):
        args = ["add", addr]
        if anycast:
            args.append("anycast")
        return self._ifconfig_success_cmd(
            netif=netif, args=args, timeout=timeout, async_=async_
        )

    def ifconfig_del(self, netif, addr, timeout=-1, async_=False):
        return self._ifconfig_success_cmd(
            netif=netif, args=("del", addr), timeout=timeout, async_=async_
        )

    def ifconfig_flag(self, netif, flag, enable=True, timeout=-1, async_=False):
        return self._ifconfig_success_cmd(
            netif=netif,
            args=("{}{}".format("" if enable else "-", flag),),
            timeout=timeout,
            async_=async_,
        )

    def ifconfig_l2filter_add(self, netif, addr, timeout=-1, async_=False):
        return self._ifconfig_success_cmd(
            netif=netif, args=("l2filter", "add", addr), timeout=timeout, async_=async_
        )

    def ifconfig_l2filter_del(self, netif, addr, timeout=-1, async_=False):
        return self._ifconfig_success_cmd(
            netif=netif, args=("l2filter", "del", addr), timeout=timeout, async_=async_
        )

    def ifconfig_stats(self, netif, module, timeout=-1, async_=False):
        res = self.ifconfig_cmd(
            netif=netif, args=("stats", module), timeout=timeout, async_=async_
        )
        if "Statistics for " in res:
            return res
        raise RuntimeError(res)

    def ifconfig_stats_reset(self, netif, module, timeout=-1, async_=False):
        res = self.ifconfig_cmd(
            netif=netif, args=("stats", module, "reset"), timeout=timeout, async_=async_
        )
        if "Reset statistics for module " in res:
            return res
        raise RuntimeError(res)

    def _ifconfig_success_cmd(self, netif=None, args=None, timeout=-1, async_=False):
        """For commands that have a success output"""
        res = self.ifconfig_cmd(netif=netif, args=args, timeout=timeout, async_=async_)
        if "success" in res:
            return res
        raise RuntimeError(res)

    def _ifconfig_error_cmd(self, netif=None, args=None, timeout=-1, async_=False):
        """For commands that only have an error output"""
        res = self.ifconfig_cmd(netif=netif, args=args, timeout=timeout, async_=async_)
        if "error" in res:
            raise RuntimeError(res)


class TXTSnd(ShellInteraction):
    @ShellInteraction.check_term
    def netif_txtsnd(self, netif, target, data, timeout=-1, async_=False):
        cmd = "txtsnd {netif} {target} {data}".format(
            netif=netif, target=target, data=data
        )
        res = self.cmd(cmd)
        if "error" in res or "usage" in res:
            raise RuntimeError(res)
        return res

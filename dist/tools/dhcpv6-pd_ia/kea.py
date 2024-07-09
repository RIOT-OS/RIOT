# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import json
import os
import tempfile
import time

import base


__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


class KeaConfig(object):
    def __init__(self, interface, prefix, prefix_len, delegated_len=None,
                 valid_lifetime=40000, preferred_lifetime=30000,
                 renew_timer=10000, rebind_timer=20000):
        if not prefix.endswith("::"):
            raise ValueError("prefix must end with '::'")
        if int(prefix_len) < 1 or int(prefix_len) > 128:
            raise ValueError("prefix_len must be between 1 and 128")
        if int(valid_lifetime) <= 0:
            raise ValueError("valid_lifetime must be greater than 0")
        if int(preferred_lifetime) <= 0:
            raise ValueError("preferred_lifetime must be greater than 0")
        if int(renew_timer) <= 0:
            raise ValueError("renew_timer must be greater than 0")
        if int(rebind_timer) <= 0:
            raise ValueError("rebind_timer must be greater than 0")
        if delegated_len is None:
            delegated_len = prefix_len
        self._config_dict = {
            "Dhcp6": {
                "interfaces-config": {
                    "interfaces": [interface]
                },
                "lease-database": {
                    "type": "memfile"
                },
                "valid-lifetime": int(valid_lifetime),
                "preferred-lifetime": int(preferred_lifetime),
                "renew-timer": int(renew_timer),
                "rebind-timer": int(rebind_timer),
                "subnet6": [{
                    "id": 1,
                    "interface": interface,
                    "subnet": "{}/{}".format(prefix, prefix_len),
                    "pd-pools": [{
                        "prefix": prefix,
                        "prefix-len": prefix_len,
                        "delegated-len": delegated_len
                    }],
                }],
            },
        }
        self.config_file = None

    def __del__(self):
        if self.config_file is not None:
            self.config_file.close()

    def _dump_json(self):
        json.dump(self._config_dict, self.config_file)
        self.config_file.flush()

    def __str__(self):
        if self.config_file is None:
            self.config_file = tempfile.NamedTemporaryFile(mode="w")
        self._dump_json()
        return self.config_file.name

    @property
    def interface(self):
        return self._config_dict["Dhcp6"]["interfaces-config"]["interfaces"][0]


class KeaServer(base.DHCPv6Server):
    command = ["kea-dhcp6", "-c"]
    package = {
            "generic": {"name": "kea-dhcp6", "url": "https://kea.isc.org/"},
            "Debian": {"name": "kea-dhcp6-server"},
            "Arch": {"name": "kea"},
        }

    def __init__(self, config, next_hop="fe80::2", *args, **kwargs):
        super().__init__(*args, **kwargs)
        self.next_hop = next_hop
        self.config = config

    def pre_run(self):
        # create config file in daemon so it is not automatically deleted
        self.command.append(str(self.config))
        if self.daemonized:
            # need to wait for interface to connect before we can run server
            time.sleep(2)

    def run(self):
        if not self.is_installed():
            self.install()
            if self.installer.os in ["Arch"] and \
               not os.path.exists("/var/run/kea/"):
                # workaround: Arch does not create that directory on first
                # install
                os.makedirs("/var/run/kea/")
        # workaround: Ubuntu does not create that directory automatically
        os.makedirs("/run/kea/", exist_ok=True)
        super().run()

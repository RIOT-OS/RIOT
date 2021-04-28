# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.


import platform
import re
import os

from .apt import Apt
from .pacman import PacMan
from .base import AskToInstall

__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


class PackageManagerFactory(object):
    @staticmethod
    def _get_linux_distro():
        if hasattr(platform, "linux_distribution"):
            return platform.linux_distribution()[0]
        elif os.path.exists("/etc/os-release"):
            with open("/etc/os-release") as f:
                for line in f:
                    m = re.match(r"^NAME=\"(.+)\"$", line)
                    if m is not None:
                        return m.group(1)
        return None

    @classmethod
    def get_installer(cls):
        system = platform.system()
        if system == "Linux":
            system = cls._get_linux_distro()
        if system in ["Debian", "Ubuntu", "Linux Mint"]:
            return Apt("Debian")
        if system in ["Arch Linux"]:
            return PacMan("Arch")
        else:
            return AskToInstall()

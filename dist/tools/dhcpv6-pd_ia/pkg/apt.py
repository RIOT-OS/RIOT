# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import subprocess

from .base import Installer

__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


class Apt(Installer):
    def _install(self, package):
        subprocess.run(["apt-get", "-y", "install",
                        package[self.os]["name"]])

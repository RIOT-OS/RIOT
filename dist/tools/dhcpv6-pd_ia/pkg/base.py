# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright (C) 2020 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import abc
import sys


__author__ = "Martine S. Lenders"
__copyright__ = "Copyright (C) 2020 Freie Universität Berlin"
__credits__ = ["Martine S. Lenders"]
__license__ = "LGPLv2.1"
__maintainer__ = "Martine S. Lenders"
__email__ = "m.lenders@fu-berlin.de"


class Installer(abc.ABC):
    def __init__(self, os):
        self.os = os

    @abc.abstractmethod
    def _install(self, package):
        """
        Executes the install command
        """
        pass

    def install(self, package):
        """
        Executes the install command, but asks the user before-hand if it is
        okay to do so.
        """
        if self._ask(package):
            self._install(package)

    @staticmethod
    def _ask(package):
        valid = {"yes": True, "y": True, "ye": True,
                 "no": False, "n": False}
        while True:
            sys.stdout.write("Install package {}? [Y/n] "
                             .format(package["generic"]["name"]))
            sys.stdout.flush()
            choice = input().lower()
            if choice == '':
                return True
            elif choice in valid:
                return valid[choice]
            else:
                raise ValueError(
                    "Please respond with 'yes' or 'no' (or 'y' or 'n').",
                )


class AskToInstall(Installer):
    def install(self, package):
        print("Please install {name} ({url})".format(**package["generic"]),
              file=sys.stderr)

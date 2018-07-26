# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles generic connection and IO to the serial driver.

"""


class RiotDriver:
    """Contains all reusable functions for connecting, sending and recieveing
    data.

    """

    used_devices = []

    def __init__(self):
        raise NotImplementedError()

    def close(self):
        """Close serial connection."""
        raise NotImplementedError()

    def read(self):
        """Read and decode data."""
        raise NotImplementedError()

    def write(self, data):
        """Tries write data."""
        raise NotImplementedError()

    @staticmethod
    def get_configs():
        """Gets available serial configurations."""
        raise NotImplementedError()


def main():
    """Tests basic usage of the class"""


if __name__ == "__main__":
    main()

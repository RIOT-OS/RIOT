# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module provice the base interface for a device to a driver.

"""
import logging
from . import driver_manager


class BaseDevice:
    """Instance for devices to connect and untilize drivers."""

    def __init__(self, *args, **kwargs):
        self._driver = driver_manager.driver_from_config(*args, **kwargs)

    def close(self):
        """Closes the device connection."""
        self._driver.close()

    def _read(self):
        """Reads data from the driver."""
        return self._driver.read()

    def _write(self, data):
        """Writes data to the driver."""
        return self._driver.write(data)

    def is_connected_to_board(self):
        """Dummy - confirm if a connection is for target board."""
        logging.warning("Check if board is connected: dummy should be"
                        " implmeneted in subclasses")
        raise NotImplementedError()

    @classmethod
    def from_autodetect(cls, *args, **dev_config):
        """Connects to a range of possible configurations."""
        configs = driver_manager.available_configs(*args, **dev_config)
        logging.debug("Configs: %r", configs)
        for config in configs:
            for retry in range(0, 2):
                logging.debug("Autodetect attempt: %d", retry)
                conn = cls(**config)
                try:
                    if conn.is_connected_to_board():
                        return conn
                except Exception as err:
                    logging.debug("Cannot connect: %r", err)
                conn.close()

        raise ValueError("Could not locate board, check if board is"
                         "connected or is_connected_to_board is correct")

    @classmethod
    def copy_driver(cls, device):
        """Copies an the driver instance so multiple devices can use one driver."""
        logging.debug("Cloning Driver: %r", device._driver)
        return cls(dev_type='driver', driver=device._driver)


def main():
    """Tests basic usage of the class

    Used for unit testing, information should be confirm with DEBUG info.
    """
    logging.getLogger().setLevel(logging.DEBUG)
    BaseDevice()


if __name__ == "__main__":
    main()

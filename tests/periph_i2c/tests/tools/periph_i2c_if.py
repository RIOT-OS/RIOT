# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles parsing of information from RIOT periph_i2c test.
"""
import logging
from dut_shell import DutShell


class PeriphI2CIf(DutShell):
    """Interface to the a node with periph_i2c firmware."""
    FW_ID = 'periph_i2c'
    DEFAULT_DEV = 0
    DEFAULT_ADDR = 85
    DEFAULT_REG = 152
    DEFAULT_LEN = 10
    DEFAULT_DATA = [0, 1, 2]

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    def is_connected_to_board(self):
        """Checks if board is connected."""
        return self.i2c_get_id()["data"] == [self.FW_ID]

    def i2c_acquire(self, dev=DEFAULT_DEV):
        """Get access to the I2C bus."""
        return self.send_cmd('i2c_acquire {}'.format(dev))

    def i2c_release(self, dev=DEFAULT_DEV):
        """Release to the I2C bus."""
        return self.send_cmd('i2c_release {}'.format(dev))

    def i2c_read_reg(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, reg=DEFAULT_REG,
                     flag=0):
        """Read byte from register."""
        return self.send_cmd('i2c_read_reg'
                             ' {} {} {} {}'.format(dev, addr, reg, flag))

    def i2c_read_regs(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, reg=DEFAULT_REG,
                      leng=DEFAULT_LEN, flag=0):
        """Read bytes from registers."""
        return self.send_cmd('i2c_read_regs'
                             ' {} {} {} {} {}'.format(dev, addr, reg,
                                                      leng, flag))

    def i2c_read_byte(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, flag=0):
        """Read byte from the I2C device."""
        return self.send_cmd('i2c_read_byte {} {} {}'.format(dev, addr, flag))

    def i2c_read_bytes(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, leng=DEFAULT_LEN,
                       flag=0):
        """Read bytes from the I2C device."""
        return self.send_cmd('i2c_read_bytes'
                             ' {} {} {} {}'.format(dev, addr, leng, flag))

    def i2c_write_reg(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, reg=DEFAULT_REG,
                      data=DEFAULT_DATA, flag=0):
        """Write byte to the I2C device."""
        if isinstance(data, list):
            data = data[0]
        return self.send_cmd('i2c_write_reg'
                             ' {} {} {} {} {}'.format(dev, addr, reg,
                                                      data, flag))

    def i2c_write_regs(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, reg=DEFAULT_REG,
                       data=DEFAULT_DATA, flag=0):
        """Write byte to register."""
        stri = ' '.join(str(x) for x in data)
        return self.send_cmd('i2c_write_regs'
                             ' {} {} {} {} {}'.format(dev, addr, reg, flag, stri))

    def i2c_write_byte(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR, data=DEFAULT_DATA,
                       flag=0):
        """Write bytes to registers."""
        if isinstance(data, list):
            data = data[0]
        return self.send_cmd('i2c_write_byte'
                             ' {} {} {} {}'.format(dev, addr, data, flag))

    def i2c_write_bytes(self, dev=DEFAULT_DEV, addr=DEFAULT_ADDR,
                        data=DEFAULT_DATA, flag=0):
        """Write bytes to registers."""
        stri = ' '.join(str(x) for x in data)
        return self.send_cmd('i2c_write_bytes'
                             ' {} {} {} {}'.format(dev, addr, flag, stri))

    def i2c_get_devs(self):
        """Gets amount of supported i2c devices."""
        return self.send_cmd('i2c_get_devs')

    def i2c_get_id(self):
        """Get the id of the fw."""
        return self.send_cmd('i2c_get_id')

    def get_command_list(self):
        """List of all commands."""
        cmds = list()
        cmds.append(self.i2c_get_devs)
        cmds.append(self.i2c_get_id)
        cmds.append(self.i2c_acquire)
        cmds.append(self.i2c_read_reg)
        cmds.append(self.i2c_read_regs)
        cmds.append(self.i2c_read_byte)
        cmds.append(self.i2c_read_bytes)
        cmds.append(self.i2c_write_reg)
        cmds.append(self.i2c_write_regs)
        cmds.append(self.i2c_write_byte)
        cmds.append(self.i2c_write_bytes)
        cmds.append(self.i2c_release)
        return cmds


def main():
    """Test for I2C."""

    logging.getLogger().setLevel(logging.DEBUG)
    try:
        i2c = PeriphI2CIf.from_autodetect()
        cmds = i2c.get_command_list()
        logging.debug("======================================================")
        for cmd in cmds:
            cmd()
            logging.debug("--------------------------------------------------")
        logging.debug("======================================================")
    except Exception as exc:
        logging.debug(exc)


if __name__ == "__main__":
    main()

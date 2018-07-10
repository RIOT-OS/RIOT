# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""@package PyToAPI
This module handles functions for a low level shell interface.

"""
import logging
import errno
import os
from base_device import BaseDevice


class LLShell(BaseDevice):
    """Handles basic functions and commands for memory map interface."""
    READ_REG_CMD = "rr"
    WRITE_REG_CMD = "wr"
    EXECUTE_CMD = "ex"
    RESET_CMD = "mcu_rst"
    SUCCESS = '0'
    RESULT_SUCCESS = 'Success'
    RESULT_ERROR = 'Error'
    RESULT_TIMEOUT = 'Timeout'

    def __init__(self, *args, **kwargs):
        super().__init__(*args, **kwargs)

    @staticmethod
    def _try_parse_data(data):
        parsed_data = None
        if len(data) > 1:
            # response contains data
            try:
                if len(data[1]) - 2 <= 8:
                    parsed_data = int(data[1], 0)
                else:
                    d_len = len(data[1]) - 1
                    parsed_data = bytearray.fromhex(data[1][2:d_len])
            except ValueError:
                parsed_data = data[1:]
        return parsed_data

    @staticmethod
    def _error_msg(data):
        s_errcode = errno.errorcode[data]
        s_errmsg = os.strerror(data)
        return "{}-{} [{}]".format(s_errcode, s_errmsg, data)

    def _populate_cmd_info(self, data):
        cmd_info = {}
        try:
            if data[0] == self.SUCCESS:
                cmd_info['data'] = self._try_parse_data(data)
                cmd_info['msg '] = "EOK-command success [0]"
                cmd_info['result'] = self.RESULT_SUCCESS
                logging.debug(self.RESULT_SUCCESS)
            else:
                # put error code in data
                cmd_info['data'] = int(data[0], 0)
                cmd_info['msg'] = self._error_msg(cmd_info['data'])
                cmd_info['result'] = self.RESULT_ERROR
                logging.debug(self.RESULT_ERROR)
                logging.debug(cmd_info['msg'])
        except Exception as exc:
            cmd_info['msg'] = "Unknown Error {}".format(exc)
            cmd_info['data'] = data[0]
            cmd_info['result'] = self.RESULT_ERROR
            logging.debug(self.RESULT_ERROR)
            logging.debug(exc)
        return cmd_info

    def send_cmd(self, send_cmd):
        """Returns a dictionary with information from the event.

        msg - The message from the response, only used for information.
        cmd - The command sent, used to track what has occured.
        data - Parsed information of the data requested.
        result - Either success, error or timeout.
        """
        self._write(send_cmd)
        data = self._read()
        cmd_info = {'cmd': send_cmd}
        if data == "":
            cmd_info['msg'] = "Timeout occured"
            cmd_info['data'] = None
            cmd_info['result'] = self.RESULT_TIMEOUT
            logging.debug(self.RESULT_TIMEOUT)
        else:
            data = data.replace('\n', '')
            data = data.split(',')
            cmd_info.update(self._populate_cmd_info(data))
        return cmd_info

    def read_bytes(self, index, size=1):
        """Reads bytes in the register map."""
        logging.debug("FXN: read_bytes(%r,%r)", index, size)
        cmd = '{} {} {}'.format(self.READ_REG_CMD, index, size)
        return self.send_cmd(cmd)

    def write_bytes(self, index, data, size=4):
        """Writes bytes in the register map."""
        logging.debug("FXN: write_bytes(%r,%r)", index, data)
        cmd = "{} {}".format(self.WRITE_REG_CMD, index)
        if isinstance(data, list):
            for i in range(0, len(data)):
                if len(data) - i - 1 < len(data):
                    cmd += ' {}'.format(data[len(data) - i - 1])
                else:
                    cmd += ' 0'
        else:
            for i in range(0, size):
                cmd += ' {}'.format((data >> ((i) * 8)) & 0xFF)
        return self.send_cmd(cmd)

    def read_bits(self, index, offset, bit_amount):
        """Read specific bits in the register map."""
        logging.debug("FXN: read_bits(%r, %r, %r)", index, offset, bit_amount)
        bytes_to_read = int((bit_amount - 1 + offset)/8 + 1)
        bit_mask = (2 ** bit_amount) - 1
        cmd_info = self.read_bytes(index, bytes_to_read)
        if cmd_info['result'] == self.RESULT_SUCCESS:
            cmd_info['cmd'] += ', read_bits {} {} {}'.format(index, offset,
                                                             bit_amount)
            cmd_info['data'] = cmd_info['data'] >> offset
            cmd_info['data'] = cmd_info['data'] & bit_mask

        logging.debug("Bits: %r", cmd_info['data'])
        return cmd_info

    def write_bits(self, index, offset, bit_amount, data):
        """Modifies specific bits in the register map."""
        cmd_sent = ""
        logging.debug("FXN: write_bits"
                      "(%r, %r, %r, %r)", index, offset, bit_amount, data)
        bytes_to_read = int((bit_amount - 1 + offset)/8 + 1)
        cmd_info = self.read_bytes(index, bytes_to_read)
        if cmd_info['result'] != self.RESULT_SUCCESS:
            return cmd_info
        cmd_sent += cmd_info['cmd']
        bit_mask = int((2 ** bit_amount) - 1)
        bit_mask = bit_mask << offset
        cmd_info['data'] = cmd_info['data'] & (~bit_mask)
        data = cmd_info['data'] | ((data << offset) & bit_mask)
        cmd_info = self.write_bytes(index, data, bytes_to_read)
        cmd_sent += cmd_info['cmd']
        if cmd_info['result'] == self.RESULT_SUCCESS:
            cmd_sent += ',write_bits {} {} {} {}'.format(index, offset,
                                                         bit_amount, data)
        cmd_info['cmd'] = cmd_sent
        return cmd_info

    def execute_changes(self):
        """Executes device configuration changes."""
        logging.debug("FXN: execute_changes")
        return self.send_cmd(self.EXECUTE_CMD)

    def reset_mcu(self):
        """Resets the device."""
        logging.debug("FXN: reset_mcu")
        return self.send_cmd(self.RESET_CMD)


def test_bpt():
    """Tests if basic functions work on the BPT memory map"""
    b_if = LLShell()
    b_if.reset_mcu()
    b_if.execute_changes()
    index = 152
    b_if.read_bytes(index)
    b_if.write_bytes(index, 0x0a0b0c0d)
    b_if.read_bytes(index, 4)
    b_if.write_bytes(index, 0x01, 1)
    b_if.read_bytes(index, 4)
    b_if.write_bytes(index, [9, 8, 7, 6, 5, 4, 3, 2])
    b_if.read_bytes(index, 8)
    b_if.write_bytes(index, 0)
    b_if.read_bytes(index, 1)

    b_if.write_bits(index, 0, 1, 1)
    b_if.read_bytes(index, 1)
    b_if.read_bits(index, 0, 1)

    b_if.write_bits(index, 0, 2, 2)
    b_if.read_bits(index, 0, 2)

    b_if.write_bits(index, 1, 3, 6)
    b_if.read_bits(index, 1, 3)

    b_if.write_bits(index, 0, 8, 0xa5)
    b_if.read_bits(index, 0, 8)

    b_if.write_bits(index, 1, 7, 0x7F)
    b_if.read_bits(index, 1, 7)

    b_if.write_bits(index, 0, 9, 0x142)
    b_if.read_bits(index, 0, 9)

    b_if.write_bits(index, 1, 1, 1)
    b_if.read_bits(index, 0, 3)

    b_if.write_bits(index, 2, 1, 1)
    b_if.read_bits(index, 0, 3)
    b_if.reset_mcu()


def main():
    """Tests DeviceShellIf class"""
    logging.getLogger().setLevel(logging.DEBUG)
    test_bpt()


if __name__ == "__main__":
    main()

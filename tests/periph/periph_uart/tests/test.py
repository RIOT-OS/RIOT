#!/usr/bin/env python3

# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

"""@package PyToAPI
A test that can be used for now but should be adapted to a framework.
TODO: REMOVE and adapt to a proper testing platform
"""
import argparse
import errno
import logging
import random
import string

from periph_uart_if import PeriphUartIf
from if_lib import bpt_if


def kwexpect(val1, val2, level="WARN"):
    """A poor mans pexpect"""
    res = level
    try:
        if isinstance(val1, str) and isinstance(val2, str):
            if val1.lower() == val2.lower():
                res = "PASS"
        elif isinstance(val1, list):
            if len(val1) == 0:
                return [res, val1, val2]
            err = False
            if len(val1) != len(val2):
                err = True
            for test_index in range(0, len(val1)):
                if val1[test_index] != val2[test_index]:
                    err = True
            if err is False:
                res = "PASS"
            else:
                return [level, val1, val2]
        elif (val1 == val2) or (val1 is val2):
            res = "PASS"
    except TypeError:
        res = 'TypeError'
    except Exception as exc:
        res = 'Unknown Error'
        logging.debug(exc)

    return [res, val1, val2]


class TestParam:
    """A poor mans way to store test params because I didn't know about dict"""
    test_num = None
    action = None
    expect_res = None


class Test:
    name = ""
    desc = ""
    notes = ""
    result = ""
    cmd_log = None
    num = 0

    def __init__(self, name, desc, cmd_list):
        self.name = name
        self.desc = desc
        self.cmd_log = cmd_list
        self.result = "PASS"

    def run_test(self, action, ex_res=None, ex_data=None,
                 res_lvl="WARN", data_lvl="WARN"):
        tp = TestParam()
        tp.test_num = self.num
        tp.action = action
        if 'data' not in action:
            tp.action['data'] = []
        tp.expect_res = list()
        if ex_res is not None:
            tp.expect_res.append(kwexpect(action['result'], ex_res, res_lvl))
            if tp.expect_res[-1][0] == "FAIL":
                self.result = tp.expect_res[-1][0]
            if tp.expect_res[-1][0] == "WARN" and self.result != "FAIL":
                self.result = tp.expect_res[-1][0]
        if ex_data is not None:

            tp.expect_res.append(kwexpect(action['data'], ex_data, data_lvl))
            if tp.expect_res[-1][0] == "FAIL":
                self.result = tp.expect_res[-1][0]
            if tp.expect_res[-1][0] == "WARN" and self.result != "FAIL":
                self.result = tp.expect_res[-1][0]
        self.num += 1
        self.cmd_log.append(tp)
        return action['data']

    def skip_test(self, skipped_fxn):
        tp = TestParam()

        tp.test_num = self.num
        self.num += 1
        tp.action = skipped_fxn
        tp.expect_res = "SKIP"
        self.cmd_log.append(tp)

    def manual_test(self, action):
        tp = TestParam()

        tp.test_num = self.num
        self.num += 1
        tp.action = action
        tp.expect_res = "PASS"
        self.cmd_log.append(tp)


def setup_test(bpt, t):
    bpt.reset_mcu()
    t.manual_test("Reset BPT")

    bpt.set_uart_baud(115200)
    bpt.execute_changes()
    t.manual_test("BPT: setup default baudrate")


def increment_data(data):
    """Increment each character."""
    return ''.join(chr(ord(n) + 1) for n in data)


def create_random_data(data_len):
    """Generate random data with specified length."""
    return 't' + ''.join([random.choice(
        string.digits) for n in range(data_len)])


def echo_test(bpt, uart, dut_uart, support_reset=False):
    cmd_log = list()
    t = Test('echo test',
             'Tests DUT receive/transmit functionality in loopback mode',
             cmd_log)

    setup_test(bpt, t)

    t.run_test(uart.uart_init(dut_uart, 115200), "Success")
    t.run_test(uart.uart_send_string(dut_uart, "t111"), "Success", ["t111"])
    data = create_random_data(100)
    t.run_test(uart.uart_send_string(dut_uart, data), "Success", [data])
    t.run_test(uart.uart_init(dut_uart, 38400), "Success")
    t.run_test(uart.uart_send_string(dut_uart, "t111"), "Timeout")

    return t


def echo_ext_test(bpt, uart, dut_uart, support_reset=False):
    cmd_log = list()
    t = Test('echo ext test',
             'Tests DUT receive/transmit functionality in ext loopback mode',
             cmd_log)

    setup_test(bpt, t)

    bpt.set_uart_mode(1)
    bpt.execute_changes()
    t.manual_test("BPT: set echo ext mode")

    t.run_test(uart.uart_init(dut_uart, 115200), "Success")
    t.run_test(uart.uart_send_string(dut_uart, "t111"), "Success", ["u222"])
    data = create_random_data(100)
    t.run_test(uart.uart_send_string(dut_uart, data), "Success", [increment_data(data)])

    return t


def register_read_test(bpt, uart, dut_uart, support_reset=False):
    cmd_log = list()
    t = Test('register read test',
             'Tests DUT receive/transmit functionality via reading BPT registers',
             cmd_log)

    setup_test(bpt, t)

    bpt.set_uart_mode(2)
    bpt.execute_changes()
    t.manual_test("BPT: set echo ext mode")

    t.run_test(uart.uart_init(dut_uart, 115200), "Success")
    t.run_test(uart.uart_send_string(
        dut_uart, "\"rr 152 10\""), "Success", ["0,0x09080706050403020100"])
    t.run_test(uart.uart_send_string(
        dut_uart, "\"rr -1 10\""), "Success", [errno.EINVAL])

    return t


def print_full_result(test):
    """Print full test results."""
    print('==================================================================')
    print('Name:\t\t' + test.name)
    print('Desc:\t\t' + test.desc)
    print('Result:\t\t' + test.result)
    print('Notes:\t\t' + test.notes)
    print('------------------------------------------------------------------')
    for test_param in test.cmd_log:
        print('Test Number:\t***%d***' % test_param.test_num)
        if not isinstance(test_param.action, str):
            print('Command:\t' + test_param.action['cmd'])
            if 'msg' in test_param.action:
                print('Message:\t' + test_param.action['msg'])
            if test_param.action['data'] is not None:
                print('Data:\t\t[%s]' % ', '.join(map(str,
                                                      test_param.action['data'])))
            print('Result:\t\t' + test_param.action['result'])
        else:
            print('Command:\t' + test_param.action)
        if isinstance(test_param.expect_res, str):
            print('Expect Result:\t%s' % (test_param.expect_res))
        else:
            for res in test_param.expect_res:
                print('Expect Result:\t%s (%s/%s)' % (res[0], res[1], res[2]))
        print('----------')


def print_results(test_list):
    """Print brief test results."""
    print('')
    print('==================================================================')
    for test in test_list:
        print('Name:\t\t' + test.name)
        print('Result:\t\t' + test.result + '\n')


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("--log", help='Set the log level (DEBUG, INFO, WARN)')
    parser.add_argument("--bpt_port", help='Port for the bluepill tester')
    parser.add_argument("--dut_port", help='Port for device under test')
    parser.add_argument("--dut_uart",
                        help='DUT UART number to test',
                        type=int,
                        default=1)
    args = parser.parse_args()

    if args.log is not None:
        loglevel = args.log
        numeric_level = getattr(logging, loglevel.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError('Invalid log level: %s' % loglevel)
        logging.basicConfig(level=loglevel)

    bpt = bpt_if.BptIf(port=args.bpt_port)
    uart = PeriphUartIf(port=args.dut_port)

    print('Starting Test periph_uart')
    test_list = list()
    test_list.append(echo_test(bpt, uart, args.dut_uart))
    print_full_result(test_list[-1])
    test_list.append(echo_ext_test(bpt, uart, args.dut_uart))
    print_full_result(test_list[-1])
    test_list.append(register_read_test(bpt, uart, args.dut_uart))
    print_full_result(test_list[-1])

    print_results(test_list)


if __name__ == "__main__":
    main()

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
import sys
import os
import argparse
import time
import logging


BPT_ADDR = 85
BPT_USER_REG = 152


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
    """A poor mans way to run tests"""
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


def setup_test(t, i2c, bpt):
    bpt.reset_mcu()
    t.manual_test("Reset BPT")

    bpt.set_sys_cr(1)
    bpt.execute_changes()
    time.sleep(0.01)
    bpt.set_sys_cr(0)
    bpt.execute_changes()
    t.manual_test("Try to reset the DUT")
    time.sleep(2)


def mutex_test(i2c, bpt):
    cmd_log = []
    t = Test('mutex test', 'Tests mutex aquire/release functionality', cmd_log)
    t.notes += 'Cannot test since it can still be used without mutex\n'
    t.notes += 'Cannot test expected timeout due to no reset\n'
    t.notes += 'Do we need double release failure check?\n'
    t.notes += 'Does not check multiple busses acquired\n'

    setup_test(t, i2c, bpt)

    num_of_dev = t.run_test(i2c.i2c_get_devs(), "Success")[0]

    for d_num in range(0, num_of_dev):
        t.run_test(i2c.i2c_release(d_num), "Success")

        t.run_test(i2c.i2c_acquire(d_num), "Success")

        if d_num == 0:
            t.run_test(i2c.i2c_read_reg(d_num, BPT_ADDR, BPT_USER_REG),
                       "Success", [0])

        t.run_test(i2c.i2c_release(d_num), "Success")

        t.skip_test("i2c.i2c_read_byte(d_num, BPT_ADDR), Error")
        t.skip_test("i2c.i2c_acquire(d_num), Success")
        t.skip_test("i2c.i2c_acquire(d_num), Timeout")

    return t


def read_test(i2c, bpt):
    cmd_log = []
    t = Test('read test', 'Tests the read functionality for default i2c bus \
(0) with the bpt', cmd_log)
    t.notes += 'Does not check flags\n'

    setup_test(t, i2c, bpt)

    d_num = 0
    t.run_test(i2c.i2c_release(d_num), "Success")
    t.run_test(i2c.i2c_acquire(d_num), "Success")
    t.run_test(i2c.i2c_read_reg(d_num, BPT_ADDR, BPT_USER_REG), "Success", [0])
    t.run_test(i2c.i2c_read_reg(d_num, BPT_ADDR+1, BPT_USER_REG), "Error")
    t.run_test(i2c.i2c_read_reg(d_num, BPT_ADDR, BPT_USER_REG+1),
               "Success", [1])
    t.run_test(i2c.i2c_read_regs(d_num, BPT_ADDR, BPT_USER_REG, 1),
               "Success", [0])
    t.run_test(i2c.i2c_read_regs(d_num, BPT_ADDR, BPT_USER_REG+1, 1),
               "Success", [1])
    t.run_test(i2c.i2c_read_regs(d_num, BPT_ADDR, BPT_USER_REG, 2),
               "Success", [0, 1])
    t.run_test(i2c.i2c_read_regs(d_num, BPT_ADDR, BPT_USER_REG, 3),
               "Success", [0, 1, 2])
    t.run_test(i2c.i2c_read_regs(d_num, BPT_ADDR-1, BPT_USER_REG, 1), "Error")
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR,), "Success", [0])
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR-2), "Error")
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1), "Success", [0])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 2), "Success", [0, 1])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 3), "Success", [0, 1, 2])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR+4, 3), "Error")
    t.run_test(i2c.i2c_release(d_num), "Success")
    return t


def check_write_reg(t, d_num, addr, reg, val, i2c):
    t.run_test(i2c.i2c_write_reg(d_num, addr, reg, val[0]), "Success")
    t.run_test(i2c.i2c_read_reg(d_num, addr, reg), "Success", val)


def check_write_regs(t, d_num, addr, reg, val, i2c):
    t.run_test(i2c.i2c_write_regs(d_num, addr, reg, val), "Success")
    t.run_test(i2c.i2c_read_regs(d_num, addr, reg, len(val)), "Success", val)


def write_test(i2c, bpt):
    cmd_log = []
    t = Test('write test', 'Tests the write functionality for default i2c bus \
(0) with the bpt', cmd_log)
    t.notes += 'Does not check flags\n'

    setup_test(t, i2c, bpt)

    d_num = 0

    t.run_test(i2c.i2c_release(d_num), "Success")
    t.run_test(i2c.i2c_acquire(d_num), "Success")

    check_write_reg(t, d_num, BPT_ADDR, BPT_USER_REG, [42], i2c)
    check_write_reg(t, d_num, BPT_ADDR, BPT_USER_REG, [0], i2c)
    check_write_reg(t, d_num, BPT_ADDR, BPT_USER_REG+1, [41], i2c)
    check_write_reg(t, d_num, BPT_ADDR, BPT_USER_REG+1, [1], i2c)
    t.run_test(i2c.i2c_write_reg(d_num, BPT_ADDR-4, BPT_USER_REG, 0), "Error")

    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG, [44], i2c)
    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG, [0], i2c)
    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG+1, [45, 46], i2c)
    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG+1, [1, 2], i2c)
    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG, [47, 48, 49], i2c)
    check_write_regs(t, d_num, BPT_ADDR, BPT_USER_REG, [0, 1, 2], i2c)
    t.run_test(i2c.i2c_write_regs(d_num, BPT_ADDR-5, BPT_USER_REG, [0]),
               "Error")

    t.run_test(i2c.i2c_write_byte(d_num, BPT_ADDR, BPT_USER_REG+1), "Success")
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR), "Success", [1])
    t.run_test(i2c.i2c_write_byte(d_num, BPT_ADDR, BPT_USER_REG), "Success")
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR), "Success", [0])
    t.run_test(i2c.i2c_write_byte(d_num, BPT_ADDR+5, 0), "Error")

    t.run_test(i2c.i2c_write_bytes(d_num, BPT_ADDR, [BPT_USER_REG+1]),
               "Success")
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR), "Success", [1])
    t.run_test(i2c.i2c_write_bytes(d_num, BPT_ADDR, [BPT_USER_REG, 50]),
               "Success")
    t.run_test(i2c.i2c_read_byte(d_num, BPT_ADDR), "Success", [50])
    t.run_test(i2c.i2c_write_bytes(d_num, BPT_ADDR, [BPT_USER_REG+1, 51, 52]),
               "Success")
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 2), "Success", [51, 52])
    t.run_test(i2c.i2c_write_bytes(d_num, BPT_ADDR, [BPT_USER_REG, 0, 1, 2]),
               "Success")
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 3), "Success", [0, 1, 2])

    t.run_test(i2c.i2c_write_bytes(d_num, BPT_ADDR-7, [47, 48, 49]), "Error")

    t.run_test(i2c.i2c_release(d_num), "Success")
    return t


def read_flag_test(i2c, bpt):
    cmd_log = []
    t = Test('read flag test', 'Tests the read flag functionality', cmd_log)
    t.notes += 'Not a comprehesive test but tests a proper frame\n'

    setup_test(t, i2c, bpt)

    d_num = 0

    t.run_test(i2c.i2c_release(d_num), "Success")
    t.run_test(i2c.i2c_acquire(d_num), "Success")
    t.run_test(i2c.i2c_read_reg(d_num, BPT_ADDR, BPT_USER_REG), "Success", [0])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 4), "Success", [0])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 12), "Success", [1])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 8), "Success", [2])
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 8), "Error")
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 12), "Error")
    t.skip_test("i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 4), Success, [0]")
    t.skip_test("i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 4), Error, [0]")
    t.skip_test("RESET DUT")
    t.run_test(i2c.i2c_read_bytes(d_num, BPT_ADDR, 1, 1))

    return t


def print_full_result(test):
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
    print('')
    print('==================================================================')
    for x in test_list:
        print('Name:\t\t' + x.name)
        print('Result:\t\t' + x.result + '\n')


def main():
    sys.path.append(os.path.join(os.path.dirname(__file__), 'tools'))
    print(sys.path)
    import periph_i2c_if
    import bpt_if

    parser = argparse.ArgumentParser()
    parser.add_argument("--log", help='Set the log level (DEBUG, INFO, WARN)')
    parser.add_argument("--dut_port", help='Port for device under test')
    parser.add_argument("--bpt_port", help='Port for the bluepill tester')
    parser.add_argument("--dut_baud", help='Baud rate for device under test')
    args = parser.parse_args()

    if args.log is not None:
        loglevel = args.log
        numeric_level = getattr(logging, loglevel.upper(), None)
        if not isinstance(numeric_level, int):
            raise ValueError('Invalid log level: %s' % loglevel)
        logging.basicConfig(level=loglevel)
    baud = 115200
    if args.dut_baud is not None:
        baud = int(args.dut_baud, 0)

    if args.bpt_port is None:
        bpt = bpt_if.BptIf.from_autodetect()
    else:
        bpt = bpt_if.BptIf(port=args.bpt_port)

    if args.dut_port is None:
        i2c = periph_i2c_if.PeriphI2CIf.from_autodetect(baudrate=baud)
    else:
        i2c = periph_i2c_if.PeriphI2CIf(port=args.dut_port, baudrate=baud)

    print('Starting Test periph_i2c')
    test_list = []
    test_list.append(mutex_test(i2c, bpt))
    print_full_result(test_list[-1])
    test_list.append(read_test(i2c, bpt))
    print_full_result(test_list[-1])
    test_list.append(write_test(i2c, bpt))
    print_full_result(test_list[-1])
    test_list.append(read_flag_test(i2c, bpt))
    print_full_result(test_list[-1])

    print_results(test_list)


if __name__ == "__main__":
    main()

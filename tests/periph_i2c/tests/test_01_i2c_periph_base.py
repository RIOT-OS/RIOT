# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""
Tests basic usage of RIOT based I2C devices in the pytest framework
"""


# pylint: disable=W0613, C0103
def test_acquire_and_release_should_succeed(nodes, cmd_collect):
    """Acquires and releases the i2c bus"""
    # Since acquire and release are part of the setup nothing needs to be done
    pass


def test_acquire_after_release_should_succeed(nodes, cmd_collect):
    """Acquires after releasing the i2c bus"""
    res = cmd_collect('dut', nodes['dut'].i2c_release(0))
    assert res['result'] == 'Success'
    res = cmd_collect('dut', nodes['dut'].i2c_acquire(0))
    assert res['result'] == 'Success'


def test_double_acquire_should_timeout(nodes, cmd_collect):
    """Tries to lock the thread by acquiring twice"""
    res = cmd_collect('dut', nodes['dut'].i2c_acquire(0))
    assert res['result'] == 'Timeout'


def test_read_register_should_succeed(nodes, cmd_collect):
    """Provides a basic i2c register read"""
    addr = nodes['phil'].cmd_list['i2c.slave_addr_1']['default']
    reg = nodes['phil'].cmd_list['user_reg.64']['offset']
    data = cmd_collect('phil', nodes['phil'].read_reg('user_reg.64', size=1))
    res = cmd_collect('dut', nodes['dut'].i2c_read_reg(0, addr, reg))
    assert res['result'] == 'Success'
    assert res['data'] == [data['data']]


def test_read_register_after_NACK_should_succeed(nodes, cmd_collect):
    """Forces a NACK then reads a register to see if recovered"""
    res = cmd_collect('dut', nodes['dut'].i2c_read_reg(0, 42, 0))
    assert res['result'] == 'Error'
    addr = nodes['phil'].cmd_list['i2c.slave_addr_1']['default']
    reg = nodes['phil'].cmd_list['user_reg.64']['offset']
    data = cmd_collect('phil', nodes['phil'].read_reg('user_reg.64', size=1))
    res = cmd_collect('dut', nodes['dut'].i2c_read_reg(0, addr, reg))
    assert res['result'] == 'Success'
    assert res['data'] == [data['data']]

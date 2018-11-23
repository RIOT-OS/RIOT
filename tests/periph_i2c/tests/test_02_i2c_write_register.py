# Copyright (c) 2018 Kevin Weiss, for HAW Hamburg  <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""
Tests writing to PHiLIP with a RIOT I2C device in the pytest framework
"""
import pytest


@pytest.mark.parametrize("write_data", [
    ([41]),
    ([42, 43]),
    ([44, 45, 46]),
    ([47, 48, 49, 50, 51, 52, 53, 54, 55, 56])
])
def test_write_registers(nodes, cmd_collect, write_data):
    """Tests writing different amounts of data with i2c to PHiLIP"""
    addr = nodes['phil'].cmd_list['i2c.slave_addr_1']['default']
    reg = nodes['phil'].cmd_list['user_reg.64']['offset']
    res = cmd_collect('dut', nodes['dut'].i2c_write_regs(0,
                                                         addr,
                                                         reg,
                                                         write_data))
    assert res['result'] == 'Success'
    data = cmd_collect('phil', nodes['phil'].read_reg('user_reg.64'))['data']
    assert write_data == data[:len(write_data)]

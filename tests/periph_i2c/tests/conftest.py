#!/usr/bin/env python3
# Copyright (C) 2018 Kevin Weiss <kevin.weiss@haw-hamburg.de>
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
"""
Provides basic i2c tests between two devices, PHiLIP as the reference, and
DUT as the device under test which contains RIOT based firmware
(/tests/periph_i2c).
This tests is based on the pytest framework.

Requires:
    pytest
    pytest-html
    riot-pal
    periph_i2c_if

Usage:
    usage: pytest [directory_to_tests] [optional_args]
    optional arguments:
      -h, --help  show this help message and exit
      --html          Outputs html page
      --junitxml      Outputs junit based xml
      --philip_port   Port for philip board
      --dut_port      Port for device under test board
      --dut_baud      Baudrate of the device under test
      --board         Name of the board to test

Example:
    pytest tests/periph_i2c/tests/ \
    --html=report.html --junitxml=report.xml \
    --philip_port=/dev/ttyACM0 \
    --dut_port=/dev/ttyACM1 \
    --dut_baud=115200 \
    --board=nucleo-f303re
"""
from time import sleep
from pprint import pprint
import pytest
from riot_pal import LLMemMapIf, PHILIP_MEM_MAP_PATH
from periph_i2c_if import PeriphI2CIf


_RETRY_CONNECT = 5


def pytest_addoption(parser):
    """Provides argments for connection settings in pytest"""
    parser.addoption("--philip_port", default='/dev/ttyACM0',
                     help="Port for philip board")
    parser.addoption("--dut_port", default='/dev/ttyACM1',
                     help="Port for device under test board")
    parser.addoption("--dut_baud", default=115200,
                     help="Baudrate of the device under test")
    parser.addoption("--board", default='Unknown',
                     help="Name of the board to test")


def _reset_nodes(phil):
    """Uses philip to reset the DUT and itself"""
    # reset philip
    assert phil.reset_mcu()['result'] == 'Success'
    sleep(0.1)

    # reset dut by toggle rst pin with philip
    assert phil.write_reg('sys.cr.dut_rst', 1)['result'] == 'Success'
    assert phil.execute_changes()['result'] == 'Success'
    sleep(0.1)
    assert phil.write_reg('sys.cr.dut_rst', 0)['result'] == 'Success'
    assert phil.execute_changes()['result'] == 'Success'


@pytest.fixture(scope="session")
def node_setup(metadata, request):
    """Setup and teardown of the nodes"""
    phil = LLMemMapIf(PHILIP_MEM_MAP_PATH, 'serial',
                      request.config.getoption("--philip_port"), timeout=1)
    dut = PeriphI2CIf('serial', request.config.getoption("--dut_port"),
                      request.config.getoption("--dut_baud"))

    fw_rev_res = phil.read_reg('sys.fw_rev')
    assert fw_rev_res['result'] == 'Success'

    serial_number_res = phil.read_reg('sys.sn.12')
    assert serial_number_res['result'] == 'Success'

    metadata['PHiLIP Firmware Revision'] = fw_rev_res['data']
    metadata['PHiLIP Serial Number'] = serial_number_res['data']
    metadata['BOARD'] = request.config.getoption("--board")
    yield {'phil': phil, 'dut': dut}
    _reset_nodes(phil)
    phil.close()
    dut.close()


@pytest.fixture
def cmd_collect(request, record_property):
    """Collects all command results for each test and records them for junit"""
    test = {'test_name': request.node.name}
    test['commands'] = []
    print('test_name: {}'.format(request.node.name))

    def _collect(name, value):
        value['node'] = name
        test['commands'].append(value)
        pprint(value)
        return value
    yield _collect
    record_property('test_name', test['test_name'])
    for command in test['commands']:
        record_property(command['cmd'], command)


# pylint: disable=W0621
@pytest.fixture
def nodes(node_setup, cmd_collect):
    """Return to default state after each test is run"""
    phil = node_setup['phil']
    dut = node_setup['dut']

    _reset_nodes(phil)

    # check until the board is connected
    retries = _RETRY_CONNECT
    while retries > 0:
        retries -= 1
        dut_fw_id = cmd_collect('dut', dut.i2c_get_id())["data"]
        if dut_fw_id == [dut.FW_ID]:
            retries = 0

    assert dut_fw_id == [dut.FW_ID]
    assert cmd_collect('dut', dut.i2c_acquire(0))["result"] == 'Success'
    yield node_setup
    cmd_collect('dut', dut.i2c_release(0))

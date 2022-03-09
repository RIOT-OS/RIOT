#! /usr/bin/env python3

# Copyright (C) 2021 HAW Hamburg
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell import ShellInteraction
from riotctrl.shell.json import RapidJSONShellInteractionParser


class TestTuroBase(unittest.TestCase):
    DEBUG = False

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        cls.shell = ShellInteraction(cls.ctrl)
        cls.json_parser = RapidJSONShellInteractionParser()
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()

    def exec_turo_cmd(self, cmd, timeout=-1, async_=False):
        resp = self.shell.cmd(cmd, timeout, async_)
        self.logger.debug(repr(resp))
        return self._parse(resp)

    def _parse(self, resp):
        resp = self.json_parser.parse(resp)
        if resp[-1]['exit_status'] != 0:
            raise RuntimeError("{}".format(resp[-1]))
        if len(resp) == 1:
            return None
        elif len(resp) == 2:
            return resp[0]
        return resp[:-1]


class TestTuro(TestTuroBase):
    def test_turo_simple_s32(self):
        vals = [0, -1, -2147483648, 2147483647]
        for val in vals:
            resp = self.exec_turo_cmd('turo_simple_s32 '
                                      '{}'.format(val))
            assert resp == val

    def test_turo_simple_s32_fail(self):
        vals = ["foo", -2147483649, 2147483648]
        for val in vals:
            with self.assertRaises(RuntimeError):
                self.exec_turo_cmd('turo_simple_s32 '
                                   '{}'.format(val))

    def test_turo_simple_array_u8(self):
        vals = [255, 0, 1]
        cmd = 'turo_simple_array_u8 ' + ' '.join(map(str, vals))
        resp = self.exec_turo_cmd(cmd)
        self.assertCountEqual(resp, vals)

    def test_turo_simple_array_u8_fail(self):
        vals = ["foo", -1, 256]
        for val in vals:
            with self.assertRaises(RuntimeError):
                self.exec_turo_cmd('turo_simple_array_u8 '
                                   '{}'.format(val))

    def test_turo_simple_array_s32(self):
        vals = [0, -1, -2147483648, 2147483647]
        cmd = 'turo_simple_array_s32 ' + ' '.join(map(str, vals))
        resp = self.exec_turo_cmd(cmd)
        self.assertCountEqual(resp, vals)

    def test_turo_simple_array_s32_fail(self):
        vals = ["foo", -2147483649, 2147483648]
        for val in vals:
            with self.assertRaises(RuntimeError):
                self.exec_turo_cmd('turo_simple_array_s32 '
                                   '{}'.format(val))

    def test_turo_simple_dict_string(self):
        test_dict = {'foo': 'bar', 'strnum': '42'}
        for key, val in test_dict.items():
            cmd = 'turo_simple_dict_string {} {}'.format(key, val)
            resp = self.exec_turo_cmd(cmd)
            assert resp[key] == val

    def test_turo_simple_dict_string_fail(self):
        pass

    def test_turo_simple_dict_s32(self):
        test_dict = {'foo': -1, 'bar': 2147483647}
        for key, val in test_dict.items():
            cmd = 'turo_simple_dict_s32 {} {}'.format(key, val)
            resp = self.exec_turo_cmd(cmd)
            assert resp[key] == val

    def test_turo_simple_dict_s32_fail(self):
        with self.assertRaises(RuntimeError):
            self.exec_turo_cmd('turo_simple_dict_s32 foo bar')

    def test_turo_simple_exit_status(self):
        self.exec_turo_cmd('turo_simple_exit_status 0')
        with self.assertRaises(RuntimeError):
            self.exec_turo_cmd('turo_simple_exit_status -1')

    def test_test_multi_element_dict(self):
        test_dict = {'foo': 'bar', 'strnum': '42'}
        cmd = 'test_multi_element_dict'
        for key, val in test_dict.items():
            cmd += ' {} {}'.format(key, val)

        resp = self.exec_turo_cmd(cmd)
        self.assertDictEqual(resp, test_dict)

    def test_test_netif(self):
        resp = self.exec_turo_cmd("test_netif")

        assert resp[1]['netif']['num'] == 6
        assert resp[0]['netif']['num'] == 5
        addr = resp[0]['netif']['inet6 addr']['addr']
        assert addr == 'fe80::2445:7fff:fe5a:6fd9'
        scope = resp[0]['netif']['inet6 addr']['scope']
        assert scope == 'link'

        flags = resp[0]['netif']['flags']
        for flag in flags:
            if isinstance(flag, dict):
                if "MTU" in flag.keys():
                    assert flag['MTU'] == 1500
                    break
        else:
            assert False, "MTU flag does not exist"


if __name__ == '__main__':
    unittest.main()

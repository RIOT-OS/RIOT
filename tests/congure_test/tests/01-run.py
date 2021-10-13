#! /usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import os
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell import ShellInteraction
from riotctrl.shell.json import RapidJSONShellInteractionParser, rapidjson

from riotctrl_shell.congure_test import CongureTest


class TestCongUREBase(unittest.TestCase):
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
        cls.json_parser.set_parser_args(
            parse_mode=rapidjson.PM_TRAILING_COMMAS
        )
        cls.logger = logging.getLogger(cls.__name__)
        if cls.DEBUG:
            cls.logger.setLevel(logging.DEBUG)

    @classmethod
    def tearDownClass(cls):
        cls.ctrl.stop_term()

    def setUp(self):
        self.shell.cmd('cong_clear')

    def exec_cmd(self, cmd, timeout=-1, async_=False):
        res = self.shell.cmd(cmd, timeout, async_)
        self.logger.debug(repr(res))
        if res.strip():
            return self.json_parser.parse(res)
        return None


class TestCongUREWithoutSetup(TestCongUREBase):
    def test_no_setup(self):
        state = self.exec_cmd('state')
        self.assertEqual(state, {
            'driver': '0x00000000',
            'ctx': '0x00000000',
            'cwnd': 0,
            'init': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'ctx': '0x00000000',
                },
            },
            'inter_msg_interval': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msg_size': 0,
                },
            },
            'report_msg_sent': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msg_size': 0,
                },
            },
            'report_msg_discarded': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msg_size': 0,
                },
            },
            'report_msgs_timeout': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msgs': [],
                },
            },
            'report_msgs_lost': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msgs': [],
                },
            },
            'report_msg_acked': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'msg': None,
                    'ack': None,
                },
            },
            'report_ecn_ce': {
                'calls': 0,
                'last_args': {
                    'c': '0x00000000',
                    'time': 0,
                },
            },
        })

    def test_setup_invalid_id(self):
        self.test_no_setup()
        res = self.exec_cmd('cong_setup abcd')
        self.assertEqual(res, {'error': "`id` expected to be integer"})
        res = self.exec_cmd('cong_setup 1')
        self.assertEqual(res, {'error': "`id` is invalid"})

    def test_setup(self):
        self.test_no_setup()
        res = self.exec_cmd('cong_setup')
        # res['success'] is 32-bit hexadecimal number greater zero, representing
        # the pointer to the congure state object
        self.assertGreater(int(res['success'], base=16), 0)
        self.assertEqual(len(res['success']), len('0x00000000'))
        res = self.exec_cmd('cong_setup 0')
        # res['success'] is 32-bit hexadecimal number greater zero, representing
        # the pointer to the congure state object
        self.assertGreater(int(res['success'], base=16), 0)
        self.assertEqual(len(res['success']), len('0x00000000'))

    def test_init_wo_setup(self):
        res = self.exec_cmd('cong_init 0x12345')
        self.assertEqual(res, {'error': "State object not set up"})

    def test_inter_msg_interval_wo_setup(self):
        res = self.exec_cmd('cong_imi 689')
        self.assertEqual(res, {'error': "State object not set up"})

    def test_report_wo_setup(self):
        res = self.exec_cmd('cong_report')
        self.assertEqual(res, {'error': "State object not set up"})


class TestCongUREWithSetup(TestCongUREBase):
    def setUp(self):
        super().setUp()
        res = self.exec_cmd('cong_setup')
        self.congure_state_ptr = int(res['success'], base=16)

    def tearDown(self):
        res = self.exec_cmd('cong_msgs_reset')
        self.assertIn('success', res)

    def test_init_no_args(self):
        res = self.exec_cmd('cong_init')
        self.assertEqual(res, {'error': "`ctx` argument expected"})

    def test_init_arg_not_hex(self):
        res = self.exec_cmd('cong_init foobar')
        self.assertEqual(res, {'error': "`ctx` expected to be hex"})
        res = self.exec_cmd('cong_init 123456')
        self.assertEqual(res, {'error': "`ctx` expected to be hex"})

    def test_init_success(self):
        ctx = 0x12345
        res = self.exec_cmd('cong_init 0x{ctx:x}'.format(ctx=ctx))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['init']['calls'], 1)
        self.assertEqual(int(res['init']['last_args']['c'], base=16),
                         self.congure_state_ptr)
        self.assertEqual(int(res['init']['last_args']['ctx'], base=16),
                         ctx)

    def test_inter_msg_interval_no_args(self):
        res = self.exec_cmd('cong_imi')
        self.assertEqual(res, {'error': '`msg_size` argument expected'})

    def test_inter_msg_interval_not_int(self):
        res = self.exec_cmd('cong_imi foobar')
        self.assertEqual(res, {'error': '`msg_size` expected to be integer'})

    def test_inter_msg_interval_success(self):
        msg_size = 521
        res = self.exec_cmd('cong_imi {msg_size}'.format(msg_size=msg_size))
        assert res == {'success': -1}
        res = self.exec_cmd('state')
        self.assertEqual(res['inter_msg_interval']['calls'], 1)
        self.assertEqual(int(res['inter_msg_interval']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['inter_msg_interval']['last_args']['msg_size'],
                         msg_size)

    def test_report_no_args(self):
        res = self.exec_cmd('cong_report foobar')
        self.assertEqual(res, {'error': 'Unknown command `foobar`'})

    def test_report_unknown_command(self):
        res = self.exec_cmd('cong_report')
        self.assertEqual(res, {'error': 'No report command provided'})

    def test_report_msg_sent_no_args(self):
        res = self.exec_cmd('cong_report msg_sent')
        self.assertEqual(res, {'error': '`msg_size` argument expected'})

    def test_report_msg_sent_not_int(self):
        res = self.exec_cmd('cong_report msg_sent this one')
        self.assertEqual(res, {'error': '`msg_size` expected to be integer'})

    def test_report_msg_sent_success(self):
        msg_size = 1234
        res = self.exec_cmd('cong_report msg_sent {msg_size}'
                            .format(msg_size=msg_size))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_sent']['calls'], 1)
        self.assertEqual(int(res['report_msg_sent']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_sent']['last_args']['msg_size'],
                         msg_size)

    def test_report_msg_discarded_no_args(self):
        res = self.exec_cmd('cong_report msg_discarded')
        self.assertEqual(res, {'error': "`msg_size` argument expected"})

    def test_report_msg_discarded_not_int(self):
        res = self.exec_cmd('cong_report msg_discarded this one')
        self.assertEqual(res, {'error': "`msg_size` expected to be integer"})

    def test_report_msg_discarded_success(self):
        msg_size = 1234
        res = self.exec_cmd('cong_report msg_discarded {msg_size}'
                            .format(msg_size=msg_size))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_discarded']['calls'], 1)
        self.assertEqual(int(res['report_msg_discarded']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_discarded']['last_args']['msg_size'],
                         msg_size)

    def _assert_report_msgs_timeout_lost_empty(self, cmd):
        res = self.exec_cmd('cong_report {cmd}'.format(cmd=cmd))
        self.assertEqual(res['error'], 'Message not initialized')

    def _report_msgs_timeout_lost_not_enough_args(self, cmd, exp_params):
        args = ""
        # gradually append more arguments but never get full set
        for i in range(len(exp_params) - 1):
            args += ' {}'.format(i + 1)
            res = self.exec_cmd('cong_add_msg {args}'
                                .format(args=args))
            self.assertEqual(res, {
                'error': 'At least {} arguments {} expected'
                         .format(len(exp_params),
                                 ', '.join('`{}`'.format(p)
                                           for p in exp_params))
            })
        self._assert_report_msgs_timeout_lost_empty(cmd)

    def _report_msgs_timeout_lost_args_not_int(self, cmd, exp_params):
        # generate list of arguments that are exp_params string parameters
        # and exp_params integer parameters
        args = [chr(i + ord('a')) for i in range(len(exp_params))]
        res = self.exec_cmd('cong_add_msg {}'.format(' '.join(args)))
        self.assertEqual(res, {
            'error': '`{}` expected to be integer'.format(exp_params[0])
        })
        # gradually transform all but the last string to integer and test again
        for i in range(len(exp_params) - 1):
            args[i] = str(i + 1)
            res = self.exec_cmd(
                'cong_add_msg {}'.format(' '.join(args))
            )
            self.assertEqual(res, {
                'error': '`{}` expected to be integer'
                         .format(exp_params[i + 1])
            })
        self._assert_report_msgs_timeout_lost_empty(cmd)

    def _report_msgs_timeout_lost_exceed_msg_pool_size(self, cmd):
        # expected to be set by Makefile
        pool_size = int(os.environ.get('LOST_MSG_POOL_SIZE', 4))
        args = ' '.join('1' for _ in range(3 * pool_size))
        args += ' 1 1 1'
        for _ in range(pool_size):
            res = self.exec_cmd('cong_add_msg 1 1 1')
            self.assertIn('success', res)
        res = self.exec_cmd('cong_add_msg 1 1 1')
        self.assertEqual(res, {
            'error': 'List element pool depleted'
        })

    def _report_msgs_timeout_lost_success(self, cmd):
        msgs = [{'send_time': 76543, 'size': 1234, 'resends': 2},
                {'send_time': 5432, 'size': 987, 'resends': 32}]
        for msg in msgs:
            res = self.exec_cmd(
                'cong_add_msg {msg[send_time]} {msg[size]} {msg[resends]}'
                .format(msg=msg)
            )
        res = self.exec_cmd('cong_report {}'.format(cmd))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_{}'.format(cmd)]['calls'], 1)
        self.assertEqual(int(res['report_{}'.format(cmd)]['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_{}'.format(cmd)]['last_args']['msgs'],
                         msgs)

    def _report_msg_acked_not_enough_args(self, exp_params):
        res = self.exec_cmd('cong_add_msg 0 1 2')
        self.assertIn('success', res)
        args = ""
        # gradually append more arguments but never get full set
        for i in range(len(exp_params) - 1):
            args += ' {}'.format(i + 1)
            res = self.exec_cmd('cong_report msg_acked {args}'
                                .format(args=args))
            self.assertEqual(res, {
                'error': 'At least {} arguments {} expected'
                         .format(len(exp_params),
                                 ', '.join('`{}`'.format(p)
                                           for p in exp_params))
            })

    def _report_msg_acked_args_not_int(self, exp_params):
        res = self.exec_cmd('cong_add_msg 0 1 2')
        self.assertIn('success', res)
        # generate list of arguments that are exp_params string parameters
        args = [chr(i + ord('a')) for i in range(len(exp_params))]
        res = self.exec_cmd('cong_report msg_acked {}'.format(' '.join(args)))
        self.assertEqual(res, {
            'error': '`{}` expected to be integer'.format(exp_params[0])
        })
        # gradually transform all but the last string to integer and test again
        for i in range(len(exp_params) - 1):
            args[i] = str(i + 1)
            res = self.exec_cmd(
                'cong_report msg_acked {}'.format(' '.join(args))
            )
            self.assertEqual(res, {
                'error': '`{}` expected to be integer'
                         .format(exp_params[i + 1])
            })

    def test_report_msgs_timeout_not_enough_args(self):
        self._report_msgs_timeout_lost_not_enough_args(
            'msgs_timeout', ['msg_send_time', 'msg_size', 'msg_resends']
        )

    def test_report_msgs_timeout_args_not_int(self):
        self._report_msgs_timeout_lost_args_not_int(
            'msgs_timeout', ['msg_send_time', 'msg_size', 'msg_resends']
        )

    def test_report_msgs_timeout_exceed_msg_pool_size(self):
        self._report_msgs_timeout_lost_exceed_msg_pool_size('msgs_timeout')

    def test_report_msgs_timeout_success(self):
        self._report_msgs_timeout_lost_success('msgs_timeout')

    def test_report_msgs_lost_not_enough_args(self):
        self._report_msgs_timeout_lost_not_enough_args(
            'msgs_lost', ['msg_send_time', 'msg_size', 'msg_resends']
        )

    def test_report_msgs_lost_msg_args_not_int(self):
        self._report_msgs_timeout_lost_args_not_int(
            'msgs_lost', ['msg_send_time', 'msg_size', 'msg_resends']
        )

    def test_report_msgs_lost_exceed_msg_pool_size(self):
        self._report_msgs_timeout_lost_exceed_msg_pool_size('msgs_lost')

    def test_report_msgs_lost_success(self):
        self._report_msgs_timeout_lost_success('msgs_lost')

    def test_report_msg_acked_not_enough_args(self):
        self._report_msg_acked_not_enough_args([
                'ack_recv_time', 'ack_id', 'ack_size', 'ack_clean', 'ack_wnd',
                'ack_delay',
            ])

    def test_report_msg_acked_msg_args_not_int(self):
        self._report_msg_acked_args_not_int([
                'ack_recv_time', 'ack_id', 'ack_size', 'ack_clean', 'ack_wnd',
                'ack_delay',
            ])

    def test_report_msg_acked_no_msg(self):
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': 17440, 'delay': 33325}
        res = self.exec_cmd(
            'cong_report msg_acked '
            '{ack[recv_time]} {ack[id]} {ack[size]} {ack[clean]} '
            '{ack[wnd]} {ack[delay]}'.format(ack=ack)
        )
        self.assertEqual(res['error'], 'Message not initialized')

    def test_report_msg_acked_wnd_not_wnd_size(self):
        msg = {'send_time': 2862350241, 'size': 14679, 'resends': 0}
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': (1 << 16) + 17440,
               'delay': 33325}
        res = self.exec_cmd('cong_add_msg {msg[send_time]} {msg[size]} '
                            '{msg[resends]}'.format(msg=msg))
        self.assertIn('success', res)
        res = self.exec_cmd(
            'cong_report msg_acked '
            '{ack[recv_time]} {ack[id]} {ack[size]} {ack[clean]} '
            '{ack[wnd]} {ack[delay]}'.format(ack=ack)
        )
        self.assertEqual(res, {
            'error': '`ack_wnd` not 16 bit wide'
        })

    def test_report_msg_acked_delay_not_uint16(self):
        msg = {'send_time': 2862350241, 'size': 14679, 'resends': 0}
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': 17440, 'delay': (1 << 16) + 33325}
        res = self.exec_cmd('cong_add_msg {msg[send_time]} {msg[size]} '
                            '{msg[resends]}'.format(msg=msg))
        self.assertIn('success', res)
        res = self.exec_cmd(
            'cong_report msg_acked '
            '{ack[recv_time]} {ack[id]} {ack[size]} {ack[clean]} '
            '{ack[wnd]} {ack[delay]}'.format(ack=ack)
        )
        self.assertEqual(res, {
            'error': '`ack_delay` not 16 bit wide'
        })

    def test_report_msg_acked_success(self):
        msg = {'send_time': 2862350241, 'size': 14679, 'resends': 0}
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': 17440, 'delay': 33325}
        res = self.exec_cmd('cong_add_msg {msg[send_time]} {msg[size]} '
                            '{msg[resends]}'.format(msg=msg))
        self.assertIn('success', res)
        res = self.exec_cmd(
            'cong_report msg_acked '
            '{ack[recv_time]} {ack[id]} {ack[size]} {ack[clean]} '
            '{ack[wnd]} {ack[delay]}'.format(ack=ack)
        )
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_acked']['calls'], 1)
        self.assertEqual(int(res['report_msg_acked']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_acked']['last_args']['msg'], msg)
        self.assertEqual(res['report_msg_acked']['last_args']['ack'], ack)

    def test_report_ecn_ce_no_args(self):
        res = self.exec_cmd('cong_report ecn_ce')
        self.assertEqual(res, {'error': '`time` argument expected'})

    def test_report_ecn_ce_not_int(self):
        res = self.exec_cmd('cong_report ecn_ce this one')
        self.assertEqual(res, {'error': '`time` expected to be integer'})

    def test_report_ecn_ce_success(self):
        time = 64352
        res = self.exec_cmd('cong_report ecn_ce {time}'.format(time=time))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_ecn_ce']['calls'], 1)
        self.assertEqual(int(res['report_ecn_ce']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_ecn_ce']['last_args']['time'],
                         time)


class TestCongUREWithCongureTestSI(TestCongUREBase):
    @classmethod
    def setUpClass(cls):
        super(cls, cls).setUpClass()
        cls.shell = CongureTest(cls.ctrl)

    def call_method(self, method, *args, timeout=-1, async_=False, **kwargs):
        res = getattr(self.shell, method)(*args,
                                          timeout=timeout, async_=async_,
                                          **kwargs)
        self.logger.debug(repr(res))
        if res.strip():
            return self.json_parser.parse(res)
        return None

    def setUp(self):
        super().setUp()
        res = self.call_method('setup')
        self.congure_state_ptr = int(res['success'], base=16)

    def tearDown(self):
        res = self.call_method('msgs_reset')
        self.assertIn('success', res)

    def test_init_success(self):
        ctx = 0x12345
        res = self.call_method('init', ctx=ctx)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['init']['calls'], 1)
        self.assertEqual(int(res['init']['last_args']['c'], base=16),
                         self.congure_state_ptr)
        self.assertEqual(int(res['init']['last_args']['ctx'], base=16),
                         ctx)

    def test_inter_msg_interval_success(self):
        msg_size = 521
        res = self.call_method('inter_msg_interval', msg_size=msg_size)
        assert res == {'success': -1}
        res = self.exec_cmd('state')
        self.assertEqual(res['inter_msg_interval']['calls'], 1)
        self.assertEqual(int(res['inter_msg_interval']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['inter_msg_interval']['last_args']['msg_size'],
                         msg_size)

    def test_report_unknown_command(self):
        res = self.call_method('report', 'foobar')
        self.assertEqual(res, {'error': 'Unknown command `foobar`'})

    def test_report_msg_sent_success(self):
        msg_size = 1234
        res = self.call_method('report_msg_sent', msg_size=msg_size)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_sent']['calls'], 1)
        self.assertEqual(int(res['report_msg_sent']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_sent']['last_args']['msg_size'],
                         msg_size)

    def test_report_msg_discarded_success(self):
        msg_size = 1234
        res = self.call_method('report_msg_discarded', msg_size=msg_size)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_discarded']['calls'], 1)
        self.assertEqual(int(res['report_msg_discarded']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_discarded']['last_args']['msg_size'],
                         msg_size)

    def _report_msgs_timeout_lost_base_success(self, cmd):
        msgs = [{'send_time': 76543, 'size': 1234, 'resends': 2},
                {'send_time': 5432, 'size': 987, 'resends': 32}]
        for msg in msgs:
            res = self.call_method('add_msg', **msg)
        res = self.call_method('report_{}_base'.format(cmd))
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_{}'.format(cmd)]['calls'], 1)
        self.assertEqual(int(res['report_{}'.format(cmd)]['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_{}'.format(cmd)]['last_args']['msgs'],
                         msgs)

    def _report_msgs_timeout_lost_success(self, cmd):
        msgs = [{'send_time': 76543, 'size': 1234, 'resends': 2},
                {'send_time': 5432, 'size': 987, 'resends': 32}]
        res = self.call_method('report_{}'.format(cmd), msgs=msgs)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_{}'.format(cmd)]['calls'], 1)
        self.assertEqual(int(res['report_{}'.format(cmd)]['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_{}'.format(cmd)]['last_args']['msgs'],
                         msgs)

    def test_report_msgs_timeout_base_success(self):
        self._report_msgs_timeout_lost_base_success('msgs_timeout')

    def test_report_msgs_lost_base_success(self):
        self._report_msgs_timeout_lost_base_success('msgs_lost')

    def test_report_msgs_timeout_success(self):
        self._report_msgs_timeout_lost_success('msgs_timeout')

    def test_report_msgs_lost_success(self):
        self._report_msgs_timeout_lost_success('msgs_lost')

    def test_report_msg_acked_base_success(self):
        msg = {'send_time': 2862350241, 'size': 14679, 'resends': 0}
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': 17440, 'delay': 33325}
        res = self.call_method('add_msg', **msg)
        self.assertIn('success', res)
        res = self.call_method(
            'report_msg_acked_base',
            **{'ack_{}'.format(k): v for k, v in ack.items()})
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_acked']['calls'], 1)
        self.assertEqual(int(res['report_msg_acked']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_acked']['last_args']['msg'], msg)
        self.assertEqual(res['report_msg_acked']['last_args']['ack'], ack)

    def test_report_msg_acked_success(self):
        msg = {'send_time': 2862350241, 'size': 14679, 'resends': 0}
        ack = {'recv_time': 2862350405, 'id': 1197554483, 'size': 14667,
               'clean': 1, 'wnd': 17440, 'delay': 33325}
        res = self.call_method('report_msg_acked', msg=msg, ack=ack)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_msg_acked']['calls'], 1)
        self.assertEqual(int(res['report_msg_acked']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_msg_acked']['last_args']['msg'], msg)
        self.assertEqual(res['report_msg_acked']['last_args']['ack'], ack)

    def test_report_ecn_ce_success(self):
        time = 64352
        res = self.call_method('report_ecn_ce', time=time)
        self.assertIsNone(res['success'])
        res = self.exec_cmd('state')
        self.assertEqual(res['report_ecn_ce']['calls'], 1)
        self.assertEqual(int(res['report_ecn_ce']['last_args']['c'],
                             base=16),
                         self.congure_state_ptr)
        self.assertEqual(res['report_ecn_ce']['last_args']['time'],
                         time)


if __name__ == '__main__':
    unittest.main()

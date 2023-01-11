#! /usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import sys
import time
import unittest

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell.json import RapidJSONShellInteractionParser, rapidjson

from riotctrl_shell.congure_test import CongureTest

from testrunner.spawn import MAKE_TERM_CONNECT_DELAY


class TestCongUREBase(unittest.TestCase):
    # pylint: disable=too-many-public-methods
    # it's just one more ...
    DEBUG = False

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
        time.sleep(MAKE_TERM_CONNECT_DELAY)
        cls.ctrl.start_term()
        if cls.DEBUG:
            cls.ctrl.term.logfile = sys.stdout
        cls.shell = CongureTest(cls.ctrl)
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
        self.shell.clear()

    def tearDown(self):
        self.shell.msgs_reset()

    def _parse(self, res):
        self.logger.debug(res)
        if res.strip():
            return self.json_parser.parse(res)
        return None

    def exec_cmd(self, cmd, timeout=-1, async_=False):
        res = self.shell.cmd(cmd, timeout, async_)
        return self._parse(res)

    def assertSlowStart(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """
        > The slow start algorithm is used when cwnd < ssthresh, while the
        > congestion avoidance algorithm is used when cwnd > ssthresh.  When
        > cwnd and ssthresh are equal, the sender may use either slow start or
        > congestion avoidance.
        """
        self.assertLess(state['cwnd'], state['ssthresh'])

    def assertInFastRetransmit(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """
        > The TCP sender SHOULD use the "fast retransmit" algorithm to detect
        > and repair loss, based on incoming duplicate ACKs.  The fast
        > retransmit algorithm uses the arrival of 3 duplicate ACKs [...] as
        > an indication that a segment has been lost.
        """
        self.assertGreaterEqual(state['dup_acks'], state['consts']['frthresh'])

    def assertNotInFastRetransmit(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """Reverse of self.assertInFastRetransmit()"""
        self.assertLess(state['dup_acks'], state['consts']['frthresh'])

    def get_ff_calls(self):
        res = self.exec_cmd('get_ff_calls')
        return res['fr_calls']

    def set_same_wnd_adv(self, value):
        self.exec_cmd(f'set_same_wnd_adv {value:d}')

    def set_cwnd(self, cwnd):
        self.exec_cmd(f'set_cwnd {cwnd}')

    def cong_state(self):
        return self.exec_cmd('state')

    def cong_init(self, ctx=0):
        res = self.shell.init(ctx)
        return self._parse(res)

    def cong_report_msg_sent(self, msg_size):
        res = self.shell.report_msg_sent(msg_size)
        return self._parse(res)

    def cong_report_msg_acked(self, msg, ack):
        res = self.shell.report_msg_acked(msg, ack)
        return self._parse(res)

    def cong_report_ecn_ce(self, time):
        res = self.shell.report_ecn_ce(time)
        return self._parse(res)


class TestCongUREABEWithoutSetup(TestCongUREBase):
    def test_no_setup(self):
        state = self.exec_cmd('state')
        self.assertEqual(state, {
            'cwnd': 0,
            'consts': None,
            'mss': 0,
            'last_ack': 0,
            'ssthresh': 0,
            'in_flight_size': 0,
            'dup_acks': 0,
        })


class TestCongUREABEDefaultInitTests(TestCongUREBase):
    def setUp(self):
        super().setUp()
        res = self.shell.setup(0)
        self.assertIn('success', res)

    def test_setup(self):
        state = self.cong_state()
        self.assertIsNotNone(state['consts'])
        # fast_retransmit and same_window_advertised need to be set to a
        # function pointer
        self.assertNotEqual(int(state['consts']['fr'], base=16), 0)
        self.assertNotEqual(int(state['consts']['same_wnd_adv'], base=16), 0)
        # ss_cwnd_inc, ca_cwnd_inc, fr_cwnd_dec are optional and setup 0 need
        # to be set to a function pointer
        self.assertEqual(int(state['consts']['ss_cwnd_inc'], base=16), 0)
        self.assertEqual(int(state['consts']['ca_cwnd_inc'], base=16), 0)
        self.assertEqual(int(state['consts']['fr_cwnd_dec'], base=16), 0)
        self.assertEqual(state['consts']['init_mss'], 1460)
        self.assertEqual(state['consts']['cwnd_lower'], 1095)
        self.assertEqual(state['consts']['cwnd_upper'], 2190)
        self.assertEqual(state['consts']['init_ssthresh'], 0xffff)
        self.assertEqual(state['consts']['frthresh'], 3)
        # https://tools.ietf.org/html/rfc8511#section-3.1
        beta_ecn = (state['consts']['abe_multiplier_numerator'] /
                    state['consts']['abe_multiplier_denominator'])
        self.assertAlmostEqual(beta_ecn, 0.8)
        self.assertEqual(state['consts']['frthresh'], 3)

    def test_init(self):
        """
        This is inherited from `congure_reno`, so it should be the same as
        in `tests/congure_reno`.

        https://tools.ietf.org/html/rfc5681#section-3.1

        > IW, the initial value of cwnd, MUST be set using the following
        > guidelines as an upper bound.
        >
        > If SMSS > 2190 bytes:
        >     IW = 2 * SMSS bytes and MUST NOT be more than 2 segments
        > If (SMSS > 1095 bytes) and (SMSS <= 2190 bytes):
        >     IW = 3 * SMSS bytes and MUST NOT be more than 3 segments
        > if SMSS <= 1095 bytes:
        >     IW = 4 * SMSS bytes and MUST NOT be more than 4 segments
        """
        res = self.cong_init()
        self.assertIn('success', res)
        state = self.cong_state()
        self.assertEqual(state['consts']['init_mss'], state['mss'])
        # (SMSS > 1095 bytes)
        self.assertGreater(state['mss'], state['consts']['cwnd_lower'])
        # (SMSS <= 2190 bytes)
        self.assertLessEqual(state['mss'], state['consts']['cwnd_upper'])
        # as such, IW = 3 * SMSS bytes
        self.assertEqual(state['cwnd'], 3 * state['mss'])
        # We start with slow start
        self.assertSlowStart(state)
        self.assertNotInFastRetransmit(state)


class TestCongUREABE(TestCongUREBase):
    """
    Most functionality should be the same as for `congure_reno`, except
    for the behavior of `report_ecn_ce`. So only test some basics from
    `tests/congure_reno` and focus testing on `report_ecn_ce`
    """
    def setUp(self):
        super().setUp()
        res = self.shell.setup(0)
        self.assertIn('success', res)
        res = self.cong_init()
        self.assertIn('success', res)

    def _send_msg_and_recv_ack(self, msg_size, msg_resends=0,
                               ack_id=15, ack_size=None, ack_clean=True):
        # pylint: disable=too-many-arguments
        # already reduced number of arguments, cong_report_msg_acked would
        # need...
        if ack_size is None:
            # set ack_size to arbitrary value
            ack_size = msg_size
        res = self.cong_report_msg_sent(msg_size=msg_size)
        self.assertIn('success', res)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], msg_size)
        res = self.cong_report_msg_acked(
            msg={'send_time': 1000, 'size': msg_size, 'resends': msg_resends},
            ack={'recv_time': 1100, 'id': ack_id, 'size': ack_size,
                 'clean': ack_clean, 'wnd': 1234, 'delay': 0},
        )
        self.assertIn('success', res)

    def test_slow_start_increase(self):
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        """
        See test_slow_start_increase_large_N() from `tests/congure_reno`
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_mss = state['mss']
        init_ssthresh = state['ssthresh']
        self.assertEqual(state['in_flight_size'], 0)
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        # set N to larger than SMSS
        N = state['mss'] + 1337
        self._send_msg_and_recv_ack(N)
        state = self.cong_state()
        # MSS did not change
        self.assertEqual(state['mss'], init_mss)
        self.assertEqual(state['cwnd'], init_cwnd + state['mss'])
        self.assertEqual(state['in_flight_size'], 0)
        self.assertEqual(state['ssthresh'], init_ssthresh)
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit(self):
        """
        See self.test_enter_fast_retransmit_all_check_true() from
        `tests/congure_reno`
        """
        state = self.cong_state()
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)
        self.assertEqual(state['in_flight_size'], 0)
        self._send_msg_and_recv_ack(42, ack_id=15, ack_size=0, ack_clean=True)
        # make condition (a) true
        self.cong_report_msg_sent(52)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 52)
        # make condition (e) not true
        self.set_same_wnd_adv(True)
        # condition (b) ack['size'] == 0, (c) ack['clean'] == True,
        # (d) ack['id'] == 15
        for _ in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15, 'size': 0,
                     'clean': True, 'wnd': 1234, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(1, self.get_ff_calls())
        self.assertInFastRetransmit(self.cong_state())

    def test_ecn_ce_small_flight_size(self):
        """
        https://tools.ietf.org/html/rfc8511#section-3

        > As permitted by RFC 8311, this document specifies a sender-side
        > change to TCP where receipt of a packet with the ECN-Echo flag SHOULD
        > trigger the TCP source to set the slow start threshold (ssthresh) to
        > 0.8 times the FlightSize, with a lower bound of 2 * SMSS applied to
        > the result (where SMSS stands for Sender Maximum Segment Size)).  As
        > in [RFC5681], the TCP sender also reduces the cwnd value to no more
        > than the new ssthresh value.  Section 6.1.2 of RFC 3168 provides
        > guidance on setting a cwnd less than 2 * SMSS.
        """
        state = self.cong_state()
        beta_ecn = (state['consts']['abe_multiplier_numerator'] /
                    state['consts']['abe_multiplier_denominator'])
        self.assertAlmostEqual(beta_ecn, 0.8)
        flight_size = 150
        # put some bytes in the air
        self.cong_report_msg_sent(flight_size)
        self.cong_report_ecn_ce(1204)
        state = self.cong_state()
        # [...] set the slow start threshold (ssthresh) to 0.8 times the
        # FlightSize, with a lower bound of 2 * SMSS applied to the result
        self.assertEqual(state['ssthresh'], 2 * state['mss'])
        # the TCP sender also reduces the cwnd value to no more
        # than the new ssthresh value
        self.assertLessEqual(state['cwnd'], state['ssthresh'])

    def test_ecn_ce_large_flight_size(self):
        """
        Same as test_ecn_ce_small_flight_size, but with flight size
        larger than (2 * SMSS / 0.8)
        """
        state = self.cong_state()
        beta_ecn = (state['consts']['abe_multiplier_numerator'] /
                    state['consts']['abe_multiplier_denominator'])
        self.assertAlmostEqual(beta_ecn, 0.8)
        flight_size = 3 * state['mss']
        # increase congestion window large enough to send all those bytes
        self.set_cwnd(flight_size)
        # put some bytes in the air
        for _ in range(3):
            self.cong_report_msg_sent(state['mss'])
        self.cong_report_ecn_ce(1204)
        state = self.cong_state()
        # [...] set the slow start threshold (ssthresh) to 0.8 times the
        # FlightSize, with a lower bound of 2 * SMSS applied to the result
        self.assertEqual(state['ssthresh'], beta_ecn * flight_size)
        # the TCP sender also reduces the cwnd value to no more
        # than the new ssthresh value
        self.assertLessEqual(state['cwnd'], state['ssthresh'])


if __name__ == '__main__':
    unittest.main()

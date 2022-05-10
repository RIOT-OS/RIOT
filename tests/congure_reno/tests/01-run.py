#! /usr/bin/env python3

# Copyright (C) 2021 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import logging
import sys
import unittest

from riotctrl.ctrl import RIOTCtrl
from riotctrl.shell.json import RapidJSONShellInteractionParser, rapidjson

from riotctrl_shell.congure_test import CongureTest


class TestCongUREBase(unittest.TestCase):
    # pylint: disable=too-many-public-methods
    # it's just one more ...
    DEBUG = False

    @classmethod
    def setUpClass(cls):
        cls.ctrl = RIOTCtrl()
        cls.ctrl.reset()
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

    def assertCongestionAvoidance(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """
        > The slow start algorithm is used when cwnd < ssthresh, while the
        > congestion avoidance algorithm is used when cwnd > ssthresh.  When
        > cwnd and ssthresh are equal, the sender may use either slow start or
        > congestion avoidance.
        """
        self.assertGreaterEqual(state['cwnd'], state['ssthresh'])

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
        self.exec_cmd('set_same_wnd_adv {value:d}'.format(value=value))

    def set_mss(self, mss):
        self.exec_cmd('set_mss {mss}'.format(mss=mss))

    def set_cwnd(self, cwnd):
        self.exec_cmd('set_cwnd {cwnd}'.format(cwnd=cwnd))

    def set_ssthresh(self, ssthresh):
        self.exec_cmd('set_ssthresh {ssthresh}'.format(ssthresh=ssthresh))

    def cong_state(self):
        return self.exec_cmd('state')

    def cong_init(self, ctx=0):
        res = self.shell.init(ctx)
        return self._parse(res)

    def cong_report_msg_sent(self, msg_size):
        res = self.shell.report_msg_sent(msg_size)
        return self._parse(res)

    def cong_report_msg_discarded(self, msg_size):
        res = self.shell.report_msg_discarded(msg_size)
        return self._parse(res)

    def cong_report_msgs_timeout(self, msgs):
        res = self.shell.report_msgs_timeout(msgs)
        return self._parse(res)

    def cong_report_msgs_lost(self, msgs):
        res = self.shell.report_msgs_lost(msgs)
        return self._parse(res)

    def cong_report_msg_acked(self, msg, ack):
        res = self.shell.report_msg_acked(msg, ack)
        return self._parse(res)

    def cong_report_ecn_ce(self, time):
        res = self.shell.report_ecn_ce(time)
        return self._parse(res)

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
        # this method is reused a lot, so reset internal message buffer of
        # `congure_test`
        res = self._parse(self.shell.msgs_reset())
        self.assertIn('success', res)


class TestCongURERenoWithoutSetup(TestCongUREBase):
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


class TestCongURERenoDefaultInitTests(TestCongUREBase):
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

    def test_init(self):
        """
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


class TestCongURERenoDefault(TestCongUREBase):
    def setUp(self):
        super().setUp()
        res = self.shell.setup(0)
        self.assertIn('success', res)
        res = self.cong_init()
        self.assertIn('success', res)

    def test_mss_2200(self):
        """
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
        self.set_mss(2200)
        state = self.cong_state()
        self.assertEqual(2200, state['mss'])
        # (SMSS > 2190 bytes)
        self.assertGreater(state['mss'], state['consts']['cwnd_upper'])
        # (SMSS > 1095 bytes)
        self.assertGreater(state['mss'], state['consts']['cwnd_lower'])
        # as such, IW = 2 * SMSS bytes
        self.assertEqual(state['cwnd'], 2 * state['mss'])
        # We start with slow start
        self.assertSlowStart(state)
        self.assertNotInFastRetransmit(state)

    def test_mss_1095(self):
        """
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
        self.set_mss(1095)
        state = self.cong_state()
        self.assertEqual(1095, state['mss'])
        # (SMSS < 2190 bytes)
        self.assertLess(state['mss'], state['consts']['cwnd_upper'])
        # (SMSS < 1095 bytes)
        self.assertLessEqual(state['mss'], state['consts']['cwnd_lower'])
        # as such, IW = 4 * SMSS bytes
        self.assertEqual(state['cwnd'], 4 * state['mss'])
        # We start with slow start
        self.assertSlowStart(state)
        self.assertNotInFastRetransmit(state)

    def test_slow_start_increase_small_N(self):
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        """
        https://tools.ietf.org/html/rfc5681#section-3.1

        > During slow start, a TCP increments cwnd by at most SMSS bytes for
        > each ACK received that cumulatively acknowledges new data.  Slow
        > start ends when cwnd exceeds ssthresh (or, optionally, when it
        > reaches it, as noted above) or when congestion is observed.  While
        > traditionally TCP implementations have increased cwnd by precisely
        > SMSS bytes upon receipt of an ACK covering new data, we RECOMMEND
        > that TCP implementations increase cwnd, per:
        >
        >    cwnd += min (N, SMSS)                      (2)
        >
        > where N is the number of previously unacknowledged bytes acknowledged
        > in the incoming ACK.
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertEqual(state['in_flight_size'], 0)
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        N = 42
        self._send_msg_and_recv_ack(N)
        state = self.cong_state()
        self.assertEqual(state['cwnd'], init_cwnd + N)
        self.assertEqual(state['in_flight_size'], 0)
        self.assertEqual(state['ssthresh'], init_ssthresh)
        self.assertNotInFastRetransmit(state)

    def test_slow_start_increase_large_N(self):
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        """
        Same as test_slow_start_increase_small_N(), but with N larger than SMSS
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

    def test_congestion_avoidance_increase(self):
        """
        During congestion avoidance, cwnd is incremented by roughly 1 full-
        sized segment per round-trip time (RTT).  Congestion avoidance
        continues until congestion is detected.  The basic guidelines for
        incrementing cwnd during congestion avoidance are:

           * MAY increment cwnd by SMSS bytes

           * SHOULD increment cwnd per equation (2) once per RTT

           * MUST NOT increment cwnd by more than SMSS bytes
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_mss = state['mss']
        init_ssthresh = init_cwnd - init_mss
        self.set_ssthresh(init_ssthresh)
        state = self.cong_state()
        self.assertCongestionAvoidance(state)
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        N = 42
        self._send_msg_and_recv_ack(N)
        state = self.cong_state()
        # cwnd was incremented
        self.assertGreater(state['cwnd'], init_cwnd)
        # cwnd was not incremented by more than SMSS bytes
        self.assertLessEqual(state['cwnd'], init_cwnd + init_mss)
        self.assertNotInFastRetransmit(state)

    def _send_msg_and_timeout(self, msgs):
        state = self.cong_state()
        flight_size = state['in_flight_size']
        for msg in msgs:
            res = self.cong_report_msg_sent(msg_size=msg['size'])
            self.assertIn('success', res)
            flight_size += msg['size']
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], flight_size)
        res = self.cong_report_msgs_timeout(msgs)
        self.assertIn('success', res)

    def test_reduce_ssthresh_small_flight_size(self):
        """
        https://tools.ietf.org/html/rfc5681#section-3.1

        > When a TCP sender detects segment loss using the retransmission timer
        > and the given segment has not yet been resent by way of the
        > retransmission timer, the value of ssthresh MUST be set to no more
        > than the value given in equation (4):
        >
        >    ssthresh = max (FlightSize / 2, 2*SMSS)            (4)
        >
        > where, as discussed above, FlightSize is the amount of outstanding
        > data in the network.
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_mss = state['mss']
        init_ssthresh = state['ssthresh']
        self.assertNotEqual(init_cwnd, init_mss)
        self.assertEqual(state['in_flight_size'], 0)
        flight_size = 42
        self._send_msg_and_timeout([{'size': flight_size, 'send_time': 1000,
                                     'resends': 0}])
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 0)
        # MSS did not change
        self.assertEqual(state['mss'], init_mss)
        """
        Furthermore, upon a timeout (as specified in [RFC2988]) cwnd MUST be
        set to no more than the loss window, LW, which equals 1 full-sized
        segment (regardless of the value of IW).
        """
        self.assertEqual(state['cwnd'], init_mss)
        # slow-start threshold changed
        self.assertNotEqual(state['ssthresh'], init_ssthresh)
        # and became slow-start threshold became 2*SMSS
        self.assertEqual(state['ssthresh'], init_mss * 2)
        # we should still be in slow start, as cwnd == SMSS and
        # ssthresh == 2 * SMSS
        self.assertSlowStart(state)
        self.assertNotInFastRetransmit(state)

    def test_reduce_ssthresh_large_flight_size(self):
        """
        Same as test_reduce_ssthresh_small_flight_size, but with flight size
        larger than 4 * SMSS
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_mss = state['mss']
        init_ssthresh = state['ssthresh']
        self.assertNotEqual(init_cwnd, init_mss)
        self.assertEqual(state['in_flight_size'], 0)
        flight_size = (4 * init_mss) + 543
        # increase congestion window large enough to send all those bytes
        self.set_cwnd(flight_size)
        self._send_msg_and_timeout([
            {'size': init_mss, 'send_time': 1000, 'resends': 0},
            {'size': init_mss, 'send_time': 1010, 'resends': 0},
            {'size': init_mss, 'send_time': 1020, 'resends': 0},
            {'size': init_mss, 'send_time': 1030, 'resends': 0},
            {'size': 543, 'send_time': 140, 'resends': 0},
        ])
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 0)
        # MSS did not change
        self.assertEqual(state['mss'], init_mss)
        # cwnd became SMSS due to timeout
        self.assertEqual(state['cwnd'], init_mss)
        # slow-start threshold changed
        self.assertNotEqual(state['ssthresh'], init_ssthresh)
        # and became slow-start threshold became flight_size / 2
        self.assertEqual(state['ssthresh'], flight_size // 2)
        # we should still be in slow start, as cwnd == SMSS and
        # ssthresh == flight_size / 2 == ((2 * SMSS) + (543 / 2))
        self.assertSlowStart(state)
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_check_a_not_true(self):
        """
        Fast retransmit according to RFC 5681 when receiving a duplicate
        ACK is received (see RFC 5681, section 3.2).

        > The TCP sender SHOULD use the "fast retransmit" algorithm to detect
        >    and repair loss, based on incoming duplicate ACKs.  The fast
        >    retransmit algorithm uses the arrival of 3 duplicate ACKs [...]
        >    as an indication that a segment has been lost.

        A duplicate ACK according to
        https://tools.ietf.org/html/rfc5681#section-2 is defined as

        > DUPLICATE ACKNOWLEDGMENT: An acknowledgment is considered a
        >    "duplicate" in the following algorithms when (a) the receiver of
        >    the ACK has outstanding data, (b) the incoming acknowledgment
        >    carries no data, (c) the SYN and FIN bits are both off, (d) the
        >    acknowledgment number is equal to the greatest acknowledgment
        >    received on the given connection (TCP.UNA from [RFC793]) and (e)
        >    the advertised window in the incoming acknowledgment equals the
        >    advertised window in the last incoming acknowledgment.

        This test checks if fast retransmit is NOT entered when condition (a)
        is not true but all others are
        """
        state = self.cong_state()
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)
        self.assertEqual(state['in_flight_size'], 0)
        self._send_msg_and_recv_ack(42, ack_id=15, ack_size=0, ack_clean=True)
        # condition (a) is not fulfilled
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 0)
        # make condition (e) true
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
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_check_b_not_true(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is NOT entered when condition (b)
        is not true but all others are
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
        # make condition (e) true
        self.set_same_wnd_adv(True)
        # condition (c) ack['clean'] == True, (d) ack['id'] == 15
        # condition (b) not true by setting ack['size'] > 0
        for _ in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15, 'size': 643,
                     'clean': True, 'wnd': 1234, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_check_c_not_true(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is NOT entered when condition (c)
        is not true but all others are
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
        # make condition (e) true
        self.set_same_wnd_adv(True)
        # condition (b) ack['size'] == 0, (d) ack['id'] == 15
        # condition (c) not true by setting ack['clean'] = False
        for _ in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15, 'size': 0,
                     'clean': False, 'wnd': 1234, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_check_d_not_true(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is NOT entered when condition (d)
        is not true but all others are
        """
        state = self.cong_state()
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)
        self.assertEqual(state['in_flight_size'], 0)
        self._send_msg_and_recv_ack(42, ack_id=15, ack_size=0, ack_clean=True)
        # make condition (a) true but also make receiving more ACKs possible
        self.cong_report_msg_sent(42 * 3)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 42 * 3)
        # make condition (e) true
        self.set_same_wnd_adv(True)
        # condition (b) ack['size'] == 0, (c) ack['clean'] == True
        # condition (d) not true by setting ack['id'] = 15 + i
        for i in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15 + i, 'size': 0,
                     'clean': True, 'wnd': 1234, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_check_e_not_true(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is NOT entered when condition (e)
        is not true but all others are
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
        self.set_same_wnd_adv(False)
        # condition (b) ack['size'] == 0, (c) ack['clean'] == True,
        # (d) ack['id'] == 15
        for _ in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15, 'size': 0,
                     'clean': True, 'wnd': 1234, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(0, self.get_ff_calls())
        self.assertNotInFastRetransmit(state)

    def test_enter_fast_retransmit_all_check_true_1(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is entered all conditions are true
        (in the sense that a window is advertised and the current send window
        is the same)
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
        # make condition (e) true
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

    def test_enter_fast_retransmit_all_check_true_2(self):
        """
        See self.test_enter_fast_retransmit_check_a_not_true()
        This test checks if fast retransmit is entered all conditions are true
        (in the sense that a window is not advertised and thus it is not
        comparable to the send window)
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
        # return false for same_wnd_adv, just to make sure that `wnd == 0` is
        # the condition that makes (e) go through
        self.set_same_wnd_adv(False)
        # condition (b) ack['size'] == 0, (c) ack['clean'] == True,
        # (d) ack['id'] == 15, (e) ack['wnd'] == 0
        for _ in range(3):
            res = self.cong_report_msg_acked(
                msg={'send_time': 1000, 'size': 42, 'resends': 0},
                ack={'recv_time': 1100, 'id': 15, 'size': 0,
                     'clean': True, 'wnd': 0, 'delay': 0},
            )
            self.assertIn('success', res)
        self.assertEqual(1, self.get_ff_calls())
        self.assertInFastRetransmit(self.cong_state())

    def test_ecn_ce(self):
        """
        https://tools.ietf.org/html/rfc5681#section-3

        > Also, note that the algorithms specified in this document work in
        > terms of using loss as the signal of congestion.  Explicit Congestion
        > Notification (ECN) could also be used as specified in [RFC3168].

        https://tools.ietf.org/html/rfc3168#section-6.1.2

        > If the sender receives an ECN-Echo (ECE) ACK
        > packet (that is, an ACK packet with the ECN-Echo flag set in the TCP
        > header), then the sender knows that congestion was encountered in the
        > network on the path from the sender to the receiver.  The indication
        > of congestion should be treated just as a congestion loss in non-
        > ECN-Capable TCP. That is, the TCP source halves the congestion window
        > "cwnd" and reduces the slow start threshold "ssthresh".  The sending
        > TCP SHOULD NOT increase the congestion window in response to the
        > receipt of an ECN-Echo ACK packet.

        And https://tools.ietf.org/html/rfc8311#section-4.1

        > The specific change to RFC 3168 is to insert the words "Unless
        > otherwise specified by an Experimental RFC in the IETF document
        > stream" at the beginning of the second sentence quoted above.
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.cong_report_ecn_ce(1204)
        state = self.cong_state()
        # the TCP source halves the congestion window
        self.assertEqual(state['cwnd'], init_cwnd // 2)
        # and reduces the slow start threshold "ssthresh"
        self.assertLess(state['ssthresh'], init_ssthresh)

    def test_msg_discarded(self):
        """
        RFC 5681 does not say anything about discarding messages. It's a
        feature of CongURE. Calling it, should reduce the `in_flight_size`.
        """
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 0)
        self.cong_report_msg_sent(42)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 42)
        self.cong_report_msg_discarded(42)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], 0)

    def test_msgs_lost(self):
        """
        RFC 5681 does not say anything about explicitly lost messages.
        It's a feature of CongURE. Calling it, should enforce fast retransmit,
        as implicit loss (3 duplicate ACKs) does the same.
        """
        state = self.cong_state()
        msgs = [{'size': 24, 'send_time': 1000, 'resends': 0}]
        flight_size = state['in_flight_size']
        for msg in msgs:
            res = self.cong_report_msg_sent(msg_size=msg['size'])
            self.assertIn('success', res)
            flight_size += msg['size']
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], flight_size)
        res = self.cong_report_msgs_lost(msgs)
        self.assertIn('success', res)
        state = self.cong_state()
        self.assertEqual(1, self.get_ff_calls())
        self.assertInFastRetransmit(state)
        # explicitly lost messages are not in flight anymore
        self.assertEqual(state['in_flight_size'], 0)


class TestCongURERenoCustomIncDec(TestCongUREBase):
    def setUp(self):
        self.shell.clear()
        res = self.shell.setup(1)
        self.assertIn('success', res)
        res = self.cong_init()
        self.assertIn('success', res)

    def test_custom_functions_initialized(self):
        state = self.cong_state()
        self.assertNotEqual(int(state['consts']['ss_cwnd_inc'], base=16), 0)
        self.assertNotEqual(int(state['consts']['ca_cwnd_inc'], base=16), 0)
        self.assertNotEqual(int(state['consts']['fr_cwnd_dec'], base=16), 0)

    def test_slow_start_increase(self):
        state = self.cong_state()
        init_cwnd = state['cwnd']
        self.assertEqual(state['in_flight_size'], 0)
        self.assertSlowStart(state)
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        N = 42
        self._send_msg_and_recv_ack(N)
        state = self.cong_state()
        self.assertEqual(state['cwnd'], init_cwnd + 1337)
        self.assertNotInFastRetransmit(state)

    def test_congestion_avoidance_increase(self):
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_mss = state['mss']
        self.set_ssthresh(init_cwnd - init_mss)
        state = self.cong_state()
        self.assertCongestionAvoidance(state)
        # pylint: disable=invalid-name
        # name chosen to be in line with RFC
        N = 42
        self._send_msg_and_recv_ack(N)
        state = self.cong_state()
        self.assertEqual(state['cwnd'], init_cwnd + 42)
        self.assertNotInFastRetransmit(state)

    def test_fast_retransmit_decrease(self):
        state = self.cong_state()
        init_cwnd = state['cwnd']
        msgs = [{'size': 24, 'send_time': 1000, 'resends': 0}]
        flight_size = state['in_flight_size']
        for msg in msgs:
            res = self.cong_report_msg_sent(msg_size=msg['size'])
            self.assertIn('success', res)
            flight_size += msg['size']
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], flight_size)
        res = self.cong_report_msgs_lost(msgs)
        self.assertIn('success', res)
        state = self.cong_state()
        self.assertEqual(state['cwnd'], init_cwnd // 8)


if __name__ == '__main__':
    unittest.main()

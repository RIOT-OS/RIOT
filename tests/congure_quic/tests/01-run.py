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
    DEBUG = False

    # pylint: disable=too-many-public-methods
    # it's just one more ...
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
        https://tools.ietf.org/html/rfc9002#section-7.3.1

        > A NewReno sender is in slow start any time the congestion window is
        > below the slow start threshold. A sender begins in slow start because
        > the slow start threshold is initialized to an infinite value.
        """
        self.assertLess(state['cwnd'], state['ssthresh'])

    def assertCongestionAvoidance(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """
        https://tools.ietf.org/html/rfc9002#section-7.3.3

        > A NewReno sender is in congestion avoidance any time the congestion
        > window is at or above the slow start threshold and not in a recovery
        > period.
        """
        self.assertGreaterEqual(state['cwnd'], state['ssthresh'])

    def assertInRecovery(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """recovery_start is set to current system time when entering recovery
        period"""
        self.assertGreater(state['recovery_start'], 0)

    def assertNotInRecovery(self, state):
        # pylint: disable=invalid-name
        # trying to be in line with `unittest`
        """recovery_start is reset to 0 when leaving recovery period"""
        self.assertEqual(state['recovery_start'], 0)

    def get_event_cb(self):
        res = self.exec_cmd('get_event_cb')
        return res['event_cb']

    def set_same_wnd_adv(self, value):
        self.exec_cmd('set_same_wnd_adv {value:d}'.format(value=value))

    def set_cwnd(self, cwnd):
        self.exec_cmd('set_cwnd {cwnd}'.format(cwnd=cwnd))

    def set_ssthresh(self, ssthresh):
        self.exec_cmd('set_ssthresh {ssthresh}'.format(ssthresh=ssthresh))

    def set_limited(self, limited):
        self.exec_cmd('set_limited {limited:d}'.format(limited=limited))

    def set_max_ack_delay(self, max_ack_delay):
        self.exec_cmd('set_max_ack_delay {max_ack_delay:d}'
                      .format(max_ack_delay=max_ack_delay))

    def set_recovery_start(self, recovery_start):
        self.exec_cmd('set_recovery_start {recovery_start}'
                      .format(recovery_start=recovery_start))

    def cong_state(self):
        return self.exec_cmd('state')

    def cong_init(self, ctx=0):
        res = self.shell.init(ctx)
        return self._parse(res)

    def cong_inter_msg_interval(self, msg_size):
        res = self.shell.inter_msg_interval(msg_size)
        return self._parse(res)['success']

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


class TestCongUREQUICWithoutSetup(TestCongUREBase):
    def test_no_setup(self):
        state = self.cong_state()
        self.assertEqual(state, {
            'ctx': '0x00000000',
            'cwnd': 0,
            'consts': None,
            'first_rtt_sample': 0,
            'in_flight_size': 0,
            'recovery_start': 0,
            'ssthresh': 0,
            'smoothed_rtt': 0,
            'rtt_var': 0,
            'min_rtt': 0,
            'limited': False,
            'max_ack_delay': 0,
        })


class TestCongUREQUICDefaultInitTests(TestCongUREBase):
    """
    The implementation is based on

    https://tools.ietf.org/html/rfc9002#appendix-B

    so we can check the viability based on that.
    """
    def setUp(self):
        super().setUp()
        res = self._parse(self.shell.setup(0))
        self.assertIn('success', res)

    def test_setup(self):
        state = self.cong_state()
        self.assertIsNotNone(state['consts'])
        self.assertEqual(int(state['consts']['cong_event_cb'], base=16), 0)
        self.assertEqual(state['consts']['init_wnd'], 12000)
        self.assertEqual(state['consts']['min_wnd'], 2400)
        self.assertEqual(state['consts']['init_rtt'], 333)
        self.assertEqual(state['consts']['max_msg_size'], 1200)
        self.assertEqual(state['consts']['pc_thresh'], 3000)
        self.assertEqual(state['consts']['granularity'], 1)
        self.assertEqual(state['consts']['loss_reduction_numerator'], 1)
        self.assertEqual(state['consts']['loss_reduction_denominator'], 2)
        self.assertEqual(state['consts']['inter_msg_interval_numerator'], 5)
        self.assertEqual(state['consts']['inter_msg_interval_denominator'], 4)

    def test_init(self):
        """
        > B.3.  Initialization
        >
        >    At the beginning of the connection, initialize the congestion
        >    control variables as follows:
        >
        >    congestion_window = kInitialWindow
        >    bytes_in_flight = 0
        >    congestion_recovery_start_time = 0
        >    ssthresh = infinite
        Packet number spaces are QUIC protocol-specific and thus not
        implemented

        Some recovery variables also apply for congestion, thus we also need to
        check if https://tools.ietf.org/html/rfc9002#appendix-A.4
        upholds:

        > A.4.  Initialization
        >
        >    At the beginning of the connection, initialize the loss detection
        >    variables as follows:
        >
        >    (loss_detection_timer.reset())
        >    (pto_count = 0)
        >    (latest_rtt = 0)
        >    smoothed_rtt = kInitialRtt
        >    rttvar = kInitialRtt / 2
        >    min_rtt = 0
        >    (first_rtt_sample = 0)
        Again, packet number spaces are QUIC protocol-specific and thus not
        implemented
        """
        res = self.cong_init()
        self.assertIn('success', res)
        state = self.cong_state()
        # congestion_window = kInitialWindow
        self.assertEqual(state['consts']['init_wnd'], state['cwnd'])
        # bytes_in_flight = 0
        self.assertEqual(state['in_flight_size'], 0)
        # recovery_start = 0
        self.assertNotInRecovery(state)
        # ssthresh = infinite
        self.assertEqual(state['ssthresh'], 0xffff)
        # smoothed_rtt = kInitialRtt
        self.assertEqual(state['smoothed_rtt'], state['consts']['init_rtt'])
        # rttvar = kInitialRtt / 2
        self.assertEqual(state['rtt_var'], state['consts']['init_rtt'] // 2)
        # min_rtt = 0
        self.assertEqual(state['min_rtt'], 0)


class TestCongUREQUICDefault(TestCongUREBase):
    """
    The implementation is based on

    https://tools.ietf.org/html/rfc9002#appendix-B

    so we can check the viability based on that.
    """
    def setUp(self):
        super().setUp()
        res = self._parse(self.shell.setup(0))
        self.assertIn('success', res)
        res = self.cong_init()
        self.assertIn('success', res)
        self.sent_bytes = 42
        self.ack_delay = 10

    def test_on_packet_sent(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.4
        """
        state = self.cong_state()
        init_in_flight_size = state['in_flight_size']
        self.cong_report_msg_sent(self.sent_bytes)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'],
                         init_in_flight_size + self.sent_bytes)

    def _send_msg_recv_ack(self, send_time=1000, recv_time=1120):
        # put ACK'd packet in flight
        self.test_on_packet_sent()
        msg = {'send_time': send_time, 'size': self.sent_bytes, 'resends': 0}
        ack = {'recv_time': recv_time, 'id': 1337, 'size': 0, 'clean': True,
               'wnd': 1234, 'delay': self.ack_delay}
        self.cong_report_msg_acked(msg=msg, ack=ack)
        # this method is reused a lot, so reset internal message buffer of
        # `congure_test`
        res = self._parse(self.shell.msgs_reset())
        self.assertIn('success', res)
        return msg, ack

    def test_on_packet_ack_no_rtt_sample(self, state=None):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-A.7
        """
        if state is None:
            state = self.cong_state()
        # From A.7, UpdateRtt
        # if (first_rtt_sample == 0):
        self.assertEqual(state['first_rtt_sample'], 0)
        msg, ack = self._send_msg_recv_ack()
        state = self.cong_state()
        # min_rtt = latest_rtt
        self.assertEqual(state['min_rtt'], ack['recv_time'] - msg['send_time'])
        # smoothed_rtt = latest_rtt
        self.assertEqual(state['smoothed_rtt'],
                         ack['recv_time'] - msg['send_time'])
        # rttvar = latest_rtt / 2
        self.assertEqual(state['rtt_var'],
                         (ack['recv_time'] - msg['send_time']) // 2)
        # first_rtt_sample = now()
        self.assertGreater(state['first_rtt_sample'], 0)
        return msg, ack

    def assertRTTSampleCorrect(self, state, init_min_rtt, init_smoothed_rtt,
                               init_rtt_var, adjusted_rtt):
        # rttvar = 3/4 * rttvar + 1/4 * abs(smoothed_rtt - adjusted_rtt)
        self.assertEqual(
            state['rtt_var'],
            ((3 * init_rtt_var) // 4) +
            abs(init_smoothed_rtt - adjusted_rtt) // 4
        )
        # smoothed_rtt = 7/8 * smoothed_rtt + 1/8 * adjusted_rtt
        self.assertEqual(
            state['smoothed_rtt'],
            ((7 * init_smoothed_rtt) // 8) + (adjusted_rtt // 8)
        )
        # first_rtt_sample = now()
        self.assertGreater(state['first_rtt_sample'], 0)

    def test_on_packet_ack_rtt_sample_greater_rtt(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-A.7
        """
        # set first RTT sample
        self.test_on_packet_ack_no_rtt_sample()
        state = self.cong_state()
        init_min_rtt = state['min_rtt']
        init_smoothed_rtt = state['smoothed_rtt']
        init_rtt_var = state['rtt_var']
        # From A.7, UpdateRtt
        # if (first_rtt_sample == 0):
        self.assertNotEqual(state['first_rtt_sample'], 0)
        # another message ACK pair, but with larger RTT
        msg, ack = self._send_msg_recv_ack(send_time=1140, recv_time=1300)
        state = self.cong_state()
        # min_rtt = min(min_rtt, latest_rtt)
        self.assertGreater(ack['recv_time'] - msg['send_time'], init_min_rtt)
        # latest_rtt is greater, so it should stay the same
        self.assertEqual(state['min_rtt'], init_min_rtt)
        # as min_rtt == 120, ack_delay == 10, latest_rtt = 160
        # => adjusted_rtt = latest_rtt - ack_delay
        adjusted_rtt = ack['recv_time'] - msg['send_time'] - ack['delay']
        self.assertRTTSampleCorrect(state, init_min_rtt, init_smoothed_rtt,
                                    init_rtt_var, adjusted_rtt)

    def test_on_packet_ack_rtt_sample_less_rtt(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-A.7
        """
        # set first RTT sample
        self.test_on_packet_ack_no_rtt_sample()
        state = self.cong_state()
        init_min_rtt = state['min_rtt']
        init_smoothed_rtt = state['smoothed_rtt']
        init_rtt_var = state['rtt_var']
        # From A.7, UpdateRtt
        # if (first_rtt_sample == 0):
        self.assertNotEqual(state['first_rtt_sample'], 0)
        # another message ACK pair, but with larger RTT
        msg, ack = self._send_msg_recv_ack(send_time=1140, recv_time=1250)
        state = self.cong_state()
        # min_rtt = min(min_rtt, latest_rtt)
        self.assertLess(ack['recv_time'] - msg['send_time'], init_min_rtt)
        # latest_rtt is less, so it should become latest_rtt
        self.assertEqual(state['min_rtt'], ack['recv_time'] - msg['send_time'])
        # as min_rtt == 110, ack_delay == 10, latest_rtt = 110
        # => adjusted_rtt = latest_rtt  (so unadjusted)
        adjusted_rtt = ack['recv_time'] - msg['send_time']
        self.assertRTTSampleCorrect(state, init_min_rtt, init_smoothed_rtt,
                                    init_rtt_var, adjusted_rtt)

    def test_on_packet_ack_not_limited_slow_start(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.5
        """
        state = self.cong_state()
        self.assertSlowStart(state)
        self.assertNotInRecovery(state)
        self.assertFalse(state['limited'])
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly
        msg, ack = self.test_on_packet_ack_no_rtt_sample(state)
        state = self.cong_state()
        # bytes_in_flight -= acked_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        # // Slow start.
        # congestion_window += acked_packet.sent_bytes
        self.assertEqual(state['cwnd'], init_cwnd + msg['size'])

    def test_on_packet_ack_not_limited_congestion_avoidance(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.5
        """
        state = self.cong_state()
        # enforce congestion avoidance
        self.set_ssthresh(state['cwnd'] - 100)
        state = self.cong_state()
        self.assertCongestionAvoidance(state)
        self.assertNotInRecovery(state)
        self.assertFalse(state['limited'])
        init_cwnd = state['cwnd']
        init_in_flight_size = state['in_flight_size']
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly
        msg, ack = self.test_on_packet_ack_no_rtt_sample(state)
        state = self.cong_state()
        # bytes_in_flight -= acked_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        # // Congestion avoidance.
        # congestion_window +=
        #   max_datagram_size * acked_packet.sent_bytes
        #   / congestion_window
        self.assertEqual(
            state['cwnd'],
            init_cwnd +
            ((state['consts']['max_msg_size'] * msg['size']) // init_cwnd)
        )

    def test_on_packet_ack_not_limited_recovery(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.5
        """
        # enforce recovery mode for given send time 1000 and ack recv time 1120
        self.set_recovery_start(1130)
        state = self.cong_state()
        self.assertInRecovery(state)
        self.assertFalse(state['limited'])
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly
        msg, ack = self.test_on_packet_ack_no_rtt_sample(state)
        state = self.cong_state()
        # bytes_in_flight -= acked_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        # // Do not increase congestion window in recovery period.
        # if (InCongestionRecovery(acked_packet.time_sent)):
        #   return
        self.assertEqual(state['cwnd'], init_cwnd)

    def test_on_packet_ack_limited(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.5
        """
        # set to limited by application or flow control
        self.set_limited(True)
        state = self.cong_state()
        self.assertSlowStart(state)
        self.assertNotInRecovery(state)
        self.assertTrue(state['limited'])
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly
        msg, ack = self.test_on_packet_ack_no_rtt_sample(state)
        state = self.cong_state()
        # bytes_in_flight -= acked_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        # // Do not increase congestion_window if application
        # // limited or flow control limited.
        # if (IsAppOrFlowControlLimited())
        #   return
        self.assertEqual(state['cwnd'], init_cwnd)

    def assertMaybeSendOnePacket(self, state):
        """
        Maybe send one packet is modeled as cong_event_cb in congure_quic.

        It is not assigned in this test case so it should not have been called.
        """
        event_cb = self.get_event_cb()
        self.assertEqual(event_cb['calls'], 0)

    def assertOnNewCongestionEvent(self, state, init_cwnd, init_ssthresh,
                                   in_recovery=False,
                                   persistent_congestion=False):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.6
        """
        if persistent_congestion:
            # persistent congestion triggers return to slow start; see
            # https://tools.ietf.org/html/rfc9002#section-7.3
            self.assertNotInRecovery(state)
            # See https://tools.ietf.org/html/rfc9002#appendix-B.8
            # if (InPersistentCongestion(pc_lost)):
            #   congestion_window = kMinimumWindow
            #   congestion_recovery_start_time = 0
            self.assertEqual(state['cwnd'], state['consts']['min_wnd'])
            # persistent congestion triggers return to slow start; see
            # https://tools.ietf.org/html/rfc9002#section-7.3
            # or at least congestion avoidance with ssthresh being at least
            # kMinimumWindow
            # see https://github.com/quicwg/base-drafts/issues/4826#issuecomment-776316765
            self.assertLessEqual(state['cwnd'], state['ssthresh'])
        elif in_recovery:
            self.assertInRecovery(state)
            # // No reaction if already in a recovery period.
            # if (InCongestionRecovery(sent_time)):
            #   return
            # => ssthresh remains unchanged
            self.assertEqual(state['ssthresh'], init_ssthresh)
            self.assertEqual(state['cwnd'], init_cwnd)
        else:
            # persistent congestion triggers return to slow start; see
            # https://tools.ietf.org/html/rfc9002#section-7.3
            # Enter recovery period.
            # congestion_recovery_start_time = now()
            self.assertInRecovery(state)
            # ssthresh = congestion_window * kLossReductionFactor
            exp_ssthresh = (
                init_cwnd * state['consts']['loss_reduction_numerator']
            ) // state['consts']['loss_reduction_denominator']
            self.assertEqual(state['ssthresh'], exp_ssthresh)
            # congestion_window = max(ssthresh, kMinimumWindow)
            if exp_ssthresh < state['consts']['min_wnd']:
                self.assertEqual(state['cwnd'], state['consts']['min_wnd'])
            else:
                self.assertEqual(state['cwnd'], exp_ssthresh)
            self.assertMaybeSendOnePacket(state)

    def test_on_ecn_information_not_recovery(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.7
        """
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.cong_report_ecn_ce(1000)
        state = self.cong_state()
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh)
        # just making sure we took the right path in that method
        self.assertNotEqual(state['cwnd'], state['consts']['min_wnd'])

    def test_on_ecn_information_not_recovery_low_ssthresh(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.7
        """
        state = self.cong_state()
        # enforce new cwnd to be smaller than `min_wnd`
        init_cwnd = state['consts']['min_wnd']
        self.set_cwnd(init_cwnd)
        init_ssthresh = state['ssthresh']
        self.cong_report_ecn_ce(1000)
        state = self.cong_state()
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh)
        # just making sure we took the right path in that method
        self.assertEqual(state['cwnd'], state['consts']['min_wnd'])

    def test_on_ecn_information_recovery(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.7
        """
        # enforce recovery mode for given send time = 1000
        self.set_recovery_start(2000)
        state = self.cong_state()
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.cong_report_ecn_ce(1000)
        state = self.cong_state()
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh,
                                        in_recovery=True)
        # just making sure we took the right path in that method
        self.assertNotEqual(state['cwnd'], state['consts']['min_wnd'])

    def _send_msgs(self, msgs):
        for msg in msgs:
            self.cong_report_msg_sent(msg['size'])

    def _send_msgs_and_report_lost(self, msgs, init_in_flight_size):
        self._send_msgs(msgs)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'],
                         init_in_flight_size + sum(m['size'] for m in msgs))
        self.cong_report_msgs_lost(msgs)

    def test_on_packets_lost_not_recovery_no_pc(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        msgs = [{'send_time': 1000, 'size': 124, 'resends': 1},
                {'send_time': 1050, 'size': 643, 'resends': 0},
                {'send_time': 1100, 'size': 134, 'resends': 0}]
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        state = self.cong_state()
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertNotInRecovery(state)
        self._send_msgs_and_report_lost(msgs, init_in_flight_size)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh)

    def test_on_packets_lost_not_recovery_no_pc_low_cwnd(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        msgs = [{'send_time': 1000, 'size': 124, 'resends': 1},
                {'send_time': 1050, 'size': 643, 'resends': 0},
                {'send_time': 1100, 'size': 134, 'resends': 0}]
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        # enforce new cwnd to be smaller than `min_wnd`
        state = self.cong_state()
        init_cwnd = state['consts']['min_wnd']
        self.set_cwnd(init_cwnd)
        state = self.cong_state()
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertNotInRecovery(state)
        self._send_msgs_and_report_lost(msgs, init_in_flight_size)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh)

    def test_on_packets_lost_recovery_no_pc(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        msgs = [{'send_time': 1000, 'size': 124, 'resends': 1},
                {'send_time': 1050, 'size': 643, 'resends': 0},
                {'send_time': 1100, 'size': 134, 'resends': 0}]
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        # enforce recovery mode for given send time = 1100
        self.set_recovery_start(2000)
        state = self.cong_state()
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertInRecovery(state)
        self._send_msgs_and_report_lost(msgs, init_in_flight_size)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh,
                                        in_recovery=True)

    def test_on_packets_lost_not_recovery_pc(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        max_ack_delay = 120
        self.set_max_ack_delay(max_ack_delay)
        state = self.cong_state()
        # only messages sent after the first RTT sample are considered for
        # persistent congestion, so add that time to the message's send_time
        offset = state['first_rtt_sample']
        # See https://tools.ietf.org/html/rfc9002#section-7.6.1
        #    (smoothed_rtt + max(4*rttvar, kGranularity) + max_ack_delay) *
        #           kPersistentCongestionThreshold
        pc_duration = (
            state['smoothed_rtt'] +
            max(4 * state['rtt_var'], state['consts']['granularity']) +
            max_ack_delay
        ) * state['consts']['pc_thresh']
        # for lost in lost_packets:
        #   if lost.time_sent > first_rtt_sample:
        #     pc_lost.insert(lost)
        msgs = [{'send_time': 1000 + offset, 'size': 124, 'resends': 1},
                {'send_time': 1050 + offset, 'size': 643, 'resends': 0},
                {'send_time': 1100 + offset + pc_duration,
                 'size': 134, 'resends': 0}]
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertNotInRecovery(state)
        self._send_msgs_and_report_lost(msgs, init_in_flight_size)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh,
                                        persistent_congestion=True)

    def test_on_packets_lost_not_recovery_pc_low_cwnd(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        # enforce new cwnd to be smaller than `min_wnd`
        state = self.cong_state()
        init_cwnd = state['consts']['min_wnd']
        self.set_cwnd(init_cwnd)
        state = self.cong_state()
        max_ack_delay = 120
        self.set_max_ack_delay(max_ack_delay)
        state = self.cong_state()
        # only messages sent after the first RTT sample are considered for
        # persistent congestion, so add that time to the message's send_time
        offset = state['first_rtt_sample']
        # See https://tools.ietf.org/html/rfc9002#section-7.6.1
        #    (smoothed_rtt + max(4*rttvar, kGranularity) + max_ack_delay) *
        #           kPersistentCongestionThreshold
        pc_duration = (
            state['smoothed_rtt'] +
            max(4 * state['rtt_var'], state['consts']['granularity']) +
            max_ack_delay
        ) * state['consts']['pc_thresh']
        # for lost in lost_packets:
        #   if lost.time_sent > first_rtt_sample:
        #     pc_lost.insert(lost)
        msgs = [{'send_time': 1000 + offset, 'size': 124, 'resends': 1},
                {'send_time': 1050 + offset, 'size': 643, 'resends': 0},
                {'send_time': 1100 + offset + pc_duration,
                 'size': 134, 'resends': 0}]
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertNotInRecovery(state)
        self._send_msgs(msgs)
        self.cong_report_msgs_lost(msgs)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh,
                                        persistent_congestion=True)

    def test_on_packets_lost_recovery_pc(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.8
        """
        # trigger message + ACK and check if UpdateRTT (see A.7) was called
        # correctly to have an RTT sample
        msg, ack = self.test_on_packet_ack_no_rtt_sample(self.cong_state())
        # enforce recovery mode for given send time = 1100
        self.set_recovery_start(2000)
        state = self.cong_state()
        max_ack_delay = 120
        self.set_max_ack_delay(max_ack_delay)
        state = self.cong_state()
        # only messages sent after the first RTT sample are considered for
        # persistent congestion, so add that time to the message's send_time
        offset = state['first_rtt_sample']
        # See https://tools.ietf.org/html/rfc9002#section-7.6.1
        #    (smoothed_rtt + max(4*rttvar, kGranularity) + max_ack_delay) *
        #           kPersistentCongestionThreshold
        pc_duration = (
            state['smoothed_rtt'] +
            max(4 * state['rtt_var'], state['consts']['granularity']) +
            max_ack_delay
        ) * state['consts']['pc_thresh']
        # for lost in lost_packets:
        #   if lost.time_sent > first_rtt_sample:
        #     pc_lost.insert(lost)
        msgs = [{'send_time': 1000 + offset, 'size': 124, 'resends': 1},
                {'send_time': 1050 + offset, 'size': 643, 'resends': 0},
                {'send_time': 1100 + offset + pc_duration,
                 'size': 134, 'resends': 0}]
        init_in_flight_size = state['in_flight_size']
        init_cwnd = state['cwnd']
        init_ssthresh = state['ssthresh']
        self.assertInRecovery(state)
        self._send_msgs(msgs)
        self.cong_report_msgs_lost(msgs)
        state = self.cong_state()
        # for lost_packet in lost_packets:
        #   if lost_packet.in_flight:
        #     bytes_in_flight -= lost_packet.sent_bytes
        self.assertEqual(state['in_flight_size'], init_in_flight_size)
        self.assertOnNewCongestionEvent(state, init_cwnd, init_ssthresh,
                                        in_recovery=True,
                                        persistent_congestion=True)
        # persistent congestion triggers return to slow start; see
        # https://tools.ietf.org/html/rfc9002#section-7.3
        self.assertSlowStart(state)
        self.assertNotInRecovery(state)

    def test_removing_discarded_packets_from_bytes_in_flight(self):
        """
        See https://tools.ietf.org/html/rfc9002#appendix-B.9
        """
        state = self.cong_state()
        init_in_flight_size = state['in_flight_size']
        self.cong_report_msg_sent(1337)
        self.cong_report_msg_discarded(1337)
        state = self.cong_state()
        self.assertEqual(state['in_flight_size'], init_in_flight_size)

    def test_pacing(self):
        """
        See https://tools.ietf.org/html/rfc9002#section-7.7
        """
        state = self.cong_state()
        self.assertGreater(state["smoothed_rtt"], 0)
        self.assertGreater(state["cwnd"], 0)
        # pylint: disable=invalid-name
        # name chosen to be in line with draft
        N = state["consts"]["inter_msg_interval_numerator"] / \
            state["consts"]["inter_msg_interval_denominator"]
        # Using a value for "N" that is small, but at least 1 (for example,
        # 1.25) ensures that variations in round-trip time do not result in
        # under-utilization of the congestion window.
        self.assertGreaterEqual(N, 1)
        msg_size = 760
        # interval = ( smoothed_rtt * packet_size / congestion_window ) / N
        # packet_size is expected to be multiplied in by the user.
        self.assertAlmostEqual(
            # smoothed_rtt is in milliseconds, expected return value of
            # inter_msg_interval is microseconds.
            int((state["smoothed_rtt"] * msg_size * 1000) / state["cwnd"] / N),
            self.cong_inter_msg_interval(msg_size)
        )


class TestCongUREQUICEventCb(TestCongUREQUICDefault):
    def setUp(self):
        super().setUp()
        res = self._parse(self.shell.setup(1))
        self.assertIn('success', res)
        res = self.cong_init(0xdead)
        self.assertIn('success', res)

    def test_setup_and_init(self):
        state = self.cong_state()
        self.assertNotEqual(int(state['consts']['cong_event_cb'], base=16), 0)
        self.assertEqual(int(state['ctx'], base=16), 0xdead)

    def assertMaybeSendOnePacket(self, state):
        """
        Maybe send one packet is modeled as cong_event_cb in congure_quic.

        It is assigned in this test case so it should have been called.
        """
        event_cb = self.get_event_cb()
        self.assertEqual(event_cb['calls'], 1)
        self.assertEqual(int(event_cb['last_args']['ctx'], base=16),
                         int(state['ctx'], base=16))


if __name__ == '__main__':
    unittest.main()

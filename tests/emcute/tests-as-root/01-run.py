#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import pprint
import random
import re
import socket
import sys
import subprocess
import threading
import time

from scapy.all import Automaton, ATMT, log_runtime, MTU, raw, SimpleSocket
from scapy.contrib import mqttsn
from testrunner import run, utils

TEST_INTERACTIVE_RETRIES = int(os.environ.get('TEST_INTERACTIVE_RETRIES') or 5)
TEST_INTERACTIVE_DELAY = int(os.environ.get('TEST_INTERACTIVE_DELAY') or 1)

SERVER_PORT = 1883
MODES = set(["pub", "sub", "sub_w_reg"])
INTER_PACKET_GAP = 0.07
TIMEOUT = 1


class MQTTSNServer(Automaton):
    class MQTTSNServerSocket(SimpleSocket):
        def __init__(self, server, *args, **kwargs):
            super(MQTTSNServer.MQTTSNServerSocket, self)\
                .__init__(*args, **kwargs)
            self.server = server
            self.send_lock = threading.Lock()

        def recv(self, x=MTU):
            pkt, sa = self.ins.recvfrom(x)
            self.server.last_remote = sa
            return mqttsn.MQTTSN(pkt)

        def send(self, x):
            assert self.server.last_remote is not None
            try:
                sx = raw(x)
                x.sent_time = time.time()
                # wait if last sendto was less than INTER_PACKET_GAP seconds
                # ago
                self.send_lock.acquire()
                self.outs.sendto(sx, self.server.last_remote)
                # add small delay between each send to not overwhelm ethos
                threading.Timer(INTER_PACKET_GAP,
                                self.send_lock.release).start()
            except socket.error as msg:
                log_runtime.error(msg)

    def __init__(self, *args, **kwargs):
        kwargs["ll"] = MQTTSNServer.MQTTSNServerSocket
        kwargs["recvsock"] = MQTTSNServer.MQTTSNServerSocket
        self.last_remote = None
        super(MQTTSNServer, self).__init__(*args, **kwargs)

    def parse_args(self, spawn, bind_addr, topic_name, mode, pub_interval,
                   qos_level=0,
                   data_len_start=1, data_len_end=1000, data_len_step=1,
                   bind_port=SERVER_PORT, family=socket.AF_INET,
                   type=socket.SOCK_DGRAM, proto=0, *args, **kwargs):
        assert mode in MODES
        super(MQTTSNServer, self).parse_args(*args, **kwargs)
        self.spawn = spawn
        self.topic_name = topic_name
        self.mode = mode
        self.pub_interval = pub_interval
        self.qos_level = qos_level
        self.data_len = data_len_start
        self.data_len_end = data_len_end
        self.data_len_step = data_len_step
        self.last_mid = random.randint(0, 0xffff)
        self.topics = []
        self.registered_topics = []
        self.subscriptions = []
        self.res = ""

        sock = socket.socket(family, type, proto)
        res = socket.getaddrinfo(bind_addr, bind_port)
        sockaddr = res[0][4]
        sock.bind(sockaddr)
        self.gw_addr = "[{}]:{}".format(sockaddr[0], sockaddr[1])
        self.socket_kargs = {"sock": sock, "server": self}

    # >>> private properties and methods <<< #
    @property
    def _qos_flags(self):
        qos = min(self.qos_level, 2)
        if qos < 0:
            qos = mqttsn.QOS_NEG1
        return qos

    def _check_pkt_qos(self, pkt):
        qos_types = [mqttsn.PUBLISH, mqttsn.SUBSCRIBE]
        # see MQTT-SN 1.2 spec 5.4.12
        if (pkt.type == mqttsn.PUBLISH) and \
           (pkt.qos not in [mqttsn.QOS_1, mqttsn.QOS_2]) and \
           (pkt.mid != 0):
            return False
        return (pkt.type not in qos_types) or (pkt.qos == self._qos_flags)

    def _get_tid(self, topic_name):
        if topic_name not in self.topics:
            self.topics.append(topic_name)
        return self.topics.index(topic_name) + 1

    def _get_topic_name(self, tid):
        return self.topics[tid - 1]

    # >>> automaton states <<< #
    @ATMT.state(initial=1)
    def BEGIN(self):
        utils.test_utils_interactive_sync_shell(self.spawn,
                                                TEST_INTERACTIVE_RETRIES,
                                                TEST_INTERACTIVE_DELAY)
        raise self.CONNECT_FROM_NODE()

    @ATMT.state()
    def CONNECT_FROM_NODE(self):
        self.spawn.sendline("con {}".format(self.gw_addr))
        raise self.WAITING(mqttsn.CONNECT)

    @ATMT.state()
    def REGISTER_FROM_NODE(self):
        self.spawn.sendline("reg {}".format(self.topic_name))
        raise self.WAITING(mqttsn.REGISTER)

    @ATMT.state()
    def PUBLISH_FROM_NODE(self, topic_name):
        if self.data_len < self.data_len_end:
            self.spawn.sendline("pub {} {:d} {:d}" .format(topic_name,
                                                           self.data_len,
                                                           self.qos_level))
            raise self.WAITING(mqttsn.PUBLISH)
        else:
            raise self.END()

    @ATMT.state()
    def SUBSCRIBE_FROM_NODE(self):
        self.spawn.sendline("sub {} {}".format(self.topic_name,
                                               self.qos_level))
        raise self.WAITING(mqttsn.SUBSCRIBE)

    @ATMT.state()
    def PUBLISH_TO_NODE(self, subscription):
        tid = subscription["tid"]
        self.last_mid += 1
        mid = self.last_mid
        if self.data_len == 0:
            # send deliberately broken length packets
            # (to small payload, len field < 256)
            self.last_packet = mqttsn.MQTTSN(len=128) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="128"
            )
            self.send(self.last_packet)
            # send deliberately broken length packets
            # (to small payload, len field >= 256)
            self.last_packet = mqttsn.MQTTSN(len=400) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="400"
            )
            self.send(self.last_packet)
            # send deliberately broken length packets (too large payload)
            self.last_packet = mqttsn.MQTTSN(len=10) / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="X" * 20
            )
            self.send(self.last_packet)
            return subscription, mid
        if self.data_len < self.data_len_end:
            self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNPublish(
                qos=self._qos_flags, tid=tid, mid=mid, data="X" * self.data_len
            )
            self.send(self.last_packet)
            return subscription, mid
        else:
            raise self.END()

    @ATMT.state()
    def WAITING(self, exp_type, tid=None, mid=None):
        return exp_type, mid, tid

    @ATMT.state(final=1)
    def END(self):
        self.spawn.sendline("info")
        self.spawn.expect_exact("Broker: '{}'".format(self.gw_addr))
        self.spawn.expect_exact("- Topics")
        for tid, topic_name in enumerate(self.registered_topics, 1):
            self.spawn.expect_exact("  {:2d}: {}".format(tid, topic_name))
        self.spawn.expect_exact("- Subscriptions")
        for sub in self.subscriptions:
            self.spawn.expect_exact("  {:2d}: {}".format(
                sub["tid"], sub["topic_name"].decode())
            )
        self.spawn.sendline("reboot")
        return self.res

    @ATMT.state(error=1)
    def UNEXPECTED_MESSAGE_TYPE(self, type, qos=None):
        self.res += "\nUnexpected message type {} [{}]".format(
            mqttsn.PACKET_TYPE[type],
            mqttsn.QOS_LEVELS[qos] if qos is not None else "-",
        )
        return self.res

    @ATMT.state(error=1)
    def UNEXPECTED_PARAMETERS(self, pkt):
        self.res += "\nUnexpected parameters \n" \
            " {}".format(repr(pkt))
        return self.res

    @ATMT.state(error=1)
    def MESSAGE_TIMEOUT(self, exp_type):
        self.res += "\n{} timed out".format(mqttsn.PACKET_TYPE[exp_type])
        return self.res

    # >>> automaton timeouts, conditions and actions <<< #
    @ATMT.timeout(WAITING, TIMEOUT)
    def timeout_message(self, args):
        raise self.MESSAGE_TIMEOUT(args[0])

    @ATMT.condition(PUBLISH_TO_NODE, prio=1)
    def PUBLISH_asks_for_PUBACK(self, args):
        subscription = args[0]
        tid = subscription["tid"]
        mid = args[1]
        if self.last_packet.qos in [mqttsn.QOS_1, mqttsn.QOS_2]:
            raise self.WAITING(mqttsn.PUBACK, tid, mid)

    @ATMT.condition(PUBLISH_TO_NODE, prio=2)
    def wait_for_PUBLISH_on_node(self, args):
        subscription = args[0]
        if self.data_len > 0:
            self.spawn.expect_exact(
                    "### got publication of {:d} bytes for topic "
                    "'{}' [{:d}] ###"
                    .format(self.data_len, subscription["topic_name"].decode(),
                            subscription["tid"]))
        self.data_len += self.data_len_step
        time.sleep(self.pub_interval)
        raise self.PUBLISH_TO_NODE(subscription)

    @ATMT.receive_condition(WAITING, prio=1)
    def receive_wrong_message(self, pkt, args):
        exp_type = args[0]
        if pkt.type != exp_type or not self._check_pkt_qos(pkt):
            raise self.UNEXPECTED_MESSAGE_TYPE(pkt.type, pkt.qos)

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_unexpected_parameters(self, pkt, args):
        if pkt.type == mqttsn.PUBLISH:
            if self.data_len != len(pkt.data):
                raise self.UNEXPECTED_PARAMETERS(pkt)
        elif pkt.type == mqttsn.PUBACK:
            exp_mid = args[1]
            exp_tid = args[2]
            if (exp_tid != pkt.tid) or (exp_mid != pkt.mid) or \
               (mqttsn.ACCEPTED != pkt.return_code):
                raise self.UNEXPECTED_PARAMETERS(pkt)

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_CONNECT_mode_sub(self, pkt, args):
        if pkt.type == mqttsn.CONNECT and self.mode == "sub":
            raise self.SUBSCRIBE_FROM_NODE()

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_CONNECT_mode_pub_or_sub_w_reg(self, pkt, args):
        if pkt.type == mqttsn.CONNECT and self.mode in ["pub", "sub_w_reg"]:
            raise self.REGISTER_FROM_NODE()

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_REGISTER_mode_pub(self, pkt, args):
        if pkt.type == mqttsn.REGISTER:
            topic_name = pkt.topic_name.decode()
            if self.mode in ["pub"]:
                raise self.PUBLISH_FROM_NODE(topic_name) \
                    .action_parameters(topic_name=topic_name,
                                       mid=pkt.mid)

    @ATMT.receive_condition(WAITING, prio=3)
    def receive_REGISTER_mode_sub_w_reg(self, pkt, args):
        if pkt.type == mqttsn.REGISTER:
            topic_name = pkt.topic_name.decode()
            if self.mode in ["sub_w_reg"]:
                raise self.SUBSCRIBE_FROM_NODE() \
                    .action_parameters(topic_name=topic_name,
                                       mid=pkt.mid)

    @ATMT.receive_condition(WAITING, prio=3)
    def receive_PUBLISH(self, pkt, args):
        if pkt.type == mqttsn.PUBLISH:
            topic_name = self._get_topic_name(pkt.tid)
            self.res += ":".join("{:02x}".format(c) for c in pkt.data)
            self.data_len += self.data_len_step
            raise self.PUBLISH_FROM_NODE(topic_name) \
                .action_parameters(topic_name=topic_name,
                                   qos=pkt.qos, mid=pkt.mid, tid=pkt.tid)

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_SUBSCRIBE(self, pkt, args):
        if pkt.type == mqttsn.SUBSCRIBE:
            if pkt.tid_type in [mqttsn.TID_NORMAL, mqttsn.TID_SHORT]:
                topic_name = pkt.topic_name
                tid = self._get_tid(pkt.topic_name)
            elif pkt.tid_type == mqttsn.TID_PREDEF:
                tid = pkt.tid
                topic_name = self._get_topic_name(tid)
            else:
                assert(False)
            subscription = {"tid": tid, "topic_name": topic_name}
            if subscription not in self.subscriptions:
                self.subscriptions.append(subscription)
            raise self.PUBLISH_TO_NODE(subscription) \
                .action_parameters(mid=pkt.mid, tid=tid)

    @ATMT.receive_condition(WAITING, prio=2)
    def receive_PUBACK(self, pkt, args):
        if pkt.type == mqttsn.PUBACK:
            self.data_len += self.data_len_step
            time.sleep(self.pub_interval)
            raise self.PUBLISH_TO_NODE({
                "tid": pkt.tid,
                "topic_name": self._get_topic_name(pkt.tid)
            })

    @ATMT.action(receive_CONNECT_mode_sub)
    @ATMT.action(receive_CONNECT_mode_pub_or_sub_w_reg)
    def send_CONNACK(self):
        # send too large packet for reception buffer
        # see https://github.com/RIOT-OS/RIOT/pull/12382
        self.last_packet = mqttsn.MQTTSN() / \
            mqttsn.MQTTSNConnack() / ("X" * 525)
        self.send(self.last_packet)
        # send deliberately broken length packets (too small len)
        self.last_packet = mqttsn.MQTTSN(len=2) / mqttsn.MQTTSNConnack()
        self.send(self.last_packet)
        # send deliberately broken length packets (too large len)
        self.last_packet = mqttsn.MQTTSN(len=3, type=mqttsn.CONNACK)
        self.send(self.last_packet)
        # send deliberately broken length packets (garbage payload)
        self.last_packet = mqttsn.MQTTSN(len=128) / \
            mqttsn.MQTTSNConnack() / b"this is garbage"
        self.send(self.last_packet)
        self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNConnack()
        self.send(self.last_packet)
        self.spawn.expect_exact("success: connected to gateway at {}"
                                .format(self.gw_addr))

    @ATMT.action(receive_REGISTER_mode_pub)
    @ATMT.action(receive_REGISTER_mode_sub_w_reg)
    def send_REGACK(self, topic_name, mid):
        tid = self._get_tid(topic_name)
        if topic_name not in self.registered_topics:
            self.registered_topics.append(topic_name)
        # send deliberately broken length packets (too small len)
        self.last_packet = mqttsn.MQTTSN(len=4) / \
            mqttsn.MQTTSNRegack(mid=mid, tid=tid)
        self.send(self.last_packet)
        # send deliberately broken length packets (too large len)
        # include valid MID for extra confusion
        self.last_packet = mqttsn.MQTTSN(len=7, type=mqttsn.REGACK) / \
            bytes([tid >> 8, tid & 0xff, mid >> 8, mid & 0xff])
        self.send(self.last_packet)
        # send deliberately broken length packets (garbage payload)
        self.last_packet = mqttsn.MQTTSN(len=128) / \
            mqttsn.MQTTSNRegack(mid=mid, tid=tid) / b"this is garbage"
        self.send(self.last_packet)
        self.last_packet = mqttsn.MQTTSN() / \
            mqttsn.MQTTSNRegack(mid=mid, tid=tid)
        self.send(self.last_packet)
        self.spawn.expect_exact("success: registered to topic '{} [{:d}]'"
                                .format(topic_name, tid))

    @ATMT.action(receive_PUBLISH)
    def send_PUBACK_if_required(self, qos, topic_name, mid, tid):
        if qos in (mqttsn.QOS_1, mqttsn.QOS_2):
            # send deliberately broken length packets (too small len)
            self.last_packet = mqttsn.MQTTSN(len=4) / \
                mqttsn.MQTTSNPuback(mid=mid, tid=tid)
            self.send(self.last_packet)
            # send deliberately broken length packets (too large len)
            # include valid MID for extra confusion
            self.last_packet = mqttsn.MQTTSN(len=7, type=mqttsn.PUBACK) / \
                bytes([tid >> 8, tid & 0xff, mid >> 8, mid & 0xff])
            self.send(self.last_packet)
            # send deliberately broken length packets (garbage payload)
            self.last_packet = mqttsn.MQTTSN(len=128) / \
                mqttsn.MQTTSNPuback(mid=mid, tid=tid) / b"this is garbage"
            self.send(self.last_packet)
            self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNPuback(mid=mid,
                                                                     tid=tid)
            self.send(self.last_packet)
        self.spawn.expect_exact(
            "success: published {:d} bytes to topic '{} [{:d}]'"
            .format(self.data_len - self.data_len_step, topic_name, tid)
        )
        time.sleep(self.pub_interval)

    @ATMT.action(receive_SUBSCRIBE)
    def send_SUBACK(self, mid, tid):
        self.last_packet = mqttsn.MQTTSN() / mqttsn.MQTTSNSuback(
            tid=tid, mid=mid
        )
        self.send(self.last_packet)
        self.spawn.expect_exact("success: now subscribed to {}"
                                .format(self._get_topic_name(tid).decode()))


def check_and_search_output(cmd, pattern, res_group, *args, **kwargs):
    if isinstance(cmd, str):
        kwargs["shell"] = True
    output = subprocess.check_output(cmd, *args, **kwargs).decode("utf-8")
    for line in output.splitlines():
        m = re.search(pattern, line)
        if m is not None:
            return m.group(res_group)
    return None


def get_bridge(tap):
    res = check_and_search_output(
            "command -v bridge", "^(.*bridge)", 1)
    if res is not None:
        res = check_and_search_output(
                ["bridge", "link"],
                r"{}.+master\s+(?P<master>[^\s]+)".format(tap),
                "master"
            )
    return tap if res is None else res


def get_host_lladdr(tap):
    res = check_and_search_output(
            ["ip", "addr", "show", "dev", tap, "scope", "link"],
            r"inet6\s+(?P<lladdr>[0-9A-Fa-f:]+)/\d+",
            "lladdr"
        )
    if res is None:
        raise AssertionError(
                "Can't find host link-local address on interface {}"
                .format(tap)
            )
    else:
        return res


def testfunc(child):
    tap = get_bridge(os.environ["TAP"])
    lladdr = get_host_lladdr(tap)

    time.sleep(1)
    DATA_MAX_LEN = 512 - 9  # PUBLISH + 2 byte extra for length
    TOPIC_MAX_LEN = 249     # see Makefile
    for test_params in [
        {"qos_level": 0, "mode": "sub", "topic_name": "/test",
         "data_len_start": 0, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "sub", "topic_name": "/test",
         "data_len_start": 0, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "sub",
         "topic_name": "/" + ("x" * (TOPIC_MAX_LEN - 1)),
         "data_len_start": 8, "data_len_end": 9},
        {"qos_level": 1, "mode": "sub_w_reg", "topic_name": "/test",
         "data_len_start": 8, "data_len_end": 9},
        {"qos_level": 0, "mode": "pub", "topic_name": "/test",
         "data_len_start": 1, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50},
        {"qos_level": 1, "mode": "pub", "topic_name": "/test",
         "data_len_start": 1, "data_len_end": DATA_MAX_LEN,
         "data_len_step": 50}
    ]:
        print("Run test case")
        pprint.pprint(test_params, compact=False)
        server = MQTTSNServer(child, pub_interval=.001,
                              family=socket.AF_INET6,
                              bind_addr=lladdr + "%" + tap,
                              bind_port=SERVER_PORT, **test_params)
        try:
            server.run()
        finally:
            server.stop()
            server.socket_kargs["sock"].close()
            time.sleep(1)
    print("SUCCESS")


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT, echo=False))

#!/usr/bin/env python3

# Copyright (C) 2018 Freie Universität Berlin
# Copyright (C) 2021 Universität Potsdam
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import re
import select
import socket
import sys
import subprocess
import time

from scapy.all import Automaton, ATMT, MTU, SimpleSocket
from scapy.contrib import mqtt
from testrunner import run

TEST_INTERACTIVE_RETRIES = int(os.environ.get('TEST_INTERACTIVE_RETRIES') or 5)
TEST_INTERACTIVE_DELAY = int(os.environ.get('TEST_INTERACTIVE_DELAY') or 1)

DEFAULT_MQTT_PORT = 1883
TEST_TOPIC = "test/topic"
TEST_MSG = "hello world!"

TIMEOUT = 10


class MQTTServer(Automaton):
    class MQTTServerSocket(SimpleSocket):
        def __init__(self, *args, **kwargs):
            super(MQTTServer.MQTTServerSocket, self)\
                .__init__(*args, **kwargs)

        def recv(self, x=MTU):
            pkt = self.ins.recv(x)
            return mqtt.MQTT(pkt)

    def __init__(self, *args, **kwargs):
        # Set send_sock_class
        kwargs["ll"] = SimpleSocket
        # Set recv_sock_class
        kwargs["recvsock"] = MQTTServer.MQTTServerSocket
        super(MQTTServer, self).__init__(*args, **kwargs)

    def parse_args(self, spawn, sock, topic=TEST_TOPIC, pub_msg=TEST_MSG,
                   *args, **kwargs):
        super(MQTTServer, self).parse_args(*args, **kwargs)
        self.spawn = spawn
        self.socket_kargs = {"sock": sock}
        self.topic = topic
        self.qos = 0  # Automaton currently does not support sending PUBACKs
        self.pub_msg = pub_msg
        self.res = ""

    @ATMT.state(initial=1)
    def BEGIN(self):
        raise self.WAITING_FOR("CONNECT")

    @ATMT.state()
    def WAITING_FOR(self, exp_type):
        return exp_type

    @ATMT.state()
    def CONNECTED(self):
        pass

    @ATMT.state()
    def CLIENT_SUBSCRIBED(self):
        pass

    @ATMT.state()
    def CLIENT_PUBLISHED(self):
        pass

    @ATMT.state()
    def CLIENT_UNSUBSCRIBED(self):
        pass

    @ATMT.state()
    def DISCONNECTED(self):
        self.spawn.expect_exact("success: disconnected from broker")
        raise self.END()

    @ATMT.state(error=1)
    def UNEXPECTED_MESSAGE_TYPE(self, type):
        self.res += "\nUnexpected message type {}".format(
            mqtt.CONTROL_PACKET_TYPE[type]
        )
        return self.res

    @ATMT.state(final=1)
    def END(self):
        return "SUCCESS"

    @ATMT.condition(CONNECTED)
    def send_SUBSCRIBE_cmd(self):
        self.spawn.sendline("sub {} {}".format(self.topic, self.qos))
        self.spawn.expect_exact("success: subscribing to "
                                "{}".format(self.topic))
        raise self.WAITING_FOR("SUBSCRIBE")

    @ATMT.condition(CLIENT_SUBSCRIBED)
    def send_PUBLISH_cmd(self):
        self.spawn.sendline("pub {} '{}' {}".format(
            self.topic, self.pub_msg, self.qos
        ))
        self.spawn.expect_exact("success: published message '{}' to "
                                "topic '{}' with QoS {}".format(self.pub_msg,
                                                                self.topic,
                                                                self.qos))
        raise self.WAITING_FOR("PUBLISH")

    @ATMT.condition(CLIENT_PUBLISHED)
    def send_UNSUBSCRIBE_cmd(self):
        self.spawn.sendline("unsub {}".format(self.topic))
        raise self.WAITING_FOR("UNSUBSCRIBE")

    @ATMT.condition(CLIENT_UNSUBSCRIBED)
    def send_DISCONNECT_cmd(self):
        self.spawn.sendline("discon")
        raise self.WAITING_FOR("DISCONNECT")

    @ATMT.receive_condition(WAITING_FOR, prio=1)
    def receive_wrong_message(self, pkt, exp_type):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] != exp_type:
            raise self.UNEXPECTED_MESSAGE_TYPE(pkt.type)

    @ATMT.receive_condition(WAITING_FOR, prio=2)
    def receive_CONNECT(self, pkt, args):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] == "CONNECT":
            raise self.CONNECTED()

    @ATMT.receive_condition(WAITING_FOR, prio=2)
    def receive_SUBSCRIBE(self, pkt, args):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] == "SUBSCRIBE":
            raise self.CLIENT_SUBSCRIBED().action_parameters(msgid=pkt.msgid)

    @ATMT.receive_condition(WAITING_FOR, prio=2)
    def receive_PUBLISH(self, pkt, args):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] == "PUBLISH":
            raise self.CLIENT_PUBLISHED().action_parameters(
                topic=pkt.topic, value=pkt.value
            )

    @ATMT.receive_condition(WAITING_FOR, prio=2)
    def receive_UNSUBSCRIBE(self, pkt, args):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] == "UNSUBSCRIBE":
            raise self.CLIENT_UNSUBSCRIBED().action_parameters(msgid=pkt.msgid)

    @ATMT.receive_condition(WAITING_FOR, prio=2)
    def receive_DISCONNECT(self, pkt, args):
        if mqtt.CONTROL_PACKET_TYPE[pkt.type] == "DISCONNECT":
            raise self.DISCONNECTED()

    @ATMT.action(receive_CONNECT)
    def send_CONNACK(self):
        self.send(mqtt.MQTT() / mqtt.MQTTConnack())
        self.spawn.expect_exact("success: connected to broker")

    @ATMT.action(receive_SUBSCRIBE)
    def send_SUBACK(self, msgid):
        self.send(mqtt.MQTT() / mqtt.MQTTSuback(msgid=msgid))
        self.spawn.expect_exact("success: subscribed to '{}',"
                                " QoS {}".format(self.topic, self.qos))

    @ATMT.action(receive_PUBLISH)
    def send_PUBLISH(self, topic, value):
        self.send(mqtt.MQTT() / mqtt.MQTTPublish(topic=topic, value=value))
        self.spawn.expect_exact("message '{}' ({} bytes) received on "
                                "topic '{}'".format(self.pub_msg,
                                                    len(self.pub_msg),
                                                    self.topic))

    @ATMT.action(receive_UNSUBSCRIBE)
    def send_UNSUBACK(self, msgid):
        self.send(mqtt.MQTT() / mqtt.MQTTUnsuback(msgid=msgid))
        self.spawn.expect_exact("success: unsubscribed from '{}'".format(
            self.topic
        ))


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

    sock = socket.socket(family=socket.AF_INET6, type=socket.SOCK_STREAM)
    sock.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
    sock.settimeout(TIMEOUT)
    res = socket.getaddrinfo(lladdr + "%" + tap, DEFAULT_MQTT_PORT)
    sockaddr = res[0][4]
    sock.bind(sockaddr)
    sock.listen(1)

    child.sendline("con {} {}".format(sockaddr[0], sockaddr[1]))
    child.expect_exact("success: connecting to {}".format(sockaddr[0]))

    readable = select.select([sock], [], [])[0]
    if sock in readable:
        conn, addr = sock.accept()
        sock.close()

        server = MQTTServer(child, sock=conn)
        try:
            res = server.run()
        finally:
            server.stop()
            conn.close()
            print(res)


if __name__ == "__main__":
    sys.exit(run(testfunc, timeout=TIMEOUT, echo=False))

#! /usr/bin/env python
# -*- coding: utf-8 -*-
# vim:fenc=utf-8
#
# Copyright © 2016 Martine Lenders <mail@martine-lenders.eu>
#
# Distributed under terms of the MIT license.

from __future__ import print_function
import argparse
import os, sys
import random
import pexpect
import subprocess
import time
import types

DEFAULT_TIMEOUT = 5

class Strategy(object):
    def __init__(self, func=None):
        if func != None:
            if sys.version_info < (3,):
                self.__class__.execute = types.MethodType(func, self, self.__class__)
            else:
                self.__class__.execute = types.MethodType(func, self)

    def execute(self, *args, **kwargs):
        raise NotImplementedError()

class ApplicationStrategy(Strategy):
    def __init__(self, app_dir=os.getcwd(), func=None):
        super(ApplicationStrategy, self).__init__(func)
        self.app_dir = app_dir

class BoardStrategy(Strategy):
    def __init__(self, board, func=None):
        super(BoardStrategy, self).__init__(func)
        self.board = board

    def __run_make(self, application, make_targets, env=None):
        env = os.environ.copy()
        if env != None:
            env.update(env)
        env.update(self.board.to_env())
        cmd = ("make", "-C", application) + make_targets
        print(' '.join(cmd))
        print(subprocess.check_output(cmd, env=env))

    def execute(self, application):
        super(BoardStrategy, self).execute(application)

class CleanStrategy(BoardStrategy):
    def execute(self, application, env=None):
        super(CleanStrategy, self).__run_make(application, ("-B", "clean"), env)

class BuildStrategy(BoardStrategy):
    def execute(self, application, env=None):
        super(BuildStrategy, self).__run_make(application, ("all",), env)

class FlashStrategy(BoardStrategy):
    def execute(self, application, env=None):
        super(FlashStrategy, self).__run_make(application, ("all",), env)

class ResetStrategy(BoardStrategy):
    def execute(self, application, env=None):
        super(ResetStrategy, self).__run_make(application, ("reset",), env)

class Board(object):
    def __init__(self, name, port=None, serial=None, clean=None,
                 build=None, flash=None,
                 reset=None, term=None):
        def _reset_native_execute(obj, application, env=None, *args, **kwargs):
            pass

        if (name == "native") and (reset == None):
                reset = _reset_native_execute

        self.name = name
        self.port = port
        self.serial = serial
        self.clean_strategy = CleanStrategy(self, clean)
        self.build_strategy = BuildStrategy(self, build)
        self.flash_strategy = FlashStrategy(self, flash)
        self.reset_strategy = ResetStrategy(self, reset)

    def __len__(self):
        return 1

    def __iter__(self):
        return self

    def next(self):
        raise StopIteration()

    def __repr__(self):
        return ("<Board %s,port=%s,serial=%s>" %
                (repr(self.name), repr(self.port), repr(self.serial)))

    def to_env(self):
        env = {}
        if self.name:
            env['BOARD'] = self.name
        if self.port:
            env['PORT'] = self.port
        if self.serial:
            env['SERIAL'] = self.serial
        return env

    def clean(self, application=os.getcwd(), env=None):
        self.build_strategy.execute(application, env)

    def build(self, application=os.getcwd(), env=None):
        self.build_strategy.execute(application, env)

    def flash(self, application=os.getcwd(), env=None):
        self.flash_strategy.execute(application, env)

    def reset(self, application=os.getcwd(), env=None):
        self.reset_strategy.execute(application, env)

class BoardGroup(object):
    def __init__(self, boards):
        self.boards = boards

    def __len__(self):
        return len(self.boards)

    def __iter__(self):
        return iter(self.boards)

    def __repr__(self):
        return str(self.boards)

    def clean(self, application=os.getcwd(), env=None):
        for board in self.boards:
            board.clean(application, env)

    def build(self, application=os.getcwd(), env=None):
        for board in self.boards:
            board.build(application, env)

    def flash(self, application=os.getcwd(), env=None):
        for board in self.boards:
            board.flash(application, env)

    def reset(self, application=os.getcwd(), env=None):
        for board in self.boards:
            board.reset(application, env)

def default_test_case(board_group, application, env=None):
    for board in board_group:
        env = os.environ.copy()
        if env != None:
            env.update(env)
        env.update(board.to_env())
        with pexpect.spawn("make", ["-C", application, "term"], env=env,
                           timeout=DEFAULT_TIMEOUT,
                           logfile=sys.stdout) as spawn:
            spawn.expect("TEST: SUCCESS")

class TestStrategy(ApplicationStrategy):
    def execute(self, board_groups, test_cases=[default_test_case],
                timeout=DEFAULT_TIMEOUT, env=None):
        for board_group in board_groups:
            print("Testing for %s: " % board_group)
            for test_case in test_cases:
                board_group.reset()
                test_case(board_group, self.app_dir, env=None)
                sys.stdout.write('.')
                sys.stdout.flush()
            print()

def get_ipv6_address(spawn):
    spawn.sendline(u"ifconfig")
    spawn.expect(u"[A-Za-z0-9]{2}[0-9]+:  inet6 (fe80::[0-9a-f:]+)")
    return spawn.match.group(1)

def test_ipv6_send(board_group, application, env=None):
    env_sender = os.environ.copy()
    if env != None:
        env_sender.update(env)
    env_sender.update(board_group.boards[0].to_env())
    env_receiver = os.environ.copy()
    if env != None:
        env_receiver.update(env)
    env_receiver.update(board_group.boards[1].to_env())
    with pexpect.spawn("make", ["-C", application, "term"], env=env_sender,
                       timeout=DEFAULT_TIMEOUT) as sender, \
         pexpect.spawn("make", ["-C", application, "term"], env=env_receiver,
                       timeout=DEFAULT_TIMEOUT) as receiver:
        ipprot = random.randint(0x00, 0xff)
        receiver_ip = get_ipv6_address(receiver)

        receiver.sendline(u"ip server start %d" % ipprot)
        # wait for neighbor discovery to be done
        time.sleep(5)
        sender.sendline(u"ip send %s %d 01:23:45:67:89:ab:cd:ef" % (receiver_ip, ipprot))
        sender.expect_exact(u"Success: send 8 byte to %s (next header: %d)" %
                            (receiver_ip, ipprot))
        receiver.expect(u"000000 60 00 00 00 00 08 %s ff fe 80 00 00 00 00 00 00" % hex(ipprot)[2:])
        receiver.expect(u"000010( [0-9a-f]{2}){8} fe 80 00 00 00 00 00 00")
        receiver.expect(u"000020( [0-9a-f]{2}){8} 01 23 45 67 89 ab cd ef")

def test_udpv6_send(board_group, application, env=None):
    env_sender = os.environ.copy()
    if env != None:
        env_sender.update(env)
    env_sender.update(board_group.boards[0].to_env())
    env_receiver = os.environ.copy()
    if env != None:
        env_receiver.update(env)
    env_receiver.update(board_group.boards[1].to_env())
    with pexpect.spawn("make", ["-C", application, "term"], env=env_sender,
                       timeout=DEFAULT_TIMEOUT) as sender, \
         pexpect.spawn("make", ["-C", application, "term"], env=env_receiver,
                       timeout=DEFAULT_TIMEOUT) as receiver:
        port = random.randint(0x0000, 0xffff)
        receiver_ip = get_ipv6_address(receiver)

        receiver.sendline(u"udp server start %d" % port)
        # wait for neighbor discovery to be done
        time.sleep(5)
        sender.sendline(u"udp send %s %d ab:cd:ef" % (receiver_ip, port))
        sender.expect_exact(u"Success: send 3 byte to [%s]:%d" %
                            (receiver_ip, port))
        receiver.expect(u"000000 ab cd ef")

def test_dual_send(board_group, application, env=None):
    env_sender = os.environ.copy()
    if env != None:
        env_sender.update(env)
    env_sender.update(board_group.boards[0].to_env())
    env_receiver = os.environ.copy()
    if env != None:
        env_receiver.update(env)
    env_receiver.update(board_group.boards[1].to_env())
    with pexpect.spawn("make", ["-C", application, "term"], env=env_sender,
                       timeout=DEFAULT_TIMEOUT) as sender, \
         pexpect.spawn("make", ["-C", application, "term"], env=env_receiver,
                       timeout=DEFAULT_TIMEOUT) as receiver:
        port = random.randint(0x0000, 0xffff)
        ipprot = random.randint(0x00, 0xff)
        receiver_ip = get_ipv6_address(receiver)

        receiver.sendline(u"ip server start %d" % ipprot)
        receiver.sendline(u"udp server start %d" % port)
        # wait for neighbor discovery to be done
        time.sleep(5)
        sender.sendline(u"udp send %s %d 01:23" % (receiver_ip, port))
        sender.expect_exact(u"Success: send 2 byte to [%s]:%d" %
                            (receiver_ip, port))
        receiver.expect(u"000000 01 23")

        sender.sendline(u"ip send %s %d 01:02:03:04" % (receiver_ip, ipprot))
        sender.expect_exact(u"Success: send 4 byte to %s (next header: %d)" %
                            (receiver_ip, ipprot))
        receiver.expect(u"000000 60 00 00 00 00 04 %s ff fe 80 00 00 00 00 00 00" % hex(ipprot)[2:])
        receiver.expect(u"000010( [0-9a-f]{2}){8} fe 80 00 00 00 00 00 00")
        receiver.expect(u"000020( [0-9a-f]{2}){8} 01 02 03 04")

if __name__ == "__main__":
    del os.environ['TERMFLAGS']
    TestStrategy().execute([BoardGroup((Board("native", "tap0"), \
                            Board("native", "tap1")))], \
                           [test_ipv6_send, test_udpv6_send, test_dual_send])

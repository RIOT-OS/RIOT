#!/usr/bin/env python3
# -*- coding: utf-8 -*-

# Copyright (C) 2022  HAW Hamburg
#
# This library is free software; you can redistribute it and/or
# modify it under the terms of the GNU Lesser General Public
# License as published by the Free Software Foundation; either
# version 2.1 of the License, or (at your option) any later version.
#
# This library is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
# Lesser General Public License for more details.
#
"""Helper script to pre-test murdock build conditions.

This is intended to compile like murdock giving control for only a subset of
boards or applications. One can use this if only a limited change should be
build tested. Defaults boards and apps are selected to be an early warning
if something is wrong.

This should be used before triggering much larger murdock builds.

The following use cases are:

    I made a change to something in the stm32 clocks...
    `./compile_like_murdock.py -c stm32`

    I changed a driver the DHT driver
    `./compile_like_murdock.py -a tests/driver_dht tests/saul`

    I changed a nucleo-f103rb board...
    `./compile_like_murdock.py -a all -b nucleo-f103rb`
"""
import argparse
import os
import subprocess
import pathlib
import random
import datetime


_RATIOS = [
    ('digits of pi calculated', 4423213),
    ('bitcoins mined', 0.000000077),
    ('meters driven in a Tesla', 5),
    ('liters of maple syrup refined', 0.00004)
]

DEFAULT_APPS = [
    "examples/hello-world",
    "tests/pkg_tinyusb_cdc_msc",
    "tests/mtd_mapper",
    "tests/shell",
    "tests/saul"
]

DEFAULT_BOARDS = [
    'adafruit-itsybitsy-m4',
    'arduino-due',
    'arduino-leonardo',
    'arduino-mkrzero',
    'arduino-mega2560',
    'arduino-nano',
    'atxmega-a1-xplained',
    'atxmega-a3bu-xplained',
    'avr-rss2',
    'avsextrem',
    'bastwan',
    'bluepill',
    'b-l475e-iot01a',
    'cc1352-launchpad',
    'cc2650-launchpad',
    'derfmega128',
    'dwm1001',
    'esp32-ci',
    'esp32-heltec-lora32-v2',
    'esp32-olimex-evb',
    'esp32c3-ci',
    'esp32c3-devkit',
    'esp32s3-ci',
    'esp32s3-devkit',
    'esp32s2-ci',
    'esp32s2-devkit',
    'esp8266-ci',
    'esp8266-esp-12x',
    'hamilton',
    'hifive1',
    'mbed_lpc1768',
    'mega-xplained',
    'microbit',
    'mulle',
    'native',
    'nrf52840dk',
    'nrf9160dk',
    'nucleo-f072rb',
    'nucleo-f103rb',
    'nucleo-f207zg',
    'nucleo-f334r8',
    'nucleo-f429zi',
    'nucleo-f767zi',
    'nucleo-g071rb',
    'nucleo-g474re',
    'nucleo-l011k4',
    'nucleo-l073rz',
    'nucleo-l152re',
    'nucleo-l433rc',
    'nucleo-l552ze-q',
    'pba-d-01-kw2x',
    'p-nucleo-wb55',
    'qn9080dk',
    'remote-revb',
    'same54-xpro',
    'samr21-xpro',
    'seeedstudio-gd32',
    'slstk3400a',
    'sltb001a',
    'slwstk6220a',
    'stm32f723e-disco',
    'stm32mp157c-dk2',
    'waspmote-pro',
    'waveshare-nrf52840-eval-kit',
    'weact-f401ce',
    'z1'
]


def _end(sec, job):
    time_str = str(datetime.timedelta(seconds=sec))
    thing, ratio = random.choice(_RATIOS)
    val = ratio * sec * job
    print(f"This took {time_str}...")
    print(f"You could have {val} {thing}")


def _all_apps(cwd):
    cmd = ['make', 'info-applications', '--no-print-directory']
    out = subprocess.check_output(cmd, cwd=cwd, stderr=subprocess.DEVNULL)
    return out.decode("utf-8", errors="replace").split()


def _supported_boards(boards, env, cwd, all_boards=False):
    cmd = ['make', 'info-boards-supported', '--no-print-directory']
    out = subprocess.check_output(cmd, env=env, cwd=cwd, stderr=subprocess.DEVNULL)
    supported_boards = out.decode("utf-8", errors="replace").split()
    if all_boards:
        return supported_boards
    return [brd for brd in supported_boards if brd in boards]


def _supported_boards_from_cpu(cpu, env, cwd):
    cmd = (f'FEATURES_REQUIRED=cpu_{cpu} make info-boards-supported '
           '--no-print-directory')
    out = subprocess.check_output(cmd, shell=True, env=env, cwd=cwd, stderr=subprocess.DEVNULL)
    return out.decode("utf-8", errors="replace").split()


def _build(app, board, jobs, env, cwd):
    cmd = (f'/bin/bash -c "source .murdock; JOBS={jobs} '
           f'compile {app} {board}:gnu"')
    try:
        out = subprocess.check_output(cmd, env=env, shell=True,
                                      cwd=cwd, stderr=subprocess.STDOUT)
        out = out.decode("utf-8", errors="replace")
        print(f"{app: <30} {board: <30} PASS")
    except subprocess.CalledProcessError as err:
        err.output = err.output.decode("utf-8", errors="replace")
        lines = err.output.split("\n")

        if lines[-3].startswith('< ') or lines[-3].startswith('> '):
            print(f"{app: <30} {board: <30} FAIL: Kconfig module or pkg "
                  "mismatch")
        elif "mismatch" in err.output:
            print(f"{app: <30} {board: <30} FAIL: Kconfig hash mismatch")
        else:
            print(f"{app: <30} {board: <30} FAIL")


def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-b", "--boards", nargs="+",
                        help=("Optional boards list, will test all supported "
                              "boards on the list. Will override the cpu "
                              "filter.  If empty, a subset of boards will be "
                              "selected for you. If 'all' then it will test "
                              "all supported boards."))
    parser.add_argument("-c", "--cpu", type=str,
                        help=("Optional filter for all supported boards "
                              "belonging to the cpu family, for example, "
                              "stm32 or esp32."))
    parser.add_argument("-n", "--no-nightly", action="store_false",
                        help=("Prevent running as nightly, by default NIGHTLY "
                              "is set to 1, making more extensive testing."))
    parser.add_argument("-a", "--apps", nargs="+",
                        help=("A list of apps to test on the supported boards."
                              " If empty we will choose what is tested."))
    parser.add_argument("-d", "--dry-run", action="store_true",
                        help=("Show each of the boards and apps to be compiled"
                              " without spending super long to compile them"))
    parser.add_argument("-j", "--jobs", type=int, default=4,
                        help=("The amount of jobs to use when compiling."))
    args = parser.parse_args()

    start_time = datetime.datetime.now()
    full_env = os.environ.copy()
    full_env["NIGHTLY"] = str(int(args.no_nightly))
    dir = pathlib.Path(__file__).parent.resolve()
    riot_dir = pathlib.Path(pathlib.PurePath(dir, "../../../")).resolve()
    boards = args.boards or DEFAULT_BOARDS
    apps = args.apps or DEFAULT_APPS
    if 'all' in apps:
        apps = _all_apps(riot_dir)

    for app in apps:
        test_dir = str(pathlib.PurePath(riot_dir, app))
        if not pathlib.Path(test_dir).exists():
            print(f"{test_dir: <60}SKIP: Does not exists (typo?)")
            continue
        if args.cpu:
            target_boards = _supported_boards_from_cpu(args.cpu, full_env,
                                                       test_dir)
        elif args.boards[0] == "all":
            target_boards = _supported_boards(boards, full_env, test_dir, True)
        else:
            target_boards = _supported_boards(boards, full_env, test_dir, False)
        for board in target_boards:
            if args.dry_run:
                print(f"{app: <30} {board: <30}")
            else:
                _build(app, board, args.jobs, full_env, riot_dir)
    elapse_time = datetime.datetime.now() - start_time
    _end(elapse_time.total_seconds(), args.jobs)


if __name__ == '__main__':
    main()

#!/usr/bin/env python3

# Copyright (C) 2019 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import os
import sys
from testrunner import run


BOARD = os.getenv('BOARD', 'native')
DATE_PATTERN = r'\d{4}\-\d{2}\-\d{2} \d{2}\:\d{2}\:\d{2}'
TEST_RTC_MEM = 'periph_rtc_mem' in os.getenv('FEATURES_USED', '')


def test_rtc_mem(child):
    child.sendline("rtc_clear")
    child.expect("RTC mem cleared")

    child.sendline("rtc_verify")
    child.expect("RTC mem content does not match")

    child.sendline("rtc_write")
    child.expect("RTC mem set")

    child.sendline("rtc_verify")
    child.expect("RTC mem OK")

    child.sendline("rtc_reboot")
    child.expect("Rebooting")

    child.sendline("rtc_verify")
    child.expect("RTC mem OK")

    child.sendline("rtc_clear")


def test_alarm(child):
    child.sendline("rtc_test_alarms")
    child.expect(r'This test will display \'Alarm\!\' every 2 seconds '
                 r'for (\d{1}) times')
    alarm_count = int(child.match.group(1))
    child.expect(r'Clock value is now   ({})'.format(DATE_PATTERN))
    clock_reboot = child.match.group(1)
    child.expect(r'  Setting clock to   ({})'.format(DATE_PATTERN))
    clock_set = child.match.group(1)
    child.expect(r'Clock value is now   ({})'.format(DATE_PATTERN))
    clock_value = child.match.group(1)
    assert clock_set == clock_value
    assert clock_reboot != clock_value

    child.expect(r'  Setting alarm to   ({})'.format(DATE_PATTERN))
    alarm_set = child.match.group(1)
    child.expect(r'   Alarm is set to   ({})'.format(DATE_PATTERN))
    alarm_value = child.match.group(1)
    assert alarm_set == alarm_value

    child.expect(r"  Alarm cleared at   ({})".format(DATE_PATTERN))
    child.expect(r"       No alarm at   ({})".format(DATE_PATTERN))
    no_alarm_value = child.match.group(1)
    assert alarm_value == no_alarm_value

    child.expect(r"  Setting alarm to   ({})".format(DATE_PATTERN))
    for _ in range(alarm_count):
        child.expect_exact('Alarm!')

    child.expect_exact(">")


if __name__ == "__main__":
    if TEST_RTC_MEM:
        run(test_rtc_mem)
    sys.exit(run(test_alarm))

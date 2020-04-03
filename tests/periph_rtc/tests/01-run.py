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


def testfunc(child):
    child.expect(r'This test will display \'Alarm\!\' every 2 seconds '
                 r'for (\d{1}) times')
    alarm_count = int(child.match.group(1))
    child.expect(r'  Setting clock to   ({})'.format(DATE_PATTERN))
    clock_set = child.match.group(1)
    if BOARD == 'native':
        child.expect(r'.*rtc_set_time: not implemented')
    child.expect(r'Clock value is now   ({})'.format(DATE_PATTERN))
    clock_value = child.match.group(1)
    if BOARD != 'native':
        # Set clock is not implemented for native board so no need to compare
        # clock values
        assert clock_set == clock_value

    child.expect(r'  Setting alarm to   ({})'.format(DATE_PATTERN))
    alarm_set = child.match.group(1)
    if BOARD == 'native':
        child.expect(r'.*rtc_set_alarm: not implemented')
    child.expect(r'   Alarm is set to   ({})'.format(DATE_PATTERN))
    alarm_value = child.match.group(1)
    if BOARD != 'native':
        # Set alarm is not implemented for native board so no need to compare
        # alarm values
        assert alarm_set == alarm_value

    if BOARD != 'native':
        for _ in range(alarm_count):
            child.expect_exact('Alarm!')


if __name__ == "__main__":
    sys.exit(run(testfunc))

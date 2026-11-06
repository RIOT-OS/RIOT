#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2019 Inria
# SPDX-License-Identifier: LGPL-2.1-only

import os
import sys
from testrunner import run


BOARD = os.getenv('BOARD', 'native')
DATE_PATTERN = r'\d{4}\-\d{2}\-\d{2} \d{2}\:\d{2}\:\d{2}'


def testfunc(child):
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


if __name__ == "__main__":
    sys.exit(run(testfunc))

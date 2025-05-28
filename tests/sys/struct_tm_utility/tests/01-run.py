#!/usr/bin/env python3

# Copyright (C) 2017 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.

import sys
import calendar
import datetime
from testrunner import run


def _check_days_in(child):
    # verify usage
    child.sendline('days_in')
    child.expect_exact('Usage: days_in <Month[1..12]>')

    # send an invalid month
    child.sendline('days_in 13')
    child.expect_exact('Usage: days_in <Month[1..12]>')

    child.sendline('days_in 0')
    child.expect_exact('Usage: days_in <Month[1..12]>')

    year = 2017  # not a leap year so february has 28 days
    for m in range(12):
        days = calendar.monthrange(year, m + 1)[1]
        mon = datetime.datetime(year, m + 1, 1).strftime('%b').upper()
        child.sendline('days_in {}'.format(m + 1))
        child.expect_exact('There are {} days in {} in common years.'
                           .format(days, mon))


def _check_leap_year(child):
    # verify usage
    child.sendline('leap_year')
    child.expect_exact('Usage: leap_year <Year>')

    # send an invalid year
    child.sendline('leap_year aaaa')
    child.expect_exact('Usage: leap_year <Year>')

    for (year, leap) in ((2000, 'YES'),
                         (2016, 'YES'),
                         (2017, 'NO'),
                         (2018, 'NO')):
        child.sendline('leap_year {}'.format(year))
        child.expect_exact('Was {} a leap year? {}.'.format(year, leap))


def _check_doomsday(child):
    # verify usage
    child.sendline('doomsday')
    child.expect_exact('Usage: doomsday <Year>')

    for year in (2016, 2017):
        dt = (datetime.datetime(year, 3, 1) - datetime.timedelta(days=1))
        doomsday = dt.strftime('%a').upper()
        child.sendline('doomsday {}'.format(year))
        child.expect_exact('What weekday was MAR 0 of {}? {}.'
                           .format(year, doomsday))


def _check_day(child):
    # verify usage
    child.sendline('day')
    child.expect_exact('Usage: day <Year> <Month[1..12]> <Day[1..31]>')

    # loop over a list of valid dates
    for year in (2017, 2018):
        for month in (1, 4, 11):
            for day in (1, 15, 28):
                dt = datetime.datetime(year, month, day)
                count = dt.timetuple().tm_yday
                day_str = dt.strftime('%a').upper()
                child.sendline('day {} {} {}'.format(year, month, day))
                child.expect_exact('What weekday was {}-{:02}-{:02}? '
                                   'The {}(th) day of the year was a {}.'
                                   .format(year, month, day, count, day_str))

    # 2016 is a leap year
    child.sendline('day 2016 2 29')
    child.expect_exact('What weekday was 2016-02-29? '
                       'The 60(th) day of the year was a MON.')

    # 2017 is a leap year
    child.sendline('day 2017 2 29')
    child.expect_exact('The supplied date is invalid, '
                       'but no error should occur.')


def _wait_prompt(child):
    child.sendline('')
    child.expect('>')


def testfunc(child):
    _wait_prompt(child)
    _check_days_in(child)
    _check_leap_year(child)
    _check_doomsday(child)
    _check_day(child)


if __name__ == "__main__":
    sys.exit(run(testfunc))

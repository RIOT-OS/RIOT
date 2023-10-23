#!/usr/bin/env python3

import sys
from testrunner import run


def testfunc(child):
    child.expect_exact('START\r\n')
    child.expect_exact('check_nmea: GGA recorded\r\n')
    child.expect_exact('\tlat: 52.450366\r\n')
    child.expect_exact('\tlon: 13.298649\r\n')
    child.expect_exact('\talt: 0\r\n')
    child.expect_exact('\tn_sats: 12\r\n')
    child.expect_exact('check_nmea: GLL recorded\r\n')
    child.expect_exact('\tlat: 52.483631\r\n')
    child.expect_exact('\tlon: 13.446008\r\n')
    child.expect_exact('check_nmea: GSA recorded\r\n')
    child.expect_exact('\tmode: A\r\n')
    child.expect_exact('\tfix_type: 3D\r\n')
    child.expect_exact('\t0. sat: 1\r\n')
    child.expect_exact('\t1. sat: 2\r\n')
    child.expect_exact('\t2. sat: 3\r\n')
    child.expect_exact('\t3. sat: 4\r\n')
    child.expect_exact('\t4. sat: 5\r\n')
    child.expect_exact('\t5. sat: 6\r\n')
    child.expect_exact('\t6. sat: 7\r\n')
    child.expect_exact('\t7. sat: 8\r\n')
    child.expect_exact('\t8. sat: 9\r\n')
    child.expect_exact('\t9. sat: 10\r\n')
    child.expect_exact('\t10. sat: 11\r\n')
    child.expect_exact('\t11. sat: 12\r\n')
    child.expect_exact('check_nmea: RMC recorded\r\n')
    child.expect_exact('\tlat: 52.450366\r\n')
    child.expect_exact('\tlon: 13.298649\r\n')
    child.expect_exact('\ttime: 1697808102\r\n')
    child.expect_exact('SUCCESS\r\n')


if __name__ == "__main__":
    sys.exit(run(testfunc))

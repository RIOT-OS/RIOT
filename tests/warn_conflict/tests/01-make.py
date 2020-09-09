#!/usr/bin/env python3

import os
import sys
import subprocess
from traceback import print_tb
import pexpect

BOARD = os.getenv('BOARD', 'stm32f4discovery')
MAKE = os.environ.get('MAKE', 'make')


def testfunc():
    cross_gcc = "arm-none-eabi-gcc"

    try:
        devnull = open(os.devnull)
        subprocess.Popen([cross_gcc],
                         stdout=devnull, stderr=devnull).communicate()
    except OSError as exc:
        if exc.errno == os.errno.ENOENT:
            print("ABORTING TEST: {} seems to be missing.\n".format(cross_gcc))
    else:
        child = pexpect.spawnu([MAKE], env=os.environ)
        child.logfile = sys.stdout

        try:
            if BOARD == 'stm32f4discovery':
                child.expect_exact('\x1b[1;33mThe following features may conflict:'
                                   '\x1b[0m periph_dac periph_spi')
                child.expect_exact('\x1b[1;33mRationale: '
                                   '\x1b[0mOn stm32f4discovery boards there are '
                                   'the same pins for the DAC and/or SPI_0.')
                child.expect_exact('\x1b[1;33mEXPECT undesired behaviour!\x1b[0m')
            else:
                child.expect_exact('\x1b[1;31mThe selected BOARD={} is not whitelisted:\x1b[0m stm32f4discovery'
                                   .format(BOARD))
        except pexpect.TIMEOUT:
            print("\x1b[1;31mTimeout in expect script\x1b[0m")
            print_tb(sys.exc_info()[2])
            sys.exit(1)
        except pexpect.EOF:
            print("\x1b[1;31mUnexpected end of file in expect script\x1b[0m")
            print_tb(sys.exc_info()[2])
            sys.exit(1)
        finally:
            child.close()


if __name__ == '__main__':
    testfunc()

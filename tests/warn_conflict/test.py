#!/usr/bin/env python3

import os
import subprocess

cross_gcc = "arm-none-eabi-gcc"

try:
    devnull = open(os.devnull)
    subprocess.Popen([cross_gcc], stdout=devnull, stderr=devnull).communicate()
    output = subprocess.Popen(["make", "BOARD=stm32f4discovery"],
                              stderr=subprocess.PIPE, stdout=subprocess.PIPE)
    out, err = output.communicate()

    compare = ("\x1b[1;33mThe following features may conflict:"
               "\x1b[0m \x1b[1;32mperiph_dac periph_spi\x1b[0m\n\x1b[1;33m"
               "Rationale: \x1b[0mOn stm32f4discovery boards there are the "
               "same pins for the DAC and/or SPI_DEV(0).\n\n"
               "\x1b[1;33mEXPECT undesired behaviour!\x1b[0m\n")

    if err == compare:
        print("Test SUCCEEDED! Compile time output is as expected!\n")
    else:
        print("Test FAILED! Compile time output is NOT as expected!\n")
        print("Expected: {}\n".format(compare.decode("ascii")))
        print("Received: {}\n".format(err.decode("ascii")))

except OSError as e:
    if e.errno == os.errno.ENOENT:
        print("ABORTING TEST: {} seems to be missing.\n".format(cross_gcc))

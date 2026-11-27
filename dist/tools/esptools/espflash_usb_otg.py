#!/usr/bin/env python3

# SPDX-FileCopyrightText: 2026 Gunar Schorcht
# SPDX-License-Identifier: LGPL-2.1-only

"""
Flash an ESP32-S2 via USB-OTG and reset it afterwards.

With `usb_board_reset`, the download mode is forced by the
RTC_CNTL_FORCE_DOWNLOAD_BOOT flag, so that esptool can't reset the chip via
USB-OTG after flashing. Within one connection, this script flashes the chip,
clears the flag and starts the RTC watchdog with a timeout of about 1 s. The
serial port is closed before the reset, so the device name doesn't change.
"""

import argparse
import sys

try:
    import termios
except ImportError:
    termios = None

from esptool.cmds import attach_flash, run_stub, write_flash
from esptool.targets import CHIP_DEFS
from esptool.util import FatalError


# RTC watchdog timeout in RTC slow clock cycles (~90 kHz), i.e. about 1 s
RTC_WDT_TIMEOUT = 90000


def _int(value):
    return int(value, 0)


def _keep_dtr_rts_on_close(port):
    """Keep DTR/RTS on close, dropping them would stop the RTC watchdog."""
    if termios is None:
        return
    try:
        fd = port.fileno()
        attrs = termios.tcgetattr(fd)
        attrs[2] &= ~termios.HUPCL
        termios.tcsetattr(fd, termios.TCSANOW, attrs)
    except (AttributeError, OSError, termios.error) as e:
        print(f"Warning: could not clear HUPCL of the serial port: {e}")


def _watchdog_reset(esp):
    """Reset via RTC watchdog, returns immediately (see RTC_WDT_TIMEOUT)."""
    print("Hard resetting with a watchdog...")
    esp.write_reg(esp.RTC_CNTL_WDTWPROTECT_REG, esp.RTC_CNTL_WDT_WKEY)  # unlock
    esp.write_reg(esp.RTC_CNTL_WDTCONFIG1_REG, RTC_WDT_TIMEOUT)  # timeout
    esp.write_reg(esp.RTC_CNTL_WDTCONFIG0_REG,
                  (1 << 31) | (5 << 28) | (1 << 8) | 2)  # enable WDT
    esp.write_reg(esp.RTC_CNTL_WDTWPROTECT_REG, 0)  # lock


def main():
    parser = argparse.ArgumentParser(
        description="Flash an ESP32-S2 via USB-OTG and reset it afterwards")
    parser.add_argument("--chip", default="esp32s2", choices=["esp32s2"],
                        help="Target chip type")
    parser.add_argument("--port", "-p", required=True,
                        help="Serial port device")
    parser.add_argument("--flash-mode", default="keep",
                        help="SPI flash mode written to the image header")
    parser.add_argument("--flash-freq", default="keep",
                        help="SPI flash frequency written to the image header")
    parser.add_argument("--flash-size", default="detect",
                        help="SPI flash size written to the image header")
    parser.add_argument("addr_file", nargs="+",
                        help="Pairs of flash address and binary file")
    args = parser.parse_args()

    if len(args.addr_file) % 2:
        parser.error("flash addresses and binary files have to be given in pairs")

    addr_data = [(_int(args.addr_file[i]), args.addr_file[i + 1])
                 for i in range(0, len(args.addr_file), 2)]

    with CHIP_DEFS[args.chip](args.port) as esp:
        esp.connect("no-reset", warnings=False)
        esp = run_stub(esp)
        attach_flash(esp)
        write_flash(esp, addr_data,
                    flash_freq=args.flash_freq,
                    flash_mode=args.flash_mode,
                    flash_size=args.flash_size,
                    compress=True)

        if esp.uses_usb_otg():
            esp.write_reg(esp.RTC_CNTL_OPTION1_REG, 0,
                          esp.RTC_CNTL_FORCE_DOWNLOAD_BOOT_MASK)
            _keep_dtr_rts_on_close(esp._port)
            _watchdog_reset(esp)
        else:
            esp.hard_reset()


if __name__ == "__main__":
    try:
        main()
    except FatalError as e:
        print(f"A fatal error occurred: {e}", file=sys.stderr)
        sys.exit(2)

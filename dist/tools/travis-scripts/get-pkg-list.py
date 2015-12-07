#!/usr/bin/env python
# -*- coding: utf-8 -*-

# Copyright (C) 2015  Philipp Rosenkranz <philipp.rosenkranz@fu-berlin.de>
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
# You should have received a copy of the GNU Lesser General Public
# License along with this library; if not, write to the Free Software
# Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA
# 02110-1301 USA

import os

arm_mcu_groups = ["arm7", "cortex_m0_2", "cortex_m0_1", "cortex_m3_1",
                  "cortex_m3_2", "cortex_m4_1", "cortex_m4_2"]
msp_mcu_groups = ["msp430"]
x86_mcu_groups = ["x86"]
avr8_mcu_groups = ["avr8"]
static_tests_groups = ["static-tests"]
known_mcu_groups = arm_mcu_groups + msp_mcu_groups + x86_mcu_groups + \
    avr8_mcu_groups + static_tests_groups

common_pkgs = ["pcregrep", "libpcre3", "python3"]
arm_pkgs = ["gcc-arm-none-eabi"]
msp_pkgs = ["gcc-msp430"]
x86_pkgs = ["qemu-system-x86", "g++-multilib", "gcc-multilib",
            "build-essential"]
avr8_pkgs = ["gcc-avr", "binutils-avr", "avr-libc"]
static_tests_pkgs = ["doxygen", "cppcheck"]
all_mcu_pkgs = arm_pkgs + msp_pkgs + \
    x86_pkgs + avr8_pkgs + static_tests_pkgs


pkgs_to_install = common_pkgs
if "BUILDTEST_MCU_GROUP" in os.environ:
    mcu_group = os.environ["BUILDTEST_MCU_GROUP"]
    if mcu_group not in known_mcu_groups:
        pkgs_to_install += all_mcu_pkgs
    elif mcu_group in arm_mcu_groups:
        pkgs_to_install += arm_pkgs
    elif mcu_group in msp_mcu_groups:
        pkgs_to_install += msp_pkgs
    elif mcu_group in x86_mcu_groups:
        pkgs_to_install += x86_pkgs
    elif mcu_group in avr8_mcu_groups:
        pkgs_to_install += avr8_pkgs
    elif mcu_group in static_tests_groups:
        pkgs_to_install += static_tests_pkgs
else:
    pkgs_to_install += all_mcu_pkgs

print " ".join(pkgs_to_install)

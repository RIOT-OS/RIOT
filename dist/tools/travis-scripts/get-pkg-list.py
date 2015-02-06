#!/usr/bin/env python

import os

arm_mcu_groups = ["arm7", "cortex_m0", "cortex_m3", "cortex_m3_2", "cortex_m4"]
msp_mcu_groups = ["msp430"]
x86_mcu_groups = ["x86"]
avr8_mcu_groups = ["avr8"]
known_mcu_groups = arm_mcu_groups + msp_mcu_groups + x86_mcu_groups + avr8_mcu_groups

common_pkgs = ["pcregrep", "libpcre3", "python3", "cppcheck", "doxygen"]
arm_pkgs    = ["gcc-arm-none-eabi"]
msp_pkgs    = ["gcc-msp430"]
x86_pkgs    = ["qemu-system-x86", "g++-multilib", "gcc-multilib", \
               "build-essential"]
avr8_pkgs   = ["gcc-avr", "binutils-avr", "avr-libc"]
all_mcu_pkgs    = arm_pkgs + msp_pkgs + x86_pkgs + avr8_pkgs


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
else:
	pkgs_to_install += all_mcu_pkgs

print " ".join(pkgs_to_install)

# Copyright (c) 2020 Inria
#
# This file is subject to the terms and conditions of the GNU Lesser
# General Public License v2.1. See the file LICENSE in the top level
# directory for more details.
#

menu "STM32 G4 clock configuration"
    depends on CPU_FAM_G4

choice
bool "Clock source selection"
default USE_CLOCK_PLL

config USE_CLOCK_PLL
    bool "PLL"

config USE_CLOCK_HSE
    bool "Direct High frequency external oscillator (HSE)"
    depends on BOARD_HAS_HSE

config USE_CLOCK_HSI
    bool "Direct High frequency internal oscillator (HSI16)"

endchoice

if USE_CLOCK_PLL
config CLOCK_PLL_M
    int "M: Division factor for the main PLL input clock"
    default 6 if BOARD_HAS_HSE
    default 4 if !BOARD_HAS_HSE
    range 1 16

config CLOCK_PLL_N
    int "N: Multiply factor for the VCO"
    default 40
    range 8 127

choice
bool "R: Main PLL division factor for PLL 'R' clock (system clock)"
default PLL_R_DIV_2

config PLL_R_DIV_2
    bool "Divide by 2"

config PLL_R_DIV_4
    bool "Divide by 4"

config PLL_R_DIV_6
    bool "Divide by 6"

config PLL_R_DIV_8
    bool "Divide by 8"

endchoice

config CLOCK_PLL_R
    int
    default 2 if PLL_R_DIV_2
    default 4 if PLL_R_DIV_4
    default 6 if PLL_R_DIV_6
    default 8 if PLL_R_DIV_8
endif

choice
bool "APB1 prescaler (division factor of HCLK to produce PCLK1)"
default CLOCK_APB1_DIV_1

config CLOCK_APB1_DIV_1
    bool "Divide HCLK by 1"

config CLOCK_APB1_DIV_2
    bool "Divide HCLK by 2"

config CLOCK_APB1_DIV_4
    bool "Divide HCLK by 4"

config CLOCK_APB1_DIV_8
    bool "Divide HCLK by 8"

config CLOCK_APB1_DIV_16
    bool "Divide HCLK by 16"

endchoice

config CLOCK_APB1_DIV
    int
    default 1 if CLOCK_APB1_DIV_1
    default 2 if CLOCK_APB1_DIV_2
    default 4 if CLOCK_APB1_DIV_4
    default 8 if CLOCK_APB1_DIV_8
    default 16 if CLOCK_APB1_DIV_16

choice
bool "APB2 prescaler (division factor of HCLK to produce PCLK2)"
default CLOCK_APB2_DIV_1

config CLOCK_APB2_DIV_1
    bool "Divide HCLK by 1"

config CLOCK_APB2_DIV_2
    bool "Divide HCLK by 2"

config CLOCK_APB2_DIV_4
    bool "Divide HCLK by 4"

config CLOCK_APB2_DIV_8
    bool "Divide HCLK by 8"

config CLOCK_APB2_DIV_16
    bool "Divide HCLK by 16"

endchoice

config CLOCK_APB2_DIV
    int
    default 1 if CLOCK_APB2_DIV_1
    default 2 if CLOCK_APB2_DIV_2
    default 4 if CLOCK_APB2_DIV_4
    default 8 if CLOCK_APB2_DIV_8
    default 16 if CLOCK_APB2_DIV_16

endmenu

source "$(RIOTBOARD)/common/stm32/Kconfig"

/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_nrf51822
 * @{
 *
 * @file
 * @brief           Memory definitions for the NRF51X22XXAC
 *
 * @author          Stephane D'Alu <sdalu@sdalu.com>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x00000000, LENGTH = 256K
    ram (w!rx)  : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE cortexm_base.ld

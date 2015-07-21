/*
 * Copyright (C) 2015 Rakendra Thapa <rakendrathapa@gmail.com
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_lm4f120
 * @{
 *
 * @file		lm4f120_linkerscript.ld
 * @brief       Linker description file for LM4FXXX microcontrollers.
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 *
 * @}
 */
MEMORY
{
    rom (rx)    : ORIGIN = 0x00000000, LENGTH = 256K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE cortexm_base.ld

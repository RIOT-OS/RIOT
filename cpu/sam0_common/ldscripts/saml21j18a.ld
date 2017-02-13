/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_saml21
 * @{
 *
 * @file
 * @brief           Memory definitions for the SAML21J18A
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x00000000, LENGTH = 256K
    ram (rwx)   : ORIGIN = 0x20000000, LENGTH = 32K
}

INCLUDE cortexm_base.ld

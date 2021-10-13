/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_sam3
 * @{
 *
 * @file
 * @brief           Memory definitions for the SAM3X8E
 *
 * @author          Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

MEMORY
{
    rom (rx)    : ORIGIN = 0x00080000, LENGTH = 512K
    ram (w!rx)  : ORIGIN = 0x20070000, LENGTH = 96K
}

INCLUDE cortexm_base.ld

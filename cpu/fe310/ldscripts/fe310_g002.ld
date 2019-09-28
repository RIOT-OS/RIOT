/*
 * Copyright (C) 2017, 2019   Ken Rabold
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_fe310
 * @{
 *
 * @file
 * @brief           Memory definitions for the SiFive FE310_G002
 *
 * @author          Ken Rabold
 *
 * @}
 */

MEMORY
{
  flash (rxai!w) : ORIGIN = 0x20010000, LENGTH = 0x0006a120
  ram   (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x00004000
  itim  (wxa!ri) : ORIGIN = 0x08000000, LENGTH = 0x00002000
}

INCLUDE fe310_base.ld

/*
 * Copyright (C) 2017 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup      cpu_cortex_common
 * @{
 *
 * @file
 * @brief           Offset definitions for cortex MCUs
 *
 * @author          Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

boot_offset = DEFINED( offset ) ? offset : 0x0 ;
rom_length = DEFINED( length ) ? length : rom_length ;

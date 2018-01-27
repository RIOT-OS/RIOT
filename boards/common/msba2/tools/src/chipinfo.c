/*
 * LPC 2000 Loader, http://www.pjrc.com/arm/lpc2k_pgm
 * Copyright (c) 2004, PJRC.COM, LLC, <paul@pjrc.com>
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; version 2 of the License.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General
 * Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.
 *
 */

#include <stdio.h>
#include "chipinfo.h"
#include "boot_2xxx.h"
#include "boot_23xx.h"


struct sector_info_struct lpc2106_layout[] = {
    {0x00000000, 0x2000},
    {0x00002000, 0x2000},
    {0x00004000, 0x2000},
    {0x00006000, 0x2000},
    {0x00008000, 0x2000},
    {0x0000A000, 0x2000},
    {0x0000C000, 0x2000},
    {0x0000E000, 0x2000},
    {0x00010000, 0x2000},
    {0x00012000, 0x2000},
    {0x00014000, 0x2000},
    {0x00016000, 0x2000},
    {0x00018000, 0x2000},
    {0x0001A000, 0x2000},
    {0x0001C000, 0x2000}
};

struct sector_info_struct lpc2214_layout[] = {
    {0x00000000, 0x2000},
    {0x00002000, 0x2000},
    {0x00004000, 0x2000},
    {0x00006000, 0x2000},
    {0x00008000, 0x2000},
    {0x0000A000, 0x2000},
    {0x0000C000, 0x2000},
    {0x0000E000, 0x2000},
    {0x00010000, 0x10000},
    {0x00020000, 0x10000},
    {0x00030000, 0x2000},
    {0x00032000, 0x2000},
    {0x00034000, 0x2000},
    {0x00036000, 0x2000},
    {0x00038000, 0x2000},
    {0x0003A000, 0x2000},
    {0x0003C000, 0x2000}
};

struct sector_info_struct lpc2138_layout[] = {
    {0x00000000, 0x1000},
    {0x00001000, 0x1000},
    {0x00002000, 0x1000},
    {0x00003000, 0x1000},
    {0x00004000, 0x1000},
    {0x00005000, 0x1000},
    {0x00006000, 0x1000},
    {0x00007000, 0x1000},
    {0x00008000, 0x8000},
    {0x00010000, 0x8000},
    {0x00018000, 0x8000},
    {0x00020000, 0x8000},
    {0x00028000, 0x8000},
    {0x00030000, 0x8000},
    {0x00038000, 0x8000},
    {0x00040000, 0x8000},
    {0x00048000, 0x8000},
    {0x00050000, 0x8000},
    {0x00058000, 0x8000},
    {0x00060000, 0x8000},
    {0x00068000, 0x8000},
    {0x00070000, 0x8000},
    {0x00078000, 0x1000},
    {0x00079000, 0x1000},
    {0x0007A000, 0x1000},
    {0x0007B000, 0x1000},
    {0x0007C000, 0x1000}
};



/* chunk_size is the number of bytes that will be sent with each */
/* "C" (Copy RAM to Flash) command.  This must be one of the sizes */
/* supported by that command.  Beware that different chips support */
/* different sets of sizes, so check the user manual specific to */
/* the chip.  You must choose a chunk_size which is an an integer */
/* multiple of all the sector sizes, and it must be able to fit */
/* entirely within the RAM (allowing for the bootloader memory and */
/* stack usage).  Currently, all available chunk sizes meet these */
/* requirements, but who knows what Philips will do in the future? */
//
/* ram_addr is the location in RAM where the chunks are sent by the */
/* "W" (Write to RAM) command. */


struct chip_info_struct chip_info[] = {
    /* part_number     id_string     ram_addr    _size   sec  sector layout  boot code */
    {"LPC2104 (120k)", "4293984018", 0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2105 (120k)", "4293984034", 0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2106 (120k)", "4293984050", 0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2114 (120k)", "16908050",   0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2119 (120k)", "33685266",   0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2124 (120k)", "16908051",   0x40000200, 0x2000, 15, lpc2106_layout, boot_2xxx},
    {"LPC2129 (248k)", "33685267",   0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2131 (32k)",  "196353",     0x40000200, 0x1000,  8, lpc2138_layout, boot_2xxx},
    {"LPC2132 (64k)",  "196369",     0x40000200, 0x1000,  9, lpc2138_layout, boot_2xxx},
    {"LPC2134 (128k)", "196370",     0x40000200, 0x1000, 11, lpc2138_layout, boot_2xxx},
    {"LPC2136 (256k)", "196387",     0x40000200, 0x1000, 15, lpc2138_layout, boot_2xxx},
    {"LPC2138 (500k)", "196389",     0x40000200, 0x1000, 27, lpc2138_layout, boot_2xxx},
    {"LPC2141 (32k)",  "67305217",   0x40000200, 0x1000,  8, lpc2138_layout, boot_2xxx},
    {"LPC2142 (64k)",  "67305233",   0x40000200, 0x1000,  9, lpc2138_layout, boot_2xxx},
    {"LPC2144 (128k)", "67305234",   0x40000200, 0x1000, 11, lpc2138_layout, boot_2xxx},
    {"LPC2146 (256k)", "67305251",   0x40000200, 0x1000, 15, lpc2138_layout, boot_2xxx},
    {"LPC2148 (500k)", "67305253",   0x40000200, 0x1000, 27, lpc2138_layout, boot_2xxx},
    {"LPC2194 (248k)", "50462483",   0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2212 (248k)", "67239698",   0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2214 (248k)", "100794131",  0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2292 (248k)", "67239699",   0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2294 (248k)", "84016915",   0x40000200, 0x2000, 17, lpc2214_layout, boot_2xxx},
    {"LPC2103 (32k)",  "327441",     0x40000200, 0x1000,  8, lpc2138_layout, boot_2xxx},
    {"LPC2364 (128k)", "100924162",  0x40000200, 0x1000, 11, lpc2138_layout, boot_23xx},
    {"LPC2366 (256k)", "100924195",  0x40000200, 0x1000, 15, lpc2138_layout, boot_23xx},
    {"LPC2368 (500k)", "100924197",  0x40000200, 0x1000, 27, lpc2138_layout, boot_23xx},
    {"LPC2378 (500k)", "117702437",  0x40000200, 0x1000, 27, lpc2138_layout, boot_23xx},
    {"LPC2387 (500k)", "402716981",  0x40000200, 0x1000, 27, lpc2138_layout, boot_23xx},
    {"LPC2387 (500k)", "385941301",  0x40000200, 0x1000, 27, lpc2138_layout, boot_23xx},
    {"LPC2468 (500k)", "100925237",  0x40000200, 0x1000, 27, lpc2138_layout, boot_23xx},
    {NULL, NULL, 0, 0, 0, NULL}
};



char *lpc_return_strings[] = {
    "CMD_SUCCESS", "INVALID_COMMAND", "SRC_ADDR_ERROR", "DST_ADDR_ERROR",
    "SRC_ADDR_NOT_MAPPED", "DST_ADDR_NOT_MAPPED", "COUNT_ERROR", "INVALID_SECTOR",
    "SECTOR_NOT_BLANK", "SECTOR_NOT_PREPARED_FOR_WRITE_OPERATION", "COMPARE_ERROR",
    "BUSY", "PARAM_ERROR", "ADDR_ERROR", "ADDR_NOT_MAPPED", "CMD_LOCKED",
    "INVALID_CODE", "INVALID_BAUD_RATE", "INVALID_STOP_BIT",
    "CODE_READ_PROTECTION_ENABLED"
};

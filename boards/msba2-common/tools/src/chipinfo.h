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
 */

extern char *lpc_return_strings[];

struct sector_info_struct { /*  an array of */
    int address;        /*  where each sector is located */
    int size;       /*  and how big it is */
};

struct chip_info_struct {
    char *part_number;  /*  human readable part number */
    char *id_string;    /*  id string sent by "J" command */
    unsigned int ram_addr;  /*  where to download into RAM */
    int chunk_size;     /*  download to ram chunk size */
    int num_sector;     /*  number of flash sectors */
    struct sector_info_struct *layout;  /*  layout of sectors */
    const unsigned int *bootprog;   /*  code that boots into user program (NULL = DTR/RTS only) */
};

extern struct chip_info_struct chip_info[];

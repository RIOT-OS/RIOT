/*
 * SPDX-FileCopyrightText: 2004 PJRC.COM, LLC, <paul@pjrc.com>
 * SPDX-License-Identifier: GPL-2.0-only
 */

#ifndef CHIPINFO_H
#define CHIPINFO_H

extern char *lpc_return_strings[];

/**
 * @brief struct defining sector information
 */
struct sector_info_struct {     /*  an array of */
    int address;                /**<  where each sector is located */
    int size;                   /**<  and how big it is */
};

/**
 * @brief alksd
 */
struct chip_info_struct {
    char *part_number;                  /**<  human readable part number */
    char *id_string;                    /**<  id string sent by "J" command */
    unsigned int ram_addr;              /**<  where to download into RAM */
    int chunk_size;                     /**<  download to ram chunk size */
    int num_sector;                     /**<  number of flash sectors */
    struct sector_info_struct *layout;  /**<  layout of sectors */
    const unsigned int *bootprog;       /**<  code that boots into user program
                                              (NULL = DTR/RTS only) */
};

extern struct chip_info_struct chip_info[];

#endif /* CHIPINFO_H */

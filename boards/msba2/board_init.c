/*
 * main.c - Main function of the SRF02 ultrasonic sensor project.
 * Copyright (C) 2013 Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_msba2
 * @{
 */

/**
 * @file
 * @brief       MSB-A2 board initialization
 *
 * @author      Heiko Will
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @author      Michael Baar <baar@inf.fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @note        $Id$
 */

#include "board.h"
#include "cpu.h"
#include "mtd.h"

#ifdef MODULE_MTD_MCI
extern const mtd_desc_t mtd_mci_driver;
static mtd_dev_t _mtd_mci = { .driver = &mtd_mci_driver };
MTD_XFA_ADD(_mtd_mci, 0);
#endif

#ifdef MODULE_VFS_DEFAULT
#include "vfs_default.h"
VFS_AUTO_MOUNT(fatfs, { .dev = &_mtd_mci }, VFS_DEFAULT_SD(0), 0);
#endif

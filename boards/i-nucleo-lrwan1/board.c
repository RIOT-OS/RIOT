/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_i-nucleo-lrwan1
 * @{
 *
 * @file
 * @brief       Board specific initialization for I-Nucleo-LRWAN1
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#include "board.h"
#include "mtd_eeprom.h"

#if MODULE_MTD_EEPROM
static mtd_eeprom_t eeprom_mtd = MTD_EEPROM_INIT_VAL;
MTD_XFA_ADD(eeprom_mtd, 0);
#endif /* MODULE_MTD_EEPROM */

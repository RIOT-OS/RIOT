/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     boards_common_atmega
 * @{
 *
 * @file
 * @brief       Definitions for all atmega boards.
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 * @}
 */

#include "board.h"
#include "mtd_eeprom.h"

#if MODULE_MTD_EEPROM
mtd_eeprom_t mtd_eeprom = MTD_EEPROM_INIT_VAL;
MTD_XFA_ADD(mtd_eeprom, 0);
#endif /* MODULE_MTD_EEPROM */

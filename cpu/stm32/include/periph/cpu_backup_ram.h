/*
 * Copyright (C) 2022 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Backup SRAM CPU specific definitions for the STM32 family
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef PERIPH_CPU_BACKUP_RAM_H
#define PERIPH_CPU_BACKUP_RAM_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Backup RAM must be initialized with @ref backup_ram_init on reset
 */
#define BACKUP_RAM_HAS_INIT         1

/**
 * @brief   Enable backup RAM access
 */
void backup_ram_init(void);

/**
 * @brief   Returns true if the CPU woke up from deep sleep (backup/standby)
 */
bool cpu_woke_from_backup(void);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* PERIPH_CPU_BACKUP_RAM_H */

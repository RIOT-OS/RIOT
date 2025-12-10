/*
 * SPDX-FileCopyrightText: 2022 Otto-von-Guericke-Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup         cpu_stm32
 * @{
 *
 * @file
 * @brief           Backup SRAM CPU specific definitions for the STM32 family
 *
 * @author          Fabian Hüßler <fabian.huessler@ovgu.de>
 */

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

/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_store_eeprom EEPROM storage facility
 * @ingroup     sys_registry_store
 * @brief       EEPROM based storage facility for the
 *              @ref sys_registry "RIOT Registry"
 *
 * @{
 * @file
 * @brief   Definitions for the EEPROM storage facility
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef REGISTRY_STORE_REGISTRY_STORE_EEPROM_H
#define REGISTRY_STORE_REGISTRY_STORE_EEPROM_H

#ifdef __cplusplus
extern "C" {
#endif

#include "registry/registry.h"

/**
 * @brief EEPROM storage facility descriptor.
 */
typedef struct {
    registry_store_t store; /**< base store descriptor */
} registry_eeprom_t;

/**
 * @brief Registers an EEPROM storage facility as a source of configurations on
 *        the @ref sys_registry "RIOT Registry".
 * 
 * @param[in] eeprom Pointer to the descriptor of the EEPROM storage
 * @return 0 on success, non-zero on failure
 */
int registry_eeprom_src(registry_eeprom_t *eeprom);

/**
 * @brief Registers an EEPROM storage facility as the destination of
 *        configurations on the @ref sys_registry "RIOT Registry".
 * 
 * @param[in] eeprom Pointer to the descriptor of the EEPROM storage
 * @return 0 on success, non-zero on failure
 */
int registry_eeprom_dst(registry_eeprom_t *eeprom);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_STORE_REGISTRY_STORE_EEPROM_H */

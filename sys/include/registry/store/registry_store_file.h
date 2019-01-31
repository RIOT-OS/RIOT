/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_registry_store_file File storage facility
 * @ingroup     sys_registry_store
 * @brief       File based storage facility for the
 *              @ref sys_registry "RIOT Registry"
 *
 * @{
 * @file
 * @brief   Definitions for the File storage facility
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */

#ifndef REGISTRY_STORE_REGISTRY_STORE_FILE_H
#define REGISTRY_STORE_REGISTRY_STORE_FILE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "registry/registry.h"

/**
 * @brief Extra length introduced by the line format
 */
#define REGISTRY_EXTRA_LEN         3 // '=' '\0\n'

/**
 * @brief File storage facility descriptor.
 *
 * @note A file name must be defined in order to use this storage facility.
 */
typedef struct {
    registry_store_t store; /**< base store descriptor */
    const char *file_name;  /**< name of the file to store configurations */
} registry_file_t;

/**
 * @brief Registers a file storage facility as a source of configurations on the
 *        @ref sys_registry "RIOT Registry".
 *
 * @param[in] file  Pointer to the descriptor of the file storage
 * @return 0 on success, non-zero on failure
 */
int registry_file_src(registry_file_t *file);

/**
 * @brief Registers a file storage facility as the destination of configurations
 *        on the @ref sys_registry "RIOT Registry".
 *
 * @param[in] file  Pointer to the descriptor of the file storage
 * @return 0 on success, non-zero on failure
 */
int registry_file_dst(registry_file_t *file);
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* REGISTRY_STORE_REGISTRY_STORE_FILE_H */

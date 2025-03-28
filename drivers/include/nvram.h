/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

#ifndef NVRAM_H
#define NVRAM_H

/**
 * @defgroup    drivers_nvram Non-volatile RAM
 * @ingroup     drivers_storage
 * @brief       Non-volatile RAM interface
 *
 * This API is designed around non-volatile memories which do not need blockwise
 * erase, such as ferro-electric RAM (FRAM) or magneto-resistive RAM (MRAM).
 *
 * This interface is not suitable for flash memories.
 *
 * @{
 *
 * @file
 *
 * @brief       Generic non-volatile RAM driver interface
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#include <stdint.h>
#include <stddef.h>

#if MODULE_VFS
#include "vfs.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/* Forward declaration in order to declare function pointers which take this
 * type as a parameter within the struct. */
struct nvram;

/**
 * @brief   Device descriptor for generic NVRAM devices.
 */
typedef struct nvram {
    /**
     * @brief   Pointer to device-specific read function
     *
     * Copy data from system memory to NVRAM.
     *
     * @param[in]  dev   Pointer to NVRAM device descriptor
     * @param[out] dst   Pointer to the first byte in the system memory address space
     * @param[in]  src   Starting address in the NVRAM device address space
     * @param[in]  len   Number of bytes to copy
     *
     * @return           Number of bytes read on success
     * @return           <0 on errors
     */
    int (*read)(struct nvram *dev, uint8_t *dst, uint32_t src, size_t size);

    /**
     * @brief   Pointer to device-specific write function
     *
     * Copy data from NVRAM to system memory.
     *
     * @param[in]  dev   Pointer to NVRAM device descriptor
     * @param[in]  src   Pointer to the first byte in the system memory address space
     * @param[in]  dst   Starting address in the NVRAM device address space
     * @param[in]  len   Number of bytes to copy
     *
     * @return           Number of bytes written on success
     * @return           <0 on errors
     */
    int (*write)(struct nvram *dev, const uint8_t *src, uint32_t dst, size_t size);

    /** @brief   Device capacity */
    size_t size;

    /** @brief   Device-specific parameters, if any. */
    void *extra;
} nvram_t;

#if MODULE_VFS
extern const vfs_file_ops_t nvram_vfs_ops;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* NVRAM_H */

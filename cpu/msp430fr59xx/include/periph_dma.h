/*
 * Copyright (C) 2018 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_dma DMA
 * @ingroup     drivers_periph
 * @brief       Low-level direct memory access driver
 * @{
 *
 * @file
 * @brief       Low-level direct memory access driver interface definitions
 *
 * @author      Tristan Delizy <tristan.delizy@inria.fr>
 * @author      Daniel Krebs <github@daniel-krebs.net>
 * @author      Loïc Saos <loic.saos@insa-lyon.fr>
 */

#ifndef PERIPH_DMA_H
#define PERIPH_DMA_H

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fill the first 'len' bytes of 'dst' with the constant byte 'val'
 *
 * The CPU is halted during the DMA transfer.
 *
 * @return                  'dst' on success
 * @return                  NULL  on failure (wrong size, alignment)
 */
void *dma_memset(void *dst, unsigned char val, size_t len);

/**
 * @brief Copy 'len' bytes from 'src' to 'dst' via DMA
 *
 * The memory areas must not overlap. The CPU is halted during the DMA transfer.
 *
 * @return                  'dst' on success
 * @return                  NULL  on failure (wrong size, alignment)
 */
void *dma_memcpy(void *dst, const void *src, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_DMA_H */
/** @} */

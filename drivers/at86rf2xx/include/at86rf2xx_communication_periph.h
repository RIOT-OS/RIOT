/*
 * Copyright (C) 2019 Benjamin Valentin
 *               2019 OvGU Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_at86rf2xx
 * @{
 *
 * @file
 * @brief       AT86RFXX peripheral communication interface
 *
 * @author      Benjamin Valentin <benpicco@googlemail.com>
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF2XX_COMMUNICATION_PERIPH_H
#define AT86RF2XX_COMMUNICATION_PERIPH_H

#include <string.h>

#include "at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Functions to communicate with peripheral transceivers
 * @{
 */
static inline uint8_t
at86rf2xx_periph_reg_read(volatile uint8_t *addr)
{
    return *addr;
}

static inline void
at86rf2xx_periph_reg_write(volatile uint8_t *addr, const uint8_t value)
{
    *addr = value;
}

static inline void
at86rf2xx_periph_sram_read(volatile uint8_t *sram_addr, uint8_t offset,
                           void *data, size_t len)
{
    memcpy(data, (void *)(sram_addr + offset), len);
}

static inline void
at86rf2xx_periph_sram_write(volatile uint8_t *sram_addr, uint8_t offset,
                            const void *data, size_t len)
{
    memcpy((void *)(sram_addr + offset), data, len);
}

static inline void
at86rf2xx_periph_fb_read(void *data, volatile uint8_t *fb_addr, size_t len)
{
    memcpy(data, (void *)fb_addr, len);
}
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* AT86RF2XX_COMMUNICATION_PERIPH_H */

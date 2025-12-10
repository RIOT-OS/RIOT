/*
 * SPDX-FileCopyrightText: 2016 TriaGnoSys GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     PN532 board configuration example
 *
 * @author    Víctor Ariño <victor.arino@triagnosys.com>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"

static const pn532_params_t pn532_conf[] = {
    {
#if IS_USED(MODULE_PN532_I2C)
        .i2c = TEST_PN532_I2C,
#endif
#if IS_USED(MODULE_PN532_SPI)
        .spi = TEST_PN532_SPI,
#endif
        .reset = TEST_PN532_RESET,
        .irq = TEST_PN532_IRQ,
#if IS_USED(MODULE_PN532_SPI)
        .nss   = TEST_PN532_NSS
#endif
    },
};

#ifdef __cplusplus
}
#endif

/** @} */

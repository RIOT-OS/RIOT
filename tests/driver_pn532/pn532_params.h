/*
 * Copyright (C) 2016 TriaGnoSys GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   tests
 * @{
 *
 * @file
 * @brief     PN532 board configuration example
 *
 * @author    Víctor Ariño <victor.arino@triagnosys.com>
 */

#ifndef PN532_PARAMS_H
#define PN532_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

static const pn532_params_t pn532_conf[] = {
    {
#if defined(PN532_SUPPORT_I2C)
        .i2c = TEST_PN532_I2C,
#elif defined(PN532_SUPPORT_SPI)
        .spi = TEST_PN532_SPI,
#endif
        .reset = TEST_PN532_RESET,
        .irq = TEST_PN532_IRQ,
#if defined(PN532_SUPPORT_SPI)
        .nss   = TEST_PN532_NSS
#endif
    },
};

#ifdef __cplusplus
}
#endif

#endif /* PN532_PARAMS_H */
/** @} */

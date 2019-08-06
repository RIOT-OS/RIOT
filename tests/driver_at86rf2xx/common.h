/*
 * Copyright (C) 2016 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief   Common header for at86rf2xx tests
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef COMMON_H
#define COMMON_H

#include <stdint.h>

#include "at86rf2xx.h"
#include "at86rf2xx_params.h"
#include "net/netdev.h"
#include "kernel_defines.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Application-internal functions and variables for at86rf2xx tests
 * @internal
 * @{
 */
#define AT86RF2XX_NUM   ARRAY_SIZE(at86rf2xx_params)

extern at86rf2xx_t devs[AT86RF2XX_NUM];

void recv(netdev_t *dev);
int ifconfig(int argc, char **argv);
int txtsnd(int argc, char **argv);
void print_addr(uint8_t *addr, size_t addr_len);
/**
 * @}
 */

#ifdef __cplusplus
}
#endif

#endif /* COMMON_H */
/** @} */

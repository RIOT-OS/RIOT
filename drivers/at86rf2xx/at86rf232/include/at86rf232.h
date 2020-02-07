/*
 * Copyright (C) 2019 OvGU Magdeburg
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
 * @brief       Public interface of AT86RF232
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF232_H
#define AT86RF232_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct at86rf232;
typedef struct at86rf232 at86rf232_t;
struct at86rf232_params;
typedef struct at86rf232_params at86rf232_params_t;

/**
 * @brief   Setup @p num transceiver devices of type AT86RF233
 *
 * @param[out]   devs       array of AT86RF232 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf232_setup(at86rf232_t *devs, const at86rf232_params_t *params,
                     uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF232_H */

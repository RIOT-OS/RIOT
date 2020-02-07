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
 * @brief       AT86RF233 public interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RF233_H
#define AT86RF233_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

struct at86rf233;
typedef struct at86rf233 at86rf233_t;
struct at86rf233_params;
typedef struct at86rf233_params at86rf233_params_t;

/**
 * @brief   Setup @p num transceiver devices of type AT86RF233
 *
 * @param[out]   devs       array of AT86RF233 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rf233_setup(at86rf233_t *devs, const at86rf233_params_t *params,
                     uint8_t num);

#ifdef __cplusplus
}
#endif

#endif /* AT86RF233_H */

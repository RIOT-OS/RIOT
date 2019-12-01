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
 * @brief       AT86RFR2 public interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RFR2_H
#define AT86RFR2_H

#ifdef __cplusplus
extern "C" {
#endif

struct at86rfr2;
typedef struct at86rfr2 at86rfr2_t;

/**
 * @brief   Setup @p num transceiver devices of type AT86RFA1
 *
 * @param[out]   dev        array of AT86RFA1 device handles
 * @param[in]    params     parameters
 * @param[in]    num        number of transceivers
 */
void at86rfr2_setup(at86rfr2_t *devs);

#ifdef __cplusplus
}
#endif

#endif /* AT86RFR2_H */

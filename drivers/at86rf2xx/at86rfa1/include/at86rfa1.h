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
 * @brief       AT86RFA1 public interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 */

#ifndef AT86RFA1_H
#define AT86RFA1_H

#ifdef __cplusplus
extern "C" {
#endif

struct at86rfa1;
typedef struct at86rfa1 at86rfa1_t;

/**
 * @brief   Setup @p num transceiver devices of type AT86RFA1
 *
 * @param[out]   dev        AT86RFA1 device handle
 */
void at86rfa1_setup(at86rfa1_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* AT86RFA1_H */

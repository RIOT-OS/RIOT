/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    emb6    emb6 network stack
 * @ingroup pkg
 * @brief
 * @{
 *
 * @file
 * @brief   "Board" configuration for emb6
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#ifndef BOARD_CONF_H
#define BOARD_CONF_H

#ifdef __cplusplus
extern "C" {
#endif

#include "emb6.h"

/**
 * @brief    emb6 board configuration function
 *
 * @param[in]   ps_nStack pointer to global netstack struct
 *
 * @return  success 1
 * @return  failure 0
 */
uint8_t board_conf(s_ns_t *ps_nStack);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_CONF_H */
/** @} */
/** @} */

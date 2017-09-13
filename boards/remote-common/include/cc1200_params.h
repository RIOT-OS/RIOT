/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   board_remote_revb
 * @{
 *
 * @file
 * @brief     cc1200 board specific configuration
 *
 * @author    Anon Mall <anon.mall@mail.gt-arc.com>
 */

#ifndef CC1200_PARAMS_H
#define CC1200_PARAMS_H

#ifdef __cplusplus
extern "C" {
#endif

#include "board.h"

/**
 * @name CC1200 configuration
 */
const cc1200_params_t cc1200_params[] = {
    {
        .spi  = 0,
        .cs   = CC1200_CSN_GPIO,
        .gdo0 = CC1200_GPD0_GPIO,
        .gdo1 = CC1200_MISO_GPIO,
        .gdo2 = CC1200_GPD2_GPIO
    },
};
/** @} */

#ifdef __cplusplus
}
#endif
#endif /* CC1200_PARAMS_H */
/** @} */

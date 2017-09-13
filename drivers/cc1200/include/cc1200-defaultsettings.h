/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup   driver_cc1200
 * @{
 *
 * @file
 * @brief     cc1200 default settings override
 *
 * By setting either CC1200_DEFAULT_PATABLE or CC1200_DEFAULT_FREQ in board.h,
 * it is possible to override the default pa table or base frequency registers
 * on a per-device basis.
 *
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */
#ifndef CC1200_DEFAULTSETTINGS_H
#define CC1200_DEFAULTSETTINGS_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef CC1200_DEFAULT_PATABLE
#define CC1200_DEFAULT_PATABLE cc1200_default_pa_table
extern const char cc1200_default_pa_table[8];
#endif

#ifndef CC1200_DEFAULT_FREQ
#define CC1200_DEFAULT_FREQ cc1200_default_base_freq
extern const char cc1200_default_base_freq[3];
#endif


#define CC1200_PANID_DEFAULT     (IEEE802154_DEFAULT_PANID)



#ifdef __cplusplus
}
#endif

#endif /* CC1200_DEFAULTSETTINGS_H */
/** @} */

/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/*
 * @ingroup     sys_auto_init_multimedia
 * @{
 *
 * @file
 * @brief       Auto initialization for DFPlayer Mini MP3 player
 *
 * @author      Marian Buschsieweke <marian.buschsiewke@ovgu.de>
 *
 * @}
 */

#ifdef MODULE_DFPLAYER

#include "log.h"
#include "dfplayer.h"
#include "dfplayer_params.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/**
 * @brief   Allocate memory for the device descriptors
 */
dfplayer_t dfplayer_devs[DFPLAYER_NUMOF];

void auto_init_dfplayer(void)
{
    DEBUG("[dfplayer] Auto init\n");
    for (unsigned i = 0; i < DFPLAYER_NUMOF; i++) {
        if (dfplayer_init(&dfplayer_devs[i], &dfplayer_params[i])) {
            LOG_ERROR("[auto_init_multimedia] error initializing dfplayer #%u\n", i);
            continue;
        }
    }
}

#else
typedef int dont_be_pedantic;
#endif /* MODULE_DFPLAYER */

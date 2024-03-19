/*
 * Copyright (C) 2022 SSV Software Systems GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_efm32
 * @ingroup     drivers_periph_hwrng
 * @{
 *
 * @file
 * @brief       Hardware random generator implementation.
 *
 * @author      Juergen Fitschen <me@jue.yt>
 * @}
 */

#include <assert.h>

#include "cpu.h"

#include "periph_conf.h"
#include "periph/hwrng.h"
#include "mutex.h"

#include "em_cmu.h"
#include "em_se.h"

#include <string.h>

void hwrng_init(void)
{
    /* enable clock */
#if (_SILICON_LABS_32B_SERIES_2_CONFIG > 2)
    CMU_ClockEnable(cmuClock_SEMAILBOX, true);
#endif
}

static void _get_random(uint8_t *buf, unsigned int num)
{
    assert(num % 4 == 0);

    static mutex_t mtx = MUTEX_INIT;

    SE_Command_t cmd = SE_COMMAND_DEFAULT(SE_COMMAND_TRNG_GET_RANDOM);
    SE_DataTransfer_t data_out = SE_DATATRANSFER_DEFAULT(buf, num);
    SE_Response_t cmd_rsp;

    /* prepare command */
    SE_addDataOutput(&cmd, &data_out);
    SE_addParameter(&cmd, num);

    /* exec command */
    mutex_lock(&mtx);
    SE_executeCommand(&cmd);
    SE_waitCommandCompletion();
    cmd_rsp = SE_readCommandResponse();
    mutex_unlock(&mtx);

    (void)cmd_rsp;
    assert(cmd_rsp == SE_RESPONSE_OK);
}

void hwrng_read(void *buf, unsigned int num)
{
    uint8_t *data = buf;
    unsigned int num_extra = num & 0x3U;
    num &= ~0x3U;

    if (num > 0) {
        _get_random(&data[0], num);
    }

    if (num_extra > 0) {
        uint8_t tmp[4];
        _get_random(tmp, sizeof(tmp));
        memcpy(&data[num], &tmp, num_extra);
    }
}

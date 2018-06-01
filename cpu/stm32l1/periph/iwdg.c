/*
 * Copyright (C) 2016 Unwired Devices [info@unwds.com]
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @ingroup
 * @brief
 * @{
 * @file		iwdg.com
 * @brief       STM32L1 Independent Watchdog
 * @author      EP [ep@unwds.com]
 */

#include <stdbool.h>

#include "assert.h"

#include "stm32l1xx.h"
#include "periph/wdg.h"

#ifdef __cplusplus
extern "C" {
#endif

#define IWDG_KR_KEY_RELOAD      ((uint16_t)0xAAAA)
#define IWDG_KR_KEY_ENABLE      ((uint16_t)0xCCCC)

#define IWDG_UNLOCK             ((uint16_t)0x5555)
#define IWDG_LOCK               ((uint16_t)0x0000)

static inline void iwdg_unlock(void)
{
    IWDG->KR = IWDG_UNLOCK;
}

static inline void iwdg_lock(void)
{
    IWDG->KR = IWDG_LOCK;
}

void wdg_set_prescaler(uint8_t prescaler)
{
    /* Check valid prescaler values */
    assert(prescaler == 0x00    ||
           prescaler == 0x01   ||
           prescaler == 0x02   ||
           prescaler == 0x03   ||
           prescaler == 0x04   ||
           prescaler == 0x05   ||
           prescaler == 0x06);

    /* Unlock IWDG and write new prescaler value */
    iwdg_unlock();
    IWDG->PR = prescaler;
    iwdg_lock();
}

void wdg_set_reload(uint16_t reload)
{
    /* Check reload value limit */
    assert(reload <= IWDG_RLR_RL);

    /* Unlock IWDG and write new reload value */
    iwdg_unlock();
    IWDG->RLR = reload;
    iwdg_lock();
}

void wdg_enable(void)
{
    /* Enable IWDG */
    IWDG->KR = IWDG_KR_KEY_ENABLE;
}

void wdg_reload(void)
{
    IWDG->KR = IWDG_KR_KEY_RELOAD;
}

bool wdg_reset_occurred(void)
{
    return RCC->CSR & RCC_CSR_IWDGRSTF;
}

uint32_t wdg_get_value(void)
{
    return IWDG->RLR;
}

#ifdef __cplusplus
}
#endif

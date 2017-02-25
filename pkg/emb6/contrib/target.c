/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <inttypes.h>

#include "cpu.h"
#include "led.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/hwrng.h"
#include "xtimer.h"

#include "target.h"
#include "bsp.h"

static mutex_t critical_mutex = MUTEX_INIT;

void hal_enterCritical(void)
{
    mutex_lock(&critical_mutex);
}

void hal_exitCritical(void)
{
    mutex_unlock(&critical_mutex);
}

int8_t hal_init(void)
{
    /* Should have happened long before emb6 started, so nothing to do */
    return 1;
}

uint8_t hal_getrand(void)
{
#if RANDOM_NUMOF
    uint8_t res;
    hwnrg_read((char *)&res, sizeof(res));
    return res;
#elif defined(MODULE_RANDOM)
    return (uint8_t)(genrand_uint32() % UINT8_MAX);
#else
    return 4;   /* keeping the meme alive ;-) */
#endif
}

void hal_ledOn(uint16_t ui_led)
{
    switch (ui_led) {
        case E_BSP_LED_RED:
            LED0_ON;
            break;
        case E_BSP_LED_YELLOW:
            LED1_ON;
            break;
        case E_BSP_LED_GREEN:
            LED2_ON;
            break;
        default:
            break;
    }
}

void hal_ledOff(uint16_t ui_led)
{
    switch (ui_led) {
        case E_BSP_LED_RED:
            LED0_OFF;
            break;
        case E_BSP_LED_YELLOW:
            LED1_OFF;
            break;
        case E_BSP_LED_GREEN:
            LED2_OFF;
            break;
        default:
            break;
    }
}

uint8_t hal_extIntInit(en_targetExtInt_t e_extInt, pfn_intCallb_t pfn_intCallback)
{
    /* RIOT does this in netdev2 initialization so nothing to do here. */
    return 0;
}

void hal_delay_us(uint32_t i_delay)
{
    xtimer_usleep(i_delay);
}

uint8_t hal_gpioPinInit(uint8_t c_pin, uint8_t c_dir, uint8_t c_initState)
{
    /* Only used in board init code => not needed */
    (void)c_pin;
    (void)c_dir;
    (void)c_initState;
    return 0;
}

void *hal_ctrlPinInit(en_targetExtPin_t e_pinType)
{
    (void)e_pinType;
    return NULL;
}

void hal_pinSet(void *p_pin)
{
    /* Only used in board/driver-related code code => not needed */
}

void hal_pinClr(void *p_pin)
{
    /* Only used in board/driver-related code code => not needed */
}

uint8_t hal_pinGet(void *p_pin)
{
    /* Only used in board/driver-related code code => not needed */
    return 0;
}

void *hal_spiInit(void)
{
    /* Only used in board/driver-related code code => not needed */
    return 0;
}

/*----------------------------------------------------------------------------*/
/** \brief  This function selects slave with which we will work
 *  \param         p_spi    Pointer to spi description entity
 *  \param        action    true or false
 *
 *  \return        0 if failed, 1 id ok
 */
/*---------------------------------------------------------------------------*/
uint8_t hal_spiSlaveSel(void *p_spi, bool action)
{
    /* Only used in board/driver-related code code => not needed */
    return 0;
}

uint8_t hal_spiTransceive( uint8_t *txData, uint8_t *p_reg)
{
    /* Only used in board/driver-related code code => not needed */
    return 0;
}

uint8_t hal_spiRead(uint8_t *p_reg, uint16_t i_length)
{
    /* Only used in board/driver-related code code => not needed */
    return 0;
}


/*----------------------------------------------------------------------------*/
/** \brief  This function writes a new value via given SPI interface
 *          registers.
 *
 *
 *  \param  value         Pointer to a value.
 *  \param  i_length     Length of a data to be received
 */
/*----------------------------------------------------------------------------*/
void hal_spiWrite(uint8_t *value, uint16_t i_length)
{
    /* Only used in board/driver-related code code => not needed */
}

void hal_watchdogReset(void)
{
    /* WDT and tick-less scheduling don't make much sense */
}

void hal_watchdogStart(void)
{
    /* WDT and tick-less scheduling don't make much sense */
}

void     hal_watchdogStop(void)
{
    /* WDT and tick-less scheduling don't make much sense */
}

clock_time_t hal_getTick(void)
{
    return (clock_time_t)xtimer_now_usec();
}

clock_time_t hal_getSec(void)
{
    return (clock_time_t)xtimer_now_usec() / US_PER_SEC;
}


clock_time_t hal_getTRes(void)
{
    return US_PER_SEC;
}

/** @} */

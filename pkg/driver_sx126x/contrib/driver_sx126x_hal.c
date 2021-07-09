/*
 * Copyright (C) 2021 Inria
 *               2021 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_sx126x
 * @{
 *
 * @file
 * @brief       HAL implementation for the SX1261/2 LoRa radio driver
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Akshai M <akshai.m@fu-berlin.de>
 *
 * @}
 */

#include "ztimer.h"

#include "periph/gpio.h"
#include "periph/spi.h"

#include "sx126x.h"
#include "sx126x_hal.h"
#include "sx126x_internal.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SX126X_SPI_SPEED    (SPI_CLK_1MHZ)
#define SX126X_SPI_MODE     (SPI_MODE_0)

#ifndef SX126X_PERIPH_WAKEUP_TIME
#define SX126X_PERIPH_WAKEUP_TIME   10000U
#endif

#ifndef SX126X_SPI_WAKEUP_TIME
#define SX126X_SPI_WAKEUP_TIME      500U
#endif

#if IS_USED(MODULE_SX126X_STM32WL)
static uint8_t sx126x_radio_wait_until_ready(sx126x_t *dev)
{
    if (dev->radio_sleep == true) {
        DEBUG("[sx126x_radio] : Wakeup radio \n");
        sx126x_hal_wakeup(dev);
    }

    /* Wait until Busy/ BusyMS signal goes low */
    while (((PWR->SR2 & PWR_SR2_RFBUSYMS) && ((PWR->SR2 & PWR_SR2_RFBUSYS))) == 1) {}
    return 0;
}
#endif

sx126x_hal_status_t sx126x_hal_write(const void *context,
                                     const uint8_t *command, const uint16_t command_length,
                                     const uint8_t *data, const uint16_t data_length)
{
    (void)data;
    (void)data_length;
    sx126x_t *dev = (sx126x_t *)context;

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        sx126x_radio_wait_until_ready(dev);
        spi_acquire(dev->params->spi, SPI_CS_UNDEF, SX126X_SPI_MODE, SX126X_SPI_SPEED);

        /* Check if radio is set to sleep or `RxDutyCycle` mode */
        if (command[0] == 0x84 || command[0] == 0x94) {
            dev->radio_sleep = true;
        }
        else {
            dev->radio_sleep = false;
        }

        /* Pull NSS low */
        PWR->SUBGHZSPICR &= ~PWR_SUBGHZSPICR_NSS;
        spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, data_length != 0, command,
                           NULL, command_length);
        if (data_length) {
            spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, data, NULL,
                               data_length);
        }

        /* Pull NSS high */
        PWR->SUBGHZSPICR |= PWR_SUBGHZSPICR_NSS;
#endif
    }
    else {
        /* wait for the device to not be busy anymore */
        while (gpio_read(dev->params->busy_pin)) {}

        spi_acquire(dev->params->spi, SPI_CS_UNDEF, SX126X_SPI_MODE, SX126X_SPI_SPEED);
        spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, data_length != 0,
                           command, NULL, command_length);
        if (data_length) {
            spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, data,
                               NULL, data_length);
        }
    }

    spi_release(dev->params->spi);
    return 0;
}

sx126x_hal_status_t sx126x_hal_read(const void *context,
                                    const uint8_t *command, const uint16_t command_length,
                                    uint8_t *data, const uint16_t data_length)
{
    sx126x_t *dev = (sx126x_t *)context;

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        sx126x_radio_wait_until_ready(dev);

        spi_acquire(dev->params->spi, SPI_CS_UNDEF, SX126X_SPI_MODE, SX126X_SPI_SPEED);
        /* Pull NSS low */
        PWR->SUBGHZSPICR &= ~PWR_SUBGHZSPICR_NSS;
        spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, true, command, NULL, \
                           command_length);
        spi_transfer_bytes(dev->params->spi, SPI_CS_UNDEF, false, NULL, data, \
                           data_length);
        /* Pull NSS high */
        PWR->SUBGHZSPICR |= PWR_SUBGHZSPICR_NSS;
#endif
    }
    else {
        /* wait for the device to not be busy anymore */
        while (gpio_read(dev->params->busy_pin)) {}

        spi_acquire(dev->params->spi, SPI_CS_UNDEF, SX126X_SPI_MODE, SX126X_SPI_SPEED);
        spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, true, \
                           command, NULL, command_length);
        spi_transfer_bytes(dev->params->spi, dev->params->nss_pin, false, \
                           NULL, data, data_length);
    }

    spi_release(dev->params->spi);
    return 0;
}

sx126x_hal_status_t sx126x_hal_reset(const void *context)
{
    sx126x_t *dev = (sx126x_t *)context;

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        /* Reset Radio */
        RCC->CSR |= RCC_CSR_RFRST;
        /* Clear radio reset */
        RCC->CSR &= ~RCC_CSR_RFRST;
        ztimer_sleep(ZTIMER_USEC, 100);
        /* Wait while reset is done */
        while ((RCC->CSR & RCC_CSR_RFRSTF) != 0UL) {}

        /* Asserts the reset signal of the Radio peripheral */
        PWR->SUBGHZSPICR |= PWR_SUBGHZSPICR_NSS;

        /* Enable EXTI 44 : Radio IRQ ITs for CPU1 */
        EXTI->IMR2 |= EXTI_IMR2_IM44;

        /* Set NVIC Priority and enable NVIC */
        NVIC_SetPriority(SUBGHZ_Radio_IRQn, 0);
        NVIC_EnableIRQ(SUBGHZ_Radio_IRQn);

        /* Enable wakeup signal of the SUBGHZ Radio */
        PWR->CR3 |= PWR_CR3_EWRFBUSY;
        /* Clear Pending Flag */
        PWR->SCR = PWR_SCR_CWRFBUSYF;

        dev->radio_sleep = true;
#endif
    }
    else {
        gpio_set(dev->params->reset_pin);
        gpio_clear(dev->params->reset_pin);

        /* it takes 100us for the radio to be ready after reset */
        ztimer_sleep(ZTIMER_USEC, 100);
        gpio_set(dev->params->reset_pin);
    }

    return 0;
}

sx126x_hal_status_t sx126x_hal_wakeup(const void *context)
{
    DEBUG("[sx126x_hal] wakeup\n");
    sx126x_t *dev = (sx126x_t *)context;

    if (sx126x_is_stm32wl(dev)) {
#if IS_USED(MODULE_SX126X_STM32WL)
        /* Pull NSS low */
        PWR->SUBGHZSPICR &= ~PWR_SUBGHZSPICR_NSS;
        ztimer_sleep(ZTIMER_USEC, 1000);
        /* Pull NSS high */
        PWR->SUBGHZSPICR |= PWR_SUBGHZSPICR_NSS;
        ztimer_sleep(ZTIMER_USEC, SX126X_PERIPH_WAKEUP_TIME);
#endif
    }
    else {
        spi_acquire(dev->params->spi, SPI_CS_UNDEF, SX126X_SPI_MODE, SX126X_SPI_SPEED);
        gpio_clear(dev->params->nss_pin);
        gpio_set(dev->params->nss_pin);
        spi_release(dev->params->spi);
        ztimer_sleep(ZTIMER_USEC, SX126X_SPI_WAKEUP_TIME);
    }

    return 0;
}

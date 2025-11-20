/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_stm32
 * @{
 *
 * @file
 * @brief       Low-level SDIO/SD/MMC peripheral driver interface for STM32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <errno.h>
#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "assert.h"
#include "bitarithm.h"
#include "container.h"
#include "log.h"
#include "periph/gpio.h"
#include "ztimer.h"

#include "sdmmc/sdmmc.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* millisecond timer definitions dependent on active ztimer backend */
#if IS_USED(MODULE_ZTIMER_MSEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_MSEC, n)
#elif IS_USED(MODULE_ZTIMER_USEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_USEC, n * US_PER_MS)
#else
#error "Either ztimer_msec or ztimer_usec is needed"
#endif

#define SDMMC_IRQ_PRIO  (1)

/* command related static interrupts (same as MASK and ICR bits) */
#define SDMMC_INT_STATIC_CMD (SDMMC_STA_CMDSENT | SDMMC_STA_CMDREND | \
                              SDMMC_STA_CCRCFAIL | SDMMC_STA_CTIMEOUT)

/* data transfer related static interrupts (same as MASK and ICR bits) */
#define SDMMC_INT_STATIC_DATA (SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | \
                               SDMMC_STA_DATAEND  | SDMMC_STA_DBCKEND | \
                               SDMMC_STA_TXUNDERR | SDMMC_STA_RXOVERR | \
                               SDMMC_STA_SDIOIT | SDMMC_STA_STBITERR)

/* all static interrupts */
#define SDMMC_INT_STATIC (SDMMC_INT_STATIC_CMD | SDMMC_INT_STATIC_DATA)

/* all static error interrupts (same as MASK and ICR bits) */
#define SDMMC_INT_STATIC_ERROR  (SDMMC_STA_CCRCFAIL | SDMMC_STA_CTIMEOUT \
                                 SDMMC_STA_DCRCFAIL | SDMMC_STA_DTIMEOUT | \
                                 SDMMC_STA_TXUNDERR | SDMMC_STA_RXOVERR)

#define SDMMC_CMD_WAITRESP_SHORT    (SDMMC_CMD_WAITRESP_0)
#define SDMMC_CMD_WAITRESP_LONG     (SDMMC_CMD_WAITRESP_1 | SDMMC_CMD_WAITRESP_0)

#define SDMMC_CLKCR_CLKDIV_400KHZ   (MHZ(48) / KHZ(400) - 2) /* SD clock 400 kHz */
#define SDMMC_CLKCR_CLKDIV_1MHZ     (MHZ(48) / MHZ(1) - 2)   /* SD clock 4 MHz (debug) */
#define SDMMC_CLKCR_CLKDIV_4MHZ     (MHZ(48) / MHZ(4) - 2)   /* SD clock 4 MHz (debug) */
#define SDMMC_CLKCR_CLKDIV_8MHZ     (MHZ(48) / MHZ(8) - 2)   /* SD clock 8 MHz (debug) */
#define SDMMC_CLKCR_CLKDIV_16MHZ    (MHZ(48) / MHZ(16) - 2)  /* SD clock 16 MHz (debug)  */
#define SDMMC_CLKCR_CLKDIV_20MHZ    (MHZ(48) / MHZ(20) - 2)  /* SD clock 20 MHz (MMC) */
#define SDMMC_CLKCR_CLKDIV_25MHZ    (MHZ(48) / MHZ(24) - 2)  /* SD clock 24 MHz (max) */

/* limit the Default and High Speed clock rates for debugging */
#if CONFIG_SDMMC_CLK_MAX_400KHZ
#define CONFIG_SDMMC_CLK_MAX        SDMMC_CLKCR_CLKDIV_400KHZ
#elif CONFIG_SDMMC_CLK_MAX_1MHZ
#define CONFIG_SDMMC_CLK_MAX        SDMMC_CLKCR_CLKDIV_1MHZ
#elif CONFIG_SDMMC_CLK_MAX_4MHZ
#define CONFIG_SDMMC_CLK_MAX        SDMMC_CLKCR_CLKDIV_4MHZ
#elif CONFIG_SDMMC_CLK_MAX_8MHZ
#define CONFIG_SDMMC_CLK_MAX        SDMMC_CLKCR_CLKDIV_8MHZ
#elif CONFIG_SDMMC_CLK_MAX_16MHZ
#define CONFIG_SDMMC_CLK_MAX        SDMMC_CLKCR_CLKDIV_16MHZ
#endif

/* for F1, F2, F4 and L1 the SDMMC interface is called SDIO, defined the
 * corresponding symbols for compatibility */
#if !defined(SDMMC1)

#define SDMMC1_IRQn                 SDIO_IRQn

#define SDMMC_POWER_PWRCTRL_Pos     SDIO_POWER_PWRCTRL_Pos

#define SDMMC_CMD_CMDINDEX_Pos      SDIO_CMD_CMDINDEX_Pos
#define SDMMC_CMD_CMDINDEX          SDIO_CMD_CMDINDEX
#define SDMMC_CMD_WAITRESP_Pos      SDIO_CMD_WAITRESP_Pos
#define SDMMC_CMD_WAITRESP_0        SDIO_CMD_WAITRESP_0
#define SDMMC_CMD_WAITRESP_1        SDIO_CMD_WAITRESP_1
#define SDMMC_CMD_CPSMEN            SDIO_CMD_CPSMEN

#define SDMMC_DCTRL_DBLOCKSIZE_Pos  SDIO_DCTRL_DBLOCKSIZE_Pos
#define SDMMC_DCTRL_DTDIR           SDIO_DCTRL_DTDIR
#define SDMMC_DCTRL_DTEN            SDIO_DCTRL_DTEN
#define SDMMC_DCTRL_DMAEN           SDIO_DCTRL_DMAEN

#define SDMMC_STA_SDIOIT            SDIO_STA_SDIOIT
#define SDMMC_STA_CMDSENT           SDIO_STA_CMDSENT
#define SDMMC_STA_CMDREND           SDIO_STA_CMDREND
#define SDMMC_STA_CCRCFAIL          SDIO_STA_CCRCFAIL
#define SDMMC_STA_CTIMEOUT          SDIO_STA_CTIMEOUT
#define SDMMC_STA_DCRCFAIL          SDIO_STA_DCRCFAIL
#define SDMMC_STA_DTIMEOUT          SDIO_STA_DTIMEOUT
#define SDMMC_STA_DATAEND           SDIO_STA_DATAEND
#define SDMMC_STA_DBCKEND           SDIO_STA_DBCKEND
#define SDMMC_STA_TXUNDERR          SDIO_STA_TXUNDERR
#define SDMMC_STA_TXFIFOE           SDIO_STA_TXFIFOE
#define SDMMC_STA_TXFIFOF           SDIO_STA_TXFIFOF
#define SDMMC_STA_TXFIFOHE          SDIO_STA_TXFIFOHE
#define SDMMC_STA_TXACT             SDIO_STA_TXACT
#define SDMMC_STA_RXOVERR           SDIO_STA_RXOVERR
#define SDMMC_STA_RXFIFOE           SDIO_STA_TXFIFOE
#define SDMMC_STA_RXFIFOF           SDIO_STA_TXFIFOF
#define SDMMC_STA_RXFIFOHF          SDIO_STA_RXFIFOHF
#define SDMMC_STA_RXDAVL            SDIO_STA_RXDAVL
#define SDMMC_STA_RXACT             SDIO_STA_RXACT

#ifdef SDIO_STA_STBITERR
#define SDMMC_STA_STBITERR          SDIO_STA_STBITERR
#endif
#ifdef SDIO_MASK_STBITERRIE
#define SDMMC_MASK_STBITERRIE       SDIO_MASK_STBITERRIE
#endif

#define SDMMC_MASK_CMDSENTIE        SDIO_MASK_CMDSENTIE
#define SDMMC_MASK_CMDRENDIE        SDIO_MASK_CMDRENDIE
#define SDMMC_MASK_CCRCFAILIE       SDIO_MASK_CCRCFAILIE
#define SDMMC_MASK_CTIMEOUTIE       SDIO_MASK_CTIMEOUTIE
#define SDMMC_MASK_DCRCFAILIE       SDIO_MASK_DCRCFAILIE
#define SDMMC_MASK_DTIMEOUTIE       SDIO_MASK_DTIMEOUTIE
#define SDMMC_MASK_DATAENDIE        SDIO_MASK_DATAENDIE
#define SDMMC_MASK_DBCKENDIE        SDIO_MASK_DBCKENDIE
#define SDMMC_MASK_TXUNDERRIE       SDIO_MASK_TXUNDERRIE
#define SDMMC_MASK_TXFIFOEIE        SDIO_MASK_TXFIFOEIE
#define SDMMC_MASK_TXFIFOFIE        SDIO_MASK_TXFIFOFIE
#define SDMMC_MASK_TXFIFOHEIE       SDIO_MASK_TXFIFOHEIE
#define SDMMC_MASK_RXOVERRIE        SDIO_MASK_RXOVERRIE
#define SDMMC_MASK_RXFIFOHFIE       SDIO_MASK_RXFIFOHFIE

#define SDMMC_ICR_CCRCFAILC         SDIO_ICR_CCRCFAILC
#define SDMMC_ICR_DCRCFAILC         SDIO_ICR_DCRCFAILC
#define SDMMC_ICR_CTIMEOUTC         SDIO_ICR_CTIMEOUTC
#define SDMMC_ICR_DTIMEOUTC         SDIO_ICR_DTIMEOUTC
#define SDMMC_ICR_TXUNDERRC         SDIO_ICR_TXUNDERRC
#define SDMMC_ICR_RXOVERRC          SDIO_ICR_RXOVERRC
#define SDMMC_ICR_CMDRENDC          SDIO_ICR_CMDRENDC
#define SDMMC_ICR_CMDSENTC          SDIO_ICR_CMDSENTC
#define SDMMC_ICR_DATAENDC          SDIO_ICR_DATAENDC
#define SDMMC_ICR_DBCKENDC          SDIO_ICR_DBCKENDC
#define SDMMC_ICR_SDIOITC           SDIO_ICR_SDIOITC

#define SDMMC_CLKCR_NEGEDGE         SDIO_CLKCR_NEGEDGE
#define SDMMC_CLKCR_CLKEN           SDIO_CLKCR_CLKEN
#define SDMMC_CLKCR_PWRSAV          SDIO_CLKCR_PWRSAV
#define SDMMC_CLKCR_BYPASS_Msk      SDIO_CLKCR_BYPASS_Msk
#define SDMMC_CLKCR_CLKDIV_Msk      SDIO_CLKCR_CLKDIV_Msk
#define SDMMC_CLKCR_WIDBUS_Msk      SDIO_CLKCR_WIDBUS_Msk
#define SDMMC_CLKCR_WIDBUS_Pos      SDIO_CLKCR_WIDBUS_Pos

#define isr_sdmmc1                  isr_sdio

#endif /* !defined(SDMMC1) */

#ifndef SDMMC_STA_STBITERR
#define SDMMC_STA_STBITERR          (1UL << 9)
#endif

#define SDMMC_CLKCR_WIDBUS_1BIT     (0UL << SDMMC_CLKCR_WIDBUS_Pos)
#define SDMMC_CLKCR_WIDBUS_4BIT     (1UL << SDMMC_CLKCR_WIDBUS_Pos)
#define SDMMC_CLKCR_WIDBUS_8BIT     (2UL << SDMMC_CLKCR_WIDBUS_Pos)

/* Physical Layer Simplified Specification Version 9.00,
 * 4.6.2 Read, Write and Erase Timeout Conditions
 * Maximum values are used here */
#define SDMMC_DATA_R_TIMEOUT        (MHZ(24) / 10)  /**< Data read timeout 100ms */
#define SDMMC_DATA_W_TIMEOUT        (MHZ(24) / 4)   /**< Date write timeout 250ms */

enum {
    SDMMC_POWER_PWRCTRL_OFF = 0b00,
    SDMMC_POWER_PWRCTRL_ON  = 0b11,
};

#include "board.h"

/* forward declaration of _driver */
static const sdmmc_driver_t _driver;

typedef struct {
    sdmmc_dev_t sdmmc_dev;      /**< Inherited sdmmc_dev_t struct */
    const sdmmc_conf_t *config; /**< SDIO/SD/MMC peripheral config   */
    uint32_t irq_status;        /**< Interrupt status */
    mutex_t irq_wait;           /**< Wait for an enabled interrupt */
} stm32_sdmmc_dev_t;

/* driver related */
static stm32_sdmmc_dev_t _sdmmc_devs[] = {
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .config = &sdmmc_config[0],
        .irq_wait = MUTEX_INIT_LOCKED,
    },
#if SDMMC_CONFIG_NUMOF > 1
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .config = &sdmmc_config[1],
        .irq_wait = MUTEX_INIT_LOCKED,
    }
#endif
};

/* sanity check of configuration */
static_assert(SDMMC_CONFIG_NUMOF == ARRAY_SIZE(sdmmc_config),
              "SDMMC_CONFIG_NUMOF and the number of elements in sdmmc_config differ");
static_assert(SDMMC_CONFIG_NUMOF == ARRAY_SIZE(_sdmmc_devs),
              "SDMMC_CONFIG_NUMOF and the number of elements in sdmmc_devs differ");

/* check that the number of devices does not exhaust the number of available devices */
#if defined(SDMMC2)
static_assert(SDMMC_CONFIG_NUMOF < 3, "MCU supports only 2 SDIO/SD/MMC interfaces");
#else
static_assert(SDMMC_CONFIG_NUMOF < 2, "MCU supports only 1 SDIO/SD/MMC interface");
#endif

XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0) _sdmmc_1 = (sdmmc_dev_t * const)&_sdmmc_devs[0];
#if SDMMC_CONFIG_NUMOF > 1
XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0) _sdmmc_2 = (sdmmc_dev_t * const)&_sdmmc_devs[1];
#endif

static inline bool _use_dma(const sdmmc_conf_t *conf)
{
#if IS_USED(MODULE_PERIPH_DMA)
    /* TODO SDMMC_IDMA_IDMAEN */
#   ifdef SDMMC_IDMA_IDMAEN
        /* The SDMMC of the L4+ and some L5 MCUs have an internal DMA controller
         * (IDMA) instead of using the DMA periphery. This internal DMA requires
         * separate handling. */
        return false;
#   else
        return (conf->dma != DMA_STREAM_UNDEF);
#   endif
#else /* IS_USED(MODULE_PERIPH_DMA) */
    (void)conf;
    return false;
#endif /* IS_USED(MODULE_PERIPH_DMA) */
}

static void _isr(SDMMC_TypeDef *dev);
static void _isr_cd_pin(void *arg);

static void _config_sdmmc(sdmmc_dev_t *sdmmc_dev)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);
    const sdmmc_conf_t *conf = dev->config;

#if !defined(CPU_FAM_STM32F1)
    /* reset SDIO/SD/MMC */
    /* TODO: reset for STM32F1, STM32F1 is using AHB */

    /* APB2 or AHB2 is used for SDIO/SD/MMC peripheral, bit mask for the reset
     * register is always the same as for the enable register */
    if (conf->bus == APB2) {
        RCC->APB2RSTR |= conf->rcc_mask;
        RCC->APB2RSTR &= ~conf->rcc_mask;
    }
    else if (conf->bus == AHB2) {
        RCC->AHB2RSTR |= conf->rcc_mask;
        RCC->AHB2RSTR &= ~conf->rcc_mask;
    }
    else {
        /* should not happen */
        assert(false);
    }
#endif
    /* power on SDIO/SD/MMC interface */
    conf->dev->POWER = (SDMMC_POWER_PWRCTRL_ON << SDMMC_POWER_PWRCTRL_Pos);
    conf->dev->CLKCR = SDMMC_CLKCR_WIDBUS_1BIT | SDMMC_CLKCR_CLKDIV_400KHZ |
                       SDMMC_CLKCR_CLKEN | SDMMC_CLKCR_PWRSAV;
}

static void _init_pins(sdmmc_dev_t *sdmmc_dev)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);
    const sdmmc_conf_t *conf = dev->config;

    /* configure GPIOs of the SDIO/SD/MMC interface, at minimum 1 data line */
    assert(gpio_is_valid(conf->clk.pin));
    assert(gpio_is_valid(conf->cmd.pin));
    assert(gpio_is_valid(conf->dat0.pin));

    sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_1BIT;
    gpio_init(conf->clk.pin, GPIO_OUT);
    gpio_init(conf->cmd.pin, GPIO_OUT);
    gpio_init(conf->dat0.pin, GPIO_OUT);

    if (gpio_is_valid(conf->dat1.pin) && gpio_is_valid(conf->dat2.pin) &&
        gpio_is_valid(conf->dat3.pin)) {
        sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_4BIT;
        gpio_init(conf->dat1.pin, GPIO_OUT);
        gpio_init(conf->dat2.pin, GPIO_OUT);
        gpio_init(conf->dat3.pin, GPIO_OUT);
    }

#if !defined(CPU_FAM_STM32F1)
    gpio_init_af(conf->cmd.pin, conf->cmd.af);
    gpio_init_af(conf->clk.pin, conf->clk.af);
    gpio_init_af(conf->dat0.pin, conf->dat0.af);

    if (sdmmc_dev->bus_width == SDMMC_BUS_WIDTH_4BIT) {
        gpio_init_af(conf->dat1.pin, conf->dat1.af);
        gpio_init_af(conf->dat2.pin, conf->dat2.af);
        gpio_init_af(conf->dat3.pin, conf->dat3.af);
    }
#endif

#if IS_USED(MODULE_PERIPH_SDMMC_8BIT)
    if (gpio_is_valid(conf->dat4.pin) && gpio_is_valid(conf->dat5.pin) &&
        gpio_is_valid(conf->dat6.pin) && gpio_is_valid(conf->dat7.pin)) {
        sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_8BIT;
        gpio_init(conf->dat4.pin, GPIO_OUT);
        gpio_init(conf->dat5.pin, GPIO_OUT);
        gpio_init(conf->dat6.pin, GPIO_OUT);
        gpio_init(conf->dat7.pin, GPIO_OUT);
#if !defined(CPU_FAM_STM32F1)
        gpio_init_af(conf->dat4.pin, conf->dat0.af);
        gpio_init_af(conf->dat5.pin, conf->dat1.af);
        gpio_init_af(conf->dat6.pin, conf->dat2.af);
        gpio_init_af(conf->dat7.pin, conf->dat3.af);
#endif /* !defined(CPU_FAM_STM32F1) */
    }
#endif /* IS_USED(MODULE_PERIPH_SDMMC_8BIT) */

    if (gpio_is_valid(conf->cd)) {
        gpio_init_int(conf->cd, conf->cd_mode, GPIO_BOTH, _isr_cd_pin, sdmmc_dev);
        sdmmc_dev->present = gpio_read(conf->cd) == conf->cd_active;
    }
    else {
        sdmmc_dev->present = true;
    }
}

static void _init(sdmmc_dev_t *sdmmc_dev)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    const sdmmc_conf_t *conf = dev->config;
    assert(conf);

    /* initialize GPIOs */
    _init_pins(sdmmc_dev);

#if defined(CPU_FAM_STM32F2) || defined(CPU_FAM_STM32F4) || \
    defined(CPU_FAM_STM32F7) || defined(CPU_FAM_STM32L4)
    /* TODO: init_clock (use RCC_DCKCFGR2_SDMMCxSEL to select system clock)
     * PLL clock with 48 MHz is used by default, use module periph_usbdev_clk */
#else
#error "STM32 family not supported yet"
#endif

    /* enable peripheral clock */
    periph_clk_en(conf->bus, conf->rcc_mask);

    /* reset and configure the peripheral */
    _config_sdmmc(sdmmc_dev);

#if defined(SDIO)
    assert (conf->dev == SDIO);
    NVIC_SetPriority(SDIO_IRQn, SDMMC_IRQ_PRIO);
    NVIC_EnableIRQ(SDIO_IRQn);
#else /* defined(SDIO) */
#if defined(SDMMC1)
    if (conf->dev == SDMMC1) {
        NVIC_SetPriority(SDMMC1_IRQn, SDMMC_IRQ_PRIO);
        NVIC_EnableIRQ(SDMMC1_IRQn);
    }
#endif
#if defined(SDMMC2)
    else if (conf->dev == SDMMC2) {
        NVIC_SetPriority(SDMMC2_IRQn, SDMMC_IRQ_PRIO);
        NVIC_EnableIRQ(SDMMC2_IRQn);
    }
#endif
#endif /* defined(SDIO) */
}

static int _send_cmd(sdmmc_dev_t *sdmmc_dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                     sdmmc_resp_t resp_type, uint32_t *resp)
{
    /* to ensure that `sdmmc_send_acmd` is used for application specific commands */
    assert((cmd_idx & SDMMC_ACMD_PREFIX) == 0);

    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    const sdmmc_conf_t *conf = dev->config;
    assert(conf);

    SDMMC_TypeDef *sdmmc = conf->dev;

    /* prepare the CMD register value, it has to be written as in one access */
    uint32_t cmd = (cmd_idx & SDMMC_CMD_CMDINDEX) << SDMMC_CMD_CMDINDEX_Pos;

    switch (resp_type) {
        case SDMMC_NO_R:
            break;
        case SDMMC_R2:
            cmd |= SDMMC_CMD_WAITRESP_LONG;
            break;
        default:
            cmd |= SDMMC_CMD_WAITRESP_SHORT;
            break;
    }

    /* write ARG and enable Command Path State Machine */
    sdmmc->ARG = arg;
    sdmmc->CMD = cmd | SDMMC_CMD_CPSMEN;

    /* clear and enable command related interrupts */
    sdmmc->ICR  &= ~SDMMC_INT_STATIC_CMD;
    sdmmc->MASK &= ~SDMMC_INT_STATIC_CMD;
    sdmmc->MASK |= SDMMC_MASK_CTIMEOUTIE;
    sdmmc->MASK |= (resp_type == SDMMC_NO_R) ? SDMMC_MASK_CMDSENTIE
                                             : SDMMC_MASK_CMDRENDIE |
                                               SDMMC_MASK_CCRCFAILIE;

    /* wait for command sent or response received */
    mutex_lock(&dev->irq_wait);

    /* disable and clear command related static interrupts*/
    sdmmc->MASK &= ~SDMMC_INT_STATIC_CMD;
    sdmmc->ICR &= ~SDMMC_INT_STATIC_CMD;

    /* disable the Command Path State Machine */
    sdmmc->CMD &= ~SDMMC_CMD_CPSMEN;

    if (dev->irq_status & SDMMC_STA_CTIMEOUT) {
        DEBUG("[sdmmc] Timeout error\n");
        return -ETIMEDOUT;
    }
    else if ((dev->irq_status & SDMMC_STA_CCRCFAIL) && (resp_type != SDMMC_R3)) {
        DEBUG("[sdmmc] CRC error in response\n");
        return -EBADMSG;
    }

    if ((resp_type == SDMMC_R1) || (resp_type == SDMMC_R1B)) {
        dev->sdmmc_dev.status = sdmmc->RESP1;
    }

    if (resp) {
        if (resp_type != SDMMC_NO_R) {
            resp[0] = sdmmc->RESP1;
        }
        if (resp_type == SDMMC_R2) {
            resp[1] = sdmmc->RESP2;
            resp[2] = sdmmc->RESP3;
            resp[3] = sdmmc->RESP4;
        }
    }

    return 0;
}

static int _set_bus_width(sdmmc_dev_t *sdmmc_dev, sdmmc_bus_width_t width)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    SDMMC_TypeDef *sdmmc = dev->config->dev;

    /* Note: bits have to be cleared and written as a single write operation */
    switch (width) {
    case SDMMC_BUS_WIDTH_1BIT:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~SDMMC_CLKCR_WIDBUS_Msk) |
                       SDMMC_CLKCR_WIDBUS_1BIT;
        break;
    case SDMMC_BUS_WIDTH_4BIT:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~SDMMC_CLKCR_WIDBUS_Msk) |
                       SDMMC_CLKCR_WIDBUS_4BIT;
        break;
    case SDMMC_BUS_WIDTH_8BIT:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~SDMMC_CLKCR_WIDBUS_Msk) |
                       SDMMC_CLKCR_WIDBUS_8BIT;
        break;
    }

    /* prevent to write again data to this register within three SDIOCLK
     * clock periods plus two PCLK2 clock periods */
    _ZTIMER_SLEEP_MS(1);

    return 0;
}

static int _set_clock_rate(sdmmc_dev_t *sdmmc_dev, sdmmc_clock_rate_t rate)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    SDMMC_TypeDef *sdmmc = dev->config->dev;

    /* Note: bits have to be cleared and written as a single write operation */
    switch (rate) {
    case SDMMC_CLK_400K:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~(SDMMC_CLKCR_CLKDIV_Msk | SDMMC_CLKCR_BYPASS_Msk)) |
                       SDMMC_CLKCR_CLKDIV_400KHZ;
        break;
#ifdef CONFIG_SDMMC_CLK_MAX
    default:
        /* limit the Default and High Speed clock rates for debugging */
        sdmmc->CLKCR = (sdmmc->CLKCR & ~(SDMMC_CLKCR_CLKDIV_Msk | SDMMC_CLKCR_BYPASS_Msk)) |
                       CONFIG_SDMMC_CLK_MAX;
#else
    case SDMMC_CLK_20M:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~(SDMMC_CLKCR_CLKDIV_Msk | SDMMC_CLKCR_BYPASS_Msk)) |
                       SDMMC_CLKCR_CLKDIV_20MHZ;
        break;
    case SDMMC_CLK_25M:
    case SDMMC_CLK_26M:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~(SDMMC_CLKCR_CLKDIV_Msk | SDMMC_CLKCR_BYPASS_Msk)) |
                       SDMMC_CLKCR_CLKDIV_25MHZ;
        break;
    case SDMMC_CLK_52M:
    case SDMMC_CLK_50M:
        sdmmc->CLKCR = (sdmmc->CLKCR & ~(SDMMC_CLKCR_CLKDIV_Msk | SDMMC_CLKCR_BYPASS_Msk)) |
                       SDMMC_CLKCR_BYPASS_Msk;
        break;
    default:
        return -ENOTSUP;
#endif
    }

    /* prevent to write again data to this register within three SDIOCLK
     * clock cycles plus two PCLK2 clock cycles */
    _ZTIMER_SLEEP_MS(1);

    return 0;
}

static int _xfer_prepare(sdmmc_dev_t *sdmmc_dev, sdmmc_xfer_desc_t *xfer)
{
    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    SDMMC_TypeDef *sdmmc = dev->config->dev;

    /* SDIO/SD/MMC uses 32-bit words */
    /* TODO: at the moment only 32-bit words supported */
    assert((xfer->block_size % sizeof(uint32_t)) == 0);

    /* stop the Data Path State Machine and set the block size */
    sdmmc->DCTRL = 0;

    /* configure data transfer */
    sdmmc->DTIMER = xfer->write ? SDMMC_DATA_W_TIMEOUT : SDMMC_DATA_R_TIMEOUT;
    sdmmc->DLEN = xfer->block_size * xfer->block_num;

#if IS_USED(MODULE_PERIPH_DMA)
    const sdmmc_conf_t *conf = dev->config;

    if (_use_dma(conf)) {
        dma_acquire(conf->dma);
        dma_setup(conf->dma, conf->dma_chan, (uint32_t*)&(conf->dev->FIFO),
                  xfer->write ? DMA_MEM_TO_PERIPH : DMA_PERIPH_TO_MEM,
                  DMA_DATA_WIDTH_WORD, false);
        /* additional configuration needed for F2/F4/F7 */
        dma_setup_ext(conf->dma, DMA_BURST_INCR4, DMA_BURST_INCR4,
                      true, DMA_FIFO_FULL, true);
    }
#endif

    return 0;
}

static int _xfer_execute(sdmmc_dev_t *sdmmc_dev, sdmmc_xfer_desc_t *xfer,
                         const void *data_wr, void *data_rd,
                         uint16_t *done)
{
    assert(xfer);
    assert((xfer->write && data_wr) || (!xfer->write && data_rd));

    /* check the alignment required for the buffers */
    assert(HAS_ALIGNMENT_OF(data_wr, SDMMC_CPU_DMA_ALIGNMENT));
    assert(HAS_ALIGNMENT_OF(data_rd, SDMMC_CPU_DMA_ALIGNMENT));

    stm32_sdmmc_dev_t *dev = container_of(sdmmc_dev, stm32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    const sdmmc_conf_t *conf = dev->config;
    assert(conf);

    SDMMC_TypeDef *sdmmc = conf->dev;

    uint32_t *data_to_read = data_rd;
    const uint32_t *data_to_write = data_wr;

    /* clear data transfer related static interrupt flags */
    sdmmc->ICR = SDMMC_INT_STATIC_DATA;

    /* enable used data interrupts */
    uint32_t status_mask = SDMMC_MASK_DCRCFAILIE | SDMMC_MASK_DTIMEOUTIE;
    status_mask |= xfer->write ? SDMMC_MASK_TXUNDERRIE : SDMMC_MASK_RXOVERRIE;
    status_mask |= (xfer->block_num == 1) ? SDMMC_MASK_DBCKENDIE
                                          : SDMMC_MASK_DATAENDIE;
#if defined(SDMMC_MASK_STBITERRIE)
    /* Some CMSIS headers define the STBITERR enable and clear flag but not
     * the status flag. For such MCU models, the STBITERR flag is not documented
     * in the RM but marked as reserved. */
    status_mask |= SDMMC_MASK_STBITERRIE;
#endif

    sdmmc->MASK |= status_mask;

    /* configure data transfer block size, read direction and enable the transfer */
    uint32_t dctrl = 0;
    dctrl |= bitarithm_msb(xfer->block_size) << SDMMC_DCTRL_DBLOCKSIZE_Pos;
    dctrl |= !xfer->write ? SDMMC_DCTRL_DTDIR : 0;

    /* TODO: at the moment only 32-bit word transfers supported */
    assert((xfer->block_size % sizeof(uint32_t)) == 0);
    uint32_t fifo_words = sdmmc->DLEN >> 2; /* 32-bit words to read or write */

    /* read data blocks from or write data blocks to the card */
    if (_use_dma(conf)) {
#if IS_USED(MODULE_PERIPH_DMA)
        dma_prepare(conf->dma,
                    (xfer->write) ? (void *)data_to_write : data_to_read,
                    fifo_words, 1);
        dma_start(conf->dma);

        /* TODO SDMMC_IDMA_IDMAEN
         * The SDMMC of the L4+ and some L5 MCUs have an internal DMA controller
         * (IDMA) instead of using the DMA periphery. This internal DMA requires
         * separate handling. Since `_use_dma` just returns `false` if
         * `SDMMC_IDMA_IDMAEN` is defined, this part of the code isn't used and
         * should be optimized out by the compiler. */

        dctrl |= SDMMC_DCTRL_DMAEN;
        sdmmc->DCTRL = dctrl | SDMMC_DCTRL_DTEN;

        /* wait for DBCKEND, DATAEND or any error interrupt */
        mutex_lock(&dev->irq_wait);

        /* stop and release DMA */
        dma_stop(conf->dma);
        dma_release(conf->dma);
#endif
    }
    else {
        /* start the Data Path State Machine */
        sdmmc->DCTRL = dctrl | SDMMC_DCTRL_DTEN;

        do {
            if (xfer->write) {
                /* Fill the TX FIFO as long as TXFIFOF (TX FIFO Full) is not set
                 * and further data have to be written. */
                while (((sdmmc->STA & SDMMC_STA_TXFIFOF) == 0) && fifo_words) {
                    sdmmc->FIFO = *data_to_write++;
                    fifo_words--;
                }
                /* Once the FIFO is full and more data needs to be sent, wait
                 * for TXFIFOHE (TX FIFO Half Empty), i.e. there are at least
                 * 8 32-bit words available in the FIFO again. TXFIFOHE is
                 * dynamic and can't be cleared. It is therefore disabled in
                 * ISR to prevent endless triggering. */
                if (fifo_words) {
                    sdmmc->MASK |= status_mask | SDMMC_MASK_TXFIFOHEIE;
                }
                /* wait for TXFIFOHE or any other status interrupt */
                mutex_lock(&dev->irq_wait);
            }
            else {
                /* If further data have to be read, read all available data
                 * from the RX FIFO (as long as RXDAVL is set) at once */
                while ((sdmmc->STA & SDMMC_STA_RXDAVL) && fifo_words) {
                    *data_to_read++ = sdmmc->FIFO;
                    fifo_words--;
                }
                /* If there are still at least 8 32-bit words to be read, wait
                 * until at least 8 32-bit words have been received in the FIFO.
                 * RXFIFOHF interrupt is dynamic and can't be cleared. It is
                 * therefore disabled in ISR to prevent endless triggering. */
                if (fifo_words >= 8) {
                    sdmmc->MASK |= SDMMC_MASK_RXFIFOHFIE;
                }
                /* wait for RXFIFOHF or any other status interrupt */
                mutex_lock(&dev->irq_wait);
            }
        } while (!(dev->irq_status & status_mask));

        if (!xfer->write) {
            /* read remaining data from RX FIFO if there are any */
            while (sdmmc->STA & SDMMC_STA_RXDAVL) {
                *data_to_read++ = sdmmc->FIFO;
            }
        }
    }

    /* disable the Data Path State Machine */
    sdmmc->DCTRL = 0;

    /* disable and clear all data transfer related static interrupts */
    sdmmc->MASK &= ~SDMMC_INT_STATIC_DATA;
    sdmmc->ICR = SDMMC_INT_STATIC_DATA;

    if (done) {
        *done = xfer->block_num - (sdmmc->DCOUNT / xfer->block_size);
    }

    if (dev->irq_status & SDMMC_STA_DTIMEOUT) {
        return -ETIMEDOUT;
    }
    else if (dev->irq_status & SDMMC_STA_DCRCFAIL) {
        return -EBADMSG;
    }
    else if (dev->irq_status & SDMMC_STA_RXOVERR) {
        return -EOVERFLOW;
    }
    else if (dev->irq_status & SDMMC_STA_TXUNDERR) {
        return -EOVERFLOW;
    }
#if defined(SDMMC_STA_STBITERR)
    else if (dev->irq_status & SDMMC_STA_STBITERR) {
        return -EIO;
    }
#endif

    return 0;
}

static int _xfer_finish(sdmmc_dev_t *sdmmc_dev, sdmmc_xfer_desc_t *xfer)
{
    (void)sdmmc_dev;
    (void)xfer;

    return 0;
}

static void _isr(SDMMC_TypeDef *dev)
{
    unsigned i;

    /* determine the index of the device that triggers the IRQ */
    for (i = 0; i < ARRAY_SIZE(_sdmmc_devs); i++) {
        if (_sdmmc_devs[i].config->dev == dev) {
            break;
        }
    }
    assert(i < ARRAY_SIZE(_sdmmc_devs));
    stm32_sdmmc_dev_t *sdmmc_dev = &_sdmmc_devs[i];

    if ((dev->STA & SDMMC_STA_TXFIFOHE) && (dev->MASK & SDMMC_MASK_TXFIFOHEIE)) {
        /* Write transfer is waiting for free FIFO space signaled by RXFIFOF.
         * Since TXFIFOHE is dynamic and can't be cleared, it has to be
         * disabled here to prevent endless triggering. */
        sdmmc_dev->irq_status = dev->STA;
        dev->MASK &= ~SDMMC_MASK_TXFIFOHEIE;
        mutex_unlock(&sdmmc_dev->irq_wait);
    }
    else if ((dev->STA & SDMMC_STA_RXFIFOHF) && (dev->MASK & SDMMC_MASK_RXFIFOHFIE)) {
        /* Read transfer is waiting for available data signaled by RXFIFOF.
         * Since RXFIFOHF is dynamic and can't be cleared, it has to be
         * disabled here to prevent endless triggering. */
        sdmmc_dev->irq_status = dev->STA;
        dev->MASK &= ~SDMMC_MASK_RXFIFOHFIE;
        mutex_unlock(&sdmmc_dev->irq_wait);
    }
    else if (dev->STA & SDMMC_INT_STATIC_DATA) {
        /* data transfer is in progress and has to be handled. */
        sdmmc_dev->irq_status = dev->STA;
        dev->ICR = SDMMC_INT_STATIC_DATA;
        mutex_unlock(&sdmmc_dev->irq_wait);
    }
    else if (dev->STA & SDMMC_INT_STATIC_CMD) {
        /* Send command is in progress and has to be handled. */
        sdmmc_dev->irq_status = dev->STA;
        dev->ICR = SDMMC_INT_STATIC_CMD;
        mutex_unlock(&sdmmc_dev->irq_wait);
    }

#ifdef MODULE_CORTEXM_COMMON
    cortexm_isr_end();
#endif
}

#if defined(SDIO)
void isr_sdio(void)
{
    _isr(SDIO);
}
#endif

#if defined(SDMMC1)
void isr_sdmmc1(void)
{
    _isr(SDMMC1);
}
#endif

#if defined(SDMMC2)
void _isr_sdmmc2(void)
{
    _isr(SDMMC2);
}
#endif

static void _isr_cd_pin(void *arg)
{
    stm32_sdmmc_dev_t *dev = arg;
    assert(dev);

    const sdmmc_conf_t *conf = dev->config;
    assert(conf);

    sdmmc_dev_t *sdmmc_dev = &dev->sdmmc_dev;

    sdmmc_dev->present = gpio_read(conf->cd) == conf->cd_active;
    sdmmc_dev->init_done = false;

    if (sdmmc_dev->event_cb) {
        sdmmc_dev->event_cb(sdmmc_dev,
                            sdmmc_dev->present ? SDMMC_EVENT_CARD_INSERTED
                                               : SDMMC_EVENT_CARD_REMOVED);
    }
}

static const sdmmc_driver_t _driver = {
    .init = _init,
    .card_init = NULL,  /* no own card init function */
    .send_cmd = _send_cmd,
    .set_bus_width = _set_bus_width,
    .set_clock_rate = _set_clock_rate,
    .xfer_prepare = _xfer_prepare,
    .xfer_execute = _xfer_execute,
    .xfer_finish = _xfer_finish,
};

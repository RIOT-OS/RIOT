/*
 * Copyright (C) 2023 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Low-level SDIO/SD/MMC peripheral driver interface for ESP32
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
#include "syscalls.h"
#include "ztimer.h"

#include "sdmmc/sdmmc.h"

#include "driver/sdmmc_types.h"
#include "driver/sdmmc_host.h"
#include "soc/sdmmc_reg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* CLK_EDGE_SEL - clock phase selection register */
#define SDMMC_CLOCK_REG_CCLKIN_EDGE_SAM_SEL_S   (3)
#define SDMMC_CLOCK_REG_CCLKIN_EDGE_SAM_SEL_M   (0x7 << SDMMC_CLOCK_REG_CCLKIN_EDGE_SAM_SEL_S)

/* we have to redefine it here since we can't include "gpio_types.h" due to
 * naming conflicts */
#define GPIO_NUM_NC     (GPIO_UNDEF)

/* debounce time for CD pin */
#define CONFIG_CD_PIN_DEBOUNCE_US   25000

/* limit the Default and High Speed clock rates for debugging */
#if CONFIG_SDMMC_CLK_MAX_400KHZ
#define CONFIG_SDMMC_CLK_MAX        KHZ(400)
#elif CONFIG_SDMMC_CLK_MAX_1MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(1)
#elif CONFIG_SDMMC_CLK_MAX_4MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(4)
#elif CONFIG_SDMMC_CLK_MAX_10MHZ
#define CONFIG_SDMMC_CLK_MAX        MHZ(10)
#elif CONFIG_SDMMC_CLK_MAX_20MHZ || !IS_USED(MODULE_PERIPH_SDMMC_HS)
#define CONFIG_SDMMC_CLK_MAX        MHZ(20)
#else
#define CONFIG_SDMMC_CLK_MAX        MHZ(40)
#endif

/* millisecond timer definitions dependent on active ztimer backend */
#if IS_USED(MODULE_ZTIMER_MSEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_MSEC, n)
#elif IS_USED(MODULE_ZTIMER_USEC)
#define _ZTIMER_SLEEP_MS(n)     ztimer_sleep(ZTIMER_USEC, n * US_PER_MS)
#else
#error "Either ztimer_msec or ztimer_usec is needed"
#endif

/* forward declaration of _driver */
static const sdmmc_driver_t _driver;

/* driver related */
typedef struct {
    sdmmc_dev_t sdmmc_dev;      /**< Inherited sdmmc_dev_t struct */
    const sdmmc_conf_t *config; /**< SDIO/SD/MMC peripheral config   */
    uint32_t last_cd_pin_irq;   /**< Last CD Pin IRQ time for debouncing */
    bool data_transfer;         /**< Transfer active */
} esp32_sdmmc_dev_t;

static esp32_sdmmc_dev_t _sdmmc_devs[] = {
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .config = &sdmmc_config[0],
    },
#if SDMMC_CONFIG_NUMOF == 2
    {
        .sdmmc_dev = {
                        .driver = &_driver,
                     },
        .config = &sdmmc_config[1],
    }
#endif
};

/* sanity check of configuration */
static_assert(SDMMC_CONFIG_NUMOF == ARRAY_SIZE(sdmmc_config),
              "SDMMC_CONFIG_NUMOF and the number of elements in sdmmc_config differ");
static_assert(SDMMC_CONFIG_NUMOF == ARRAY_SIZE(_sdmmc_devs),
              "SDMMC_CONFIG_NUMOF and the number of elements in sdmmc_devs differ");
static_assert(SDMMC_CONFIG_NUMOF <= SOC_SDMMC_NUM_SLOTS,
              "SDMMC_CONFIG_NUMOF is greater than the supported number of slots");

XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0) _sdmmc_0 = (sdmmc_dev_t * const)&_sdmmc_devs[0];
#if SDMMC_CONFIG_NUMOF > 1
XFA_CONST(sdmmc_dev_t * const, sdmmc_devs, 0) _sdmmc_1 = (sdmmc_dev_t * const)&_sdmmc_devs[1];
#endif

/* forward declaration of internal functions */
static int _esp_err_to_sdmmc_err_code(esp_err_t code);
static void _isr_cd_pin(void *arg);

static void _init(sdmmc_dev_t *sdmmc_dev)
{
    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    const sdmmc_conf_t *conf = dev->config;
    assert(conf);

    /* additional sanity checks */
    assert(conf->slot < SOC_SDMMC_NUM_SLOTS);

    sdmmc_slot_config_t slot_config = SDMMC_SLOT_CONFIG_DEFAULT();

#if IS_USED(CPU_FAM_ESP32)

    /* On ESP32 only Slot 1 can be used */
    assert(conf->slot == SDMMC_SLOT_1);
    /* Slot 1 has only 4 data lines */
    assert((conf->bus_width == 1) || (conf->bus_width == 4));

    sdmmc_dev->bus_width = conf->bus_width;

#elif IS_USED(CPU_FAM_ESP32S3)

    assert(gpio_is_valid(conf->clk) && !gpio_is_equal(conf->clk, GPIO0));
    assert(gpio_is_valid(conf->cmd) && !gpio_is_equal(conf->cmd, GPIO0));
    assert(gpio_is_valid(conf->dat0) && !gpio_is_equal(conf->dat0, GPIO0));

    /* TODO Check for collision with Flash GPIOs */
    slot_config.clk = conf->clk;
    slot_config.cmd = conf->cmd;
    slot_config.d0 = conf->dat0;

    sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_1BIT;

    slot_config.d1 = GPIO_UNDEF;
    slot_config.d2 = GPIO_UNDEF;
    slot_config.d3 = GPIO_UNDEF;
    slot_config.d4 = GPIO_UNDEF;
    slot_config.d5 = GPIO_UNDEF;
    slot_config.d6 = GPIO_UNDEF;
    slot_config.d7 = GPIO_UNDEF;

    if (gpio_is_valid(conf->dat1) && !gpio_is_equal(conf->dat1, GPIO0) &&
        gpio_is_valid(conf->dat2) && !gpio_is_equal(conf->dat2, GPIO0) &&
        gpio_is_valid(conf->dat3) && !gpio_is_equal(conf->dat3, GPIO0)) {
        slot_config.d1 = conf->dat1;
        slot_config.d2 = conf->dat2;
        slot_config.d3 = conf->dat3;
        sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_4BIT;

#if IS_USED(MODULE_PERIPH_SDMMC_8BIT)
        if (gpio_is_valid(conf->dat4) && !gpio_is_equal(conf->dat4, GPIO0) &&
            gpio_is_valid(conf->dat5) && !gpio_is_equal(conf->dat5, GPIO0) &&
            gpio_is_valid(conf->dat6) && !gpio_is_equal(conf->dat6, GPIO0) &&
            gpio_is_valid(conf->dat7) && !gpio_is_equal(conf->dat7, GPIO0)) {
            slot_config.d4 = conf->dat4;
            slot_config.d5 = conf->dat5;
            slot_config.d6 = conf->dat6;
            slot_config.d7 = conf->dat7;
            sdmmc_dev->bus_width = SDMMC_BUS_WIDTH_8BIT;
        }
#endif
    }

#else
#error "ESP32x variant not supported"
#endif

#if IS_USED(CONFIG_SDMMC_INTERNAL_PULLUP)
    slot_config.flags |= SDMMC_SLOT_FLAG_INTERNAL_PULLUP;
#endif

    slot_config.width = sdmmc_dev->bus_width;

    dev->data_transfer = false;

    esp_err_t res;

    if ((res = sdmmc_host_init())) {
        LOG_ERROR("[sdmmc] Could not initialize SDMMC host controller\n");
        assert(false);
    }
    if ((res = sdmmc_host_init_slot(dev->config->slot, &slot_config))) {
        LOG_ERROR("[sdmmc] Could not initialize SDMMC slot\n");
        assert(false);
    }

    if (gpio_is_valid(conf->cd)) {
        dev->last_cd_pin_irq = system_get_time();

        gpio_init_int(conf->cd, GPIO_IN, GPIO_BOTH, _isr_cd_pin, sdmmc_dev);
        sdmmc_dev->present = gpio_read(conf->cd) == 0;
    }
    else {
        sdmmc_dev->present = true;
    }
}

static int _send_cmd(sdmmc_dev_t *sdmmc_dev, sdmmc_cmd_t cmd_idx, uint32_t arg,
                     sdmmc_resp_t resp_type, uint32_t *resp)
{
    /* to ensure that `sdmmc_send_acmd` is used for application specific commands */
    assert((cmd_idx & SDMMC_ACMD_PREFIX) == 0);

    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    if (dev->data_transfer) {
        /* data transfer command is issued in _xfer_execute as one transaction
         * together with data phase */
        return 0;
    }

    sdmmc_command_t cmd = {
        .opcode =cmd_idx,
        .flags = 0,
        .arg = arg,
        .data = 0,
        .datalen = 0,
        .blklen = 0,
        .timeout_ms = 100,
    };

    switch (resp_type) {
    case SDMMC_R1:
        cmd.flags |= SCF_RSP_R1;
        break;
    case SDMMC_R1B:
        cmd.flags |= SCF_RSP_R1B;
        break;
    case SDMMC_R2:
        cmd.flags |= SCF_RSP_R2;
        break;
    case SDMMC_R3:
        cmd.flags |= SCF_RSP_R3;
        break;
    case SDMMC_R4:
        cmd.flags |= SCF_RSP_R4;
        break;
    case SDMMC_R5:
        cmd.flags |= SCF_RSP_R5;
        break;
    case SDMMC_R6:
        cmd.flags |= SCF_RSP_R7;
        break;
    case SDMMC_R7:
        cmd.flags |= SCF_RSP_R7;
        break;
    default:
        break;
    }

    esp_err_t res = sdmmc_host_do_transaction(dev->config->slot, &cmd);
    if (res) {
        return _esp_err_to_sdmmc_err_code(res);
    }
    else if (cmd.error) {
        return _esp_err_to_sdmmc_err_code(cmd.error);
    }

    if ((resp_type == SDMMC_R1) || (resp_type == SDMMC_R1B)) {
        sdmmc_dev->status = cmd.response[0];
    }

    if (resp) {
        if (resp_type == SDMMC_R2) {
            resp[0] = cmd.response[3];
            resp[1] = cmd.response[2];
            resp[2] = cmd.response[1];
            resp[3] = cmd.response[0];
        }
        else if (resp_type != SDMMC_NO_R) {
            resp[0] = cmd.response[0];
        }
    }

    return 0;
}

static int _set_bus_width(sdmmc_dev_t *sdmmc_dev, sdmmc_bus_width_t width)
{
    DEBUG("[sdmmc] %s width=%d\n", __func__, width);
    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    esp_err_t res = sdmmc_host_set_bus_width(dev->config->slot, width);
    if (res) {
        return _esp_err_to_sdmmc_err_code(res);
    }

    return 0;
}

static int _set_clock_rate(sdmmc_dev_t *sdmmc_dev, sdmmc_clock_rate_t rate)
{
    DEBUG("[sdmmc] %s rate=%"PRIu32" ", __func__, (uint32_t)rate);

    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);
    assert(dev);

    if (rate > CONFIG_SDMMC_CLK_MAX) {
        rate = CONFIG_SDMMC_CLK_MAX;
    }

    DEBUG("actual_rate=%"PRIu32"\n", (uint32_t)rate);

    esp_err_t res = sdmmc_host_set_card_clk(dev->config->slot, rate / KHZ(1));
    if (res) {
        return _esp_err_to_sdmmc_err_code(res);
    }

#if SOC_SDMMC_USE_GPIO_MATRIX
    /* phase has to be modified to get it working for MMCs if
     * SOC_SDMMC_USE_GPIO_MATRIX is used */
    uint32_t reg = *((uint32_t *)SDMMC_CLOCK_REG);
    reg &= ~SDMMC_CLOCK_REG_CCLKIN_EDGE_SAM_SEL_M;
    reg |= (6 << SDMMC_CLOCK_REG_CCLKIN_EDGE_SAM_SEL_S);
    *((uint32_t *)SDMMC_CLOCK_REG) = reg;
#endif

    return 0;
}

static int _xfer_prepare(sdmmc_dev_t *sdmmc_dev, sdmmc_xfer_desc_t *xfer)
{
    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    /* SDIO/SD/MMC uses 32-bit words */
    /* TODO: at the moment only 32-bit words supported */
    assert((xfer->block_size % sizeof(uint32_t)) == 0);

    dev->data_transfer = true;

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

    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);

    assert(dev);
    assert(dev->config);

    sdmmc_command_t cmd = {
        .opcode = xfer->cmd_idx & ~SDMMC_ACMD_PREFIX,
        .flags = SCF_RSP_R1 | (xfer->write ? 0 : SCF_CMD_READ),
        .arg = xfer->arg,
        .data = xfer->write ? (void *)data_wr : data_rd,
        .datalen = xfer->block_num * xfer->block_size,
        .blklen = xfer->block_size,
        .timeout_ms = xfer->write ? 2500 : 1000, // TODO
    };

    if (done) {
        *done = 0;
    }

    esp_err_t res = sdmmc_host_do_transaction(dev->config->slot, &cmd);
    if (res) {
        return _esp_err_to_sdmmc_err_code(res);
    }
    else if (cmd.error) {
        return _esp_err_to_sdmmc_err_code(cmd.error);
    }

    if (done) {
        *done = xfer->block_num;
    }

    return 0;
}

static int _xfer_finish(sdmmc_dev_t *sdmmc_dev, sdmmc_xfer_desc_t *xfer)
{
    (void)xfer;

    esp32_sdmmc_dev_t *dev = container_of(sdmmc_dev, esp32_sdmmc_dev_t, sdmmc_dev);
    dev->data_transfer = false;

    return 0;
}

static int _esp_err_to_sdmmc_err_code(esp_err_t error)
{
    switch (error) {
    case ESP_ERR_TIMEOUT:
        DEBUG("[sdmmc] Timeout error\n");
        return -ETIMEDOUT;
    case ESP_ERR_INVALID_CRC:
        DEBUG("[sdmmc] CRC error\n");
        return -EBADMSG;
    case ESP_ERR_INVALID_RESPONSE:
        DEBUG("[sdmmc] Invalid response\n");
        return -EIO;
    case ESP_ERR_INVALID_SIZE:
        DEBUG("[sdmmc] Invalid size\n");
        return -EIO;
    case ESP_ERR_INVALID_ARG:
        DEBUG("[sdmmc] Invalid argument\n");
        return -EIO;
    default:
        DEBUG("[sdmmc] Other error\n");
        return -EIO;
    }
}

static void _isr_cd_pin(void *arg)
{
    uint32_t state = irq_disable();

    esp32_sdmmc_dev_t *dev = arg;
    assert(dev);

    /* for debouncing handle only the first CD Pin interrupts and ignore further
     * interrupts that happen within the debouncing time interval */
    if ((system_get_time() - dev->last_cd_pin_irq) > CONFIG_CD_PIN_DEBOUNCE_US) {
        dev->last_cd_pin_irq = system_get_time();

        sdmmc_dev_t *sdmmc_dev = &dev->sdmmc_dev;

        sdmmc_dev->present = !sdmmc_dev->present;
        sdmmc_dev->init_done = false;

        if (sdmmc_dev->event_cb) {
            sdmmc_dev->event_cb(sdmmc_dev,
                                sdmmc_dev->present ? SDMMC_EVENT_CARD_INSERTED
                                                   : SDMMC_EVENT_CARD_REMOVED);
        }
    }
    irq_restore(state);
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

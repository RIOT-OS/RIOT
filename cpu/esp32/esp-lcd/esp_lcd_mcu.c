/*
 * SPDX-FileCopyrightText: 2023 Gunar Schorcht
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Peripheral low-Level parallel interface implementation for LCDs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>
#include <string.h>

#include "lcd.h"
#include "lcd_internal.h"
#include "log.h"
#include "macros/units.h"
#include "periph/gpio.h"
#include "ztimer.h"

#include "esp_lcd_common.h"
#include "esp_lcd_panel_io.h"
#include "soc/gpio_reg.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if !defined(CPU_FAM_ESP32) && !defined(CPU_FAM_ESP32S2) && !defined(CPU_FAM_ESP32S3)
#error "ESP32x SoC family not supported"
#endif

#ifndef CONFIG_LCD_WRITE_CLOCK_MHZ
#if CONFIG_LCD_I80_COLOR_IN_PSRAM
/* PCLK has to be low enough for SPI RAM */
#define CONFIG_LCD_WRITE_CLOCK_MHZ  2
#else

#if defined(CPU_FAM_ESP32S3)
#define CONFIG_LCD_WRITE_CLOCK_MHZ  20
#elif defined(CPU_FAM_ESP32S2)
#define CONFIG_LCD_WRITE_CLOCK_MHZ  40
#else /* ESP32 */
#define CONFIG_LCD_WRITE_CLOCK_MHZ  10
#endif

#endif /* CONFIG_LCD_I80_COLOR_IN_PSRAM */
#endif /* CONFIG_LCD_WRITE_CLOCK_MHZ */

static_assert(CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE >= 32,
              "CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE mus be at least 32");

static_assert(CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE <= LCD_I80_IO_FORMAT_BUF_SIZE,
              "CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE must be less or equal to "
              "LCD_I80_IO_FORMAT_BUF_SIZE");

/* ESP32x SoCs support only one LCD peripheral so we can use single instances
 * of the following variables */

int _cmd = -1;  /* means no command needed in ESP-IDF */

size_t _idx_bytes = 0;
uint8_t _data_bytes[CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE];
uint8_t _trans_bytes[CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE];

esp_lcd_i80_bus_handle_t _i80_bus_handle = NULL;
esp_lcd_panel_io_handle_t _i80_io_handle = NULL;

/* indicates that a transfer of the data buffer is still in progress and must
 * not be overwritten */
static bool _dma_transfer_in_progress = false;

static bool _dma_transfer_done(esp_lcd_panel_io_handle_t io_handle,
                               esp_lcd_panel_io_event_data_t *io_event_data,
                               void *user_ctx)
{
    (void)io_handle;
    (void)io_event_data;
    (void)user_ctx;

    _dma_transfer_in_progress = false;

    return false;
}

static void _lcd_ll_mcu_init(lcd_t *dev)
{
   esp_lcd_i80_bus_config_t i80_bus_config = {
        .dc_gpio_num = dev->params->dcx_pin,
        .wr_gpio_num = dev->params->wrx_pin,
        .clk_src = LCD_CLK_SRC_DEFAULT,
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
        .data_gpio_nums = {
            dev->params->d0_pin,
            dev->params->d1_pin,
            dev->params->d2_pin,
            dev->params->d3_pin,
            dev->params->d4_pin,
            dev->params->d5_pin,
            dev->params->d6_pin,
            dev->params->d7_pin,
            dev->params->d8_pin,
            dev->params->d9_pin,
            dev->params->d10_pin,
            dev->params->d11_pin,
            dev->params->d12_pin,
            dev->params->d13_pin,
            dev->params->d14_pin,
            dev->params->d15_pin,
        },
        .bus_width = 16,
#else
        .data_gpio_nums = {
            dev->params->d0_pin,
            dev->params->d1_pin,
            dev->params->d2_pin,
            dev->params->d3_pin,
            dev->params->d4_pin,
            dev->params->d5_pin,
            dev->params->d6_pin,
            dev->params->d7_pin,
        },
        .bus_width = 8,
#endif
        .max_transfer_bytes = dev->params->rgb_channels * 40 * sizeof(uint16_t),
    };

    esp_lcd_panel_io_i80_config_t i80_io_config = {
        .cs_gpio_num = gpio_is_valid(dev->params->cs_pin) ? (int)dev->params->cs_pin
                                                          : -1,
        .pclk_hz = MHZ(CONFIG_LCD_WRITE_CLOCK_MHZ),
        .trans_queue_depth = 10,
        .dc_levels = {
            .dc_idle_level = 0,
            .dc_cmd_level = 0,
            .dc_dummy_level = 1,
            .dc_data_level = 1,
        },
        .on_color_trans_done = _dma_transfer_done,
        .user_ctx = NULL,
        .lcd_cmd_bits = 8,
        .lcd_param_bits = 8,
    };

    esp_lcd_new_i80_bus(&i80_bus_config, &_i80_bus_handle);
    esp_lcd_new_panel_io_i80(_i80_bus_handle, &i80_io_config, &_i80_io_handle);

    if (gpio_is_valid(dev->params->rdx_pin)) {
        gpio_init(dev->params->rdx_pin, GPIO_IN_PU);
        gpio_set(dev->params->rdx_pin);
    }

    if (gpio_is_valid(dev->params->cs_pin)) {
        gpio_init(dev->params->cs_pin, GPIO_OUT);
        gpio_clear(dev->params->cs_pin);
    }
}

static void _lcd_ll_mcu_set_data_dir(lcd_t *dev, bool output)
{
    (void)dev;
    (void)output;
    LOG_ERROR("[lcd_ll_mcu] set dir: %d is not supported\n", output);
    /* not supported yet */
}

static void _lcd_ll_mcu_cmd_start(lcd_t *dev, uint8_t cmd, bool cont)
{
    DEBUG("[lcd_ll_mcu] write cmd: %02x\n", cmd);

    if (!cont) {
        /* cmd without parameters */
        esp_lcd_panel_io_tx_param(_i80_io_handle, cmd, NULL, 0);
        _cmd = -1;
    }
    else {
        /* cmd with parameters */
        _cmd = cmd;
    }
}

static void _lcd_ll_mcu_transfer(lcd_t *dev, bool cont)
{
    if (!cont) {
        /* if no further data follow, send the command with the data in the buffer */
        esp_lcd_panel_io_tx_param(_i80_io_handle, _cmd, _data_bytes, _idx_bytes);
        _cmd = -1;
        _idx_bytes = 0;
    }
    else if (_idx_bytes == CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE) {
        /* spin lock as long as a DMA data transfer is still in progress */
        while (_dma_transfer_in_progress) {}

        /* copy data buffer to the DMA transfer buffer */
        memcpy(_trans_bytes, _data_bytes, _idx_bytes);
        /* start DMA data transfer */
        _dma_transfer_in_progress = true;
        esp_lcd_panel_io_tx_color(_i80_io_handle, _cmd, _data_bytes, _idx_bytes);

        /* It should only be possible to follow more data than
         * CONFIG_LCD_PANEL_IO_FORMAT_BUF_SIZE with the RAMWR command.
         * Transferring more data to continue the operation with cmd=-1 does
         * not seem to work. Therefore a RAMWRC generated in this
         * case for further data */
        _cmd = (_cmd == LCD_CMD_RAMWR) ? LCD_CMD_RAMWRC : _cmd;
        _idx_bytes = 0;
    }
}

static void _lcd_ll_mcu_write_byte(lcd_t *dev, bool cont, uint8_t out)
{
    DEBUG("[lcd_ll_mcu] write byte: %02x\n", out);

    _data_bytes[_idx_bytes++] = out;
    /* transfer the data if necessary */
    _lcd_ll_mcu_transfer(dev, cont);
}

static uint8_t _lcd_ll_mcu_read_byte(lcd_t *dev, bool cont)
{
    LOG_ERROR("[lcd_ll_mcu] read from LCD is not supported\n");
    return 0;
}

#if IS_USED(MODULE_LCD_PARALLEL_16BIT)

static void _lcd_ll_mcu_write_word(lcd_t *dev, bool cont, uint16_t out)
{
    DEBUG("[lcd_ll_mcu] write word: %04x\n", out);

    /* out is given in BE order */
    _data_bytes[_idx_bytes++] = out >> 8;
    _data_bytes[_idx_bytes++] = out & 0xff;
    /* transfer the data if necessary */
    _lcd_ll_mcu_transfer(dev, cont);
}

static uint16_t _lcd_ll_mcu_read_word(lcd_t *dev, bool cont)
{
    LOG_ERROR("[lcd_ll_mcu] read from LCD is not supported\n");
    /* not supported yet */
    return 0;
}

#endif /* IS_USED(MODULE_LCD_PARALLEL_16BIT) */

const lcd_ll_par_driver_t lcd_ll_par_driver = {
    .init = _lcd_ll_mcu_init,
    .set_data_dir = _lcd_ll_mcu_set_data_dir,
    .cmd_start = _lcd_ll_mcu_cmd_start,
    .write_byte = _lcd_ll_mcu_write_byte,
    .read_byte = _lcd_ll_mcu_read_byte,
#if IS_USED(MODULE_LCD_PARALLEL_16BIT)
    .write_word = _lcd_ll_mcu_write_word,
    .read_word = _lcd_ll_mcu_read_word,
#endif
};

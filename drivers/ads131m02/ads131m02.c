/*
 * SPDX-FileCopyrightText: 2026 ML!PA Consulting GmbH
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     drivers_ads131m02
 * @{
 *
 * @file
 * @brief       ADS131M02 Analog-to-digital converter driver
 *
 * @author     Benjamin Valentin <benjamin.valentin@ml-pa.com>
 * @author     Fabian Hüßler <fabian.huessler@ml-pa.com>
 * @}
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>

#include "ads131m02.h"
#include "byteorder.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "time_units.h"
#include "xtimer.h"

#include "ads131m02_commands.h"
#include "ads131m02_registers.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#define SPI_PARAM(dev)          dev->params->spi, dev->params->cs_pin
#define SPI_ACQUIRE(dev)        spi_acquire(dev->params->spi, dev->params->cs_pin, \
                                            SPI_MODE_1, dev->params->spi_clk);
#define SPI_RELEASE(dev)        spi_release(dev->params->spi)

#define ADS131M02_DEVICE_ID     0x22
#define ADS131M02_RESET_DELAY   100
#define ADS131M02_POR_DELAY_US  300

#define ADS131M02_SPS(sps)      ((sps) >= 32000 ? 0 : \
                                 (sps) <= 125   ? 8 : \
                                 (31 - __builtin_clz(32000 / (sps))))

static void _read_regs(ads131m02_t *dev, uint8_t addr, uint16_t *dest, size_t numof)
{
    assert(numof >= 1);
    /* every SPI frame is always 4 words (response + CH0 + CH1 + CRC/zero),
     * regardless of the command - so both the command and the response
     * read must always transfer a full 12-byte frame. */
    uint8_t tx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};
    uint8_t rx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN];

    uint16_t cmd = ADS131M02_CMD_RREG(addr, numof);
    byteorder_htobebufs(&tx_buf[0], cmd);
    /* the response to a register command is only returned in the
     * *following* SPI frame, so CS must be deasserted between the
     * command and the response read to mark the frame boundary. */
    spi_transfer_bytes(SPI_PARAM(dev), false, &tx_buf, NULL, sizeof(tx_buf));
    if (numof == 1) {
        spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));
        /* the 16-bit register value is returned left-justified in the
        * 24-bit response word (upper two bytes), the last byte is zero
        * padding - only take the first two bytes to get the real value. */
        *dest = byteorder_bebuftohs(rx_buf);
    }
    else {
        numof += 1; /* ACK */
        uint16_t ack = 0;
        while(numof) {
            spi_transfer_bytes(SPI_PARAM(dev), numof > 4, NULL, rx_buf, sizeof(rx_buf));
            if (!ack) {
                ack = byteorder_bebuftohs(&rx_buf[0 * ADS131M02_WORD_LEN]);
                numof--;
            }
            else {
                *dest++ = byteorder_bebuftohs(&rx_buf[0 * ADS131M02_WORD_LEN]);
                numof--;
            }
            if (numof) {
                *dest++ = byteorder_bebuftohs(&rx_buf[1 * ADS131M02_WORD_LEN]);
                numof--;
            }
            if (numof) {
                *dest++ = byteorder_bebuftohs(&rx_buf[2 * ADS131M02_WORD_LEN]);
                numof--;
            }
            if (numof) {
                *dest++ = byteorder_bebuftohs(&rx_buf[3 * ADS131M02_WORD_LEN]);
                numof--;
            }
        }
    }
}

static uint16_t _read_reg(ads131m02_t *dev, uint8_t addr)
{
    uint16_t val;
    _read_regs(dev, addr, &val, 1);
    return val;
}

static void _write_regs(ads131m02_t *dev, uint8_t addr, uint16_t *src, size_t numof)
{
    uint8_t tx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};
    uint8_t rx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN];
    if (numof == 1) {
        byteorder_htobebufs(&tx_buf[0], ADS131M02_CMD_WREG(addr, numof));
        byteorder_htobebufs(&tx_buf[1 * ADS131M02_WORD_LEN], *src);
        spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, NULL, sizeof(tx_buf));
        spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));
    }
    else {
        uint16_t ack = 0;
        while (numof) {
            if (!ack) {
                byteorder_htobebufs(&tx_buf[0], ADS131M02_CMD_WREG(addr, numof));
                ack = 1;
            }
            else {
                byteorder_htobebufs(&tx_buf[0 * ADS131M02_WORD_LEN], *src++);
                numof--;
            }
            if (numof) {
                byteorder_htobebufs(&tx_buf[1 * ADS131M02_WORD_LEN], *src++);
                numof--;
            }
            if (numof) {
                byteorder_htobebufs(&tx_buf[2 * ADS131M02_WORD_LEN], *src++);
                numof--;
            }
            if (numof) {
                byteorder_htobebufs(&tx_buf[3 * ADS131M02_WORD_LEN], *src++);
                numof--;
            }
            /* same as above: the write confirmation is only returned in the
            * following SPI frame, and each frame is always a full 12 bytes. */
            spi_transfer_bytes(SPI_PARAM(dev), numof, tx_buf, NULL, sizeof(tx_buf));
        }
        /* get response */
        spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, rx_buf, sizeof(rx_buf));
        ack = byteorder_bebuftohs(rx_buf);
    }
}

static void _write_reg(ads131m02_t *dev, uint8_t addr, uint16_t val)
{
    _write_regs(dev, addr, &val, 1);
}

static int _ads131m02_reset(ads131m02_t *dev)
{
    int res;
    uint8_t tx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};
    uint8_t rx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN];

    byteorder_htobebufs(tx_buf, ADS131M02_CMD_RESET);
    /* the RESET command is not latched by the device until the entire
     * (4-word/12-byte) frame is complete - sending a shorter frame
     * makes the device respond with 0x0011 instead of resetting. */
    spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, NULL, sizeof(tx_buf));
    /* the device needs tREGACQ (up to ~4ms @ 8.192 MHz CLKIN) to reload
     * its registers to their default values before the RESET
     * acknowledgment (and any further communication) is valid. */
    xtimer_msleep(ADS131M02_RESET_DELAY);
    /* clock a NULL command to read back the RESET acknowledgment,
     * which is returned in the frame following the RESET command. */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    res = byteorder_bebuftohs(rx_buf);

    if (res != ADS131M02_RSP_RESET) {
        return -EIO;
    }

    return 0;
}

static int _ads131m02_standby(ads131m02_t *dev)
{
    int res;
    uint8_t tx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};
    uint8_t rx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};

    byteorder_htobebufs(tx_buf, ADS131M02_CMD_STANDBY);

    spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, NULL, sizeof(tx_buf));
    /* clock a NULL command to read back the STANDBY acknowledgment,
     * which is returned in the frame following the STANDBY command. */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    res = byteorder_bebuftohs(rx_buf);
    if (res != ADS131M02_RSP_STANDBY) {
        return -EIO;
    }

    dev->standby = true;
    board_ads131m02_clk_ctrl(dev, false);
    return 0;
}

static int _ads131m02_wakeup(ads131m02_t *dev)
{
    int res;
    uint8_t tx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN] = {0};
    uint8_t rx_buf[ADS131M02_FRAME_WORDS * ADS131M02_WORD_LEN];

    board_ads131m02_clk_ctrl(dev, true);

    byteorder_htobebufs(tx_buf, ADS131M02_CMD_WAKEUP);

    spi_transfer_bytes(SPI_PARAM(dev), false, tx_buf, NULL, sizeof(tx_buf));
    /* clock a NULL command to read back the WAKEUP acknowledgment,
     * which is returned in the frame following the WAKEUP command. */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, rx_buf, sizeof(rx_buf));

    res = byteorder_bebuftohs(rx_buf);
    if (res != ADS131M02_RSP_WAKEUP) {
        return -EIO;
    }

    dev->standby = false;
    return 0;
}

static void _ads131m02_sync(ads131m02_t *dev)
{
    gpio_clear(dev->params->sync_pin);
    /* Hold low for 1024 (< 2048) clock cycles assuming fastest clock.
       For slower clocks, same duration results into less clock cycles,
       but still less than 2048. */
    xtimer_usleep(1024 * 1000 / 8192);
    gpio_set(dev->params->sync_pin);
}

static int32_t _ads131m02_word_to_int32(const uint8_t *p_word, ads131m02_wlength_t wlen)
{
    if (wlen == ads131m02_wlength_16bit) {
        int16_t raw16 = (int16_t)((uint16_t)p_word[0] << 8 | (uint16_t)p_word[1]);
        return ((int32_t)raw16) << 8;
    }
    else if (wlen == ads131m02_wlength_24bit || wlen == ads131m02_wlength_32bit_zp) {
        int32_t val24 = ((int32_t)p_word[0] << 16) |
                        ((int32_t)p_word[1] << 8)  |
                         (int32_t)p_word[2];
        /* Sign-extend from bit 23 to bit 31 if negative */
        if (val24 & 0x00800000) {
            val24 |= 0xFF000000;
        }
        return val24;
    }
    else if (wlen == ads131m02_wlength_32bit_se) {
        return (int32_t)byteorder_bebuftohl(p_word);
    }
    return 0; /* fallback */
}

static int32_t _ads131m02_int32_scale_nv(int32_t sample, uint8_t gain_div)
{
    int32_t fsr_nv_pos = ADS131M02_VREF_NV / gain_div;
    int32_t fsr_nv_neg = -fsr_nv_pos;
    if (sample >= 0) {
        /* 24 bit signed maximum 0x7fffff (8,388,607) */
        return (int32_t)((int64_t)sample * fsr_nv_pos / 8388607LL);
    }
    else {
        /* 24 bit signed minimum 0x800000 (-8,388,608) */
        return (int32_t)(((int64_t)sample * fsr_nv_neg) / -8388608LL);
    }
}

static void _dma_tcmpl_tx(void *ctx)
{
    ads131m02_t *dev = (ads131m02_t *)ctx;
    (void)dev;
#if MODULE_ADS131M02_IRQ
    gpio_set(dev->params->cs_pin); /* frame finished */
#endif
}

static void _dma_tcmpl_rx(void *ctx)
{
    ads131m02_t *dev = (ads131m02_t *)ctx;
    if (dev->drdy) {
        dev->drdy(dev, dev->buf);
    }
    if (++dev->buf >= dev->frame_bufs + dev->frame_bufs_numof) {
        dev->buf = dev->frame_bufs;
    }
    spi_dma_set_desc_rx(dev->params->spi, dev->buf->desc);
    spi_dma_start_rx(dev->params->spi);
}

static void _dma_setup(ads131m02_t *dev)
{
    spi_dma_setup(dev->params->spi,
                  spi_config[dev->params->spi].rx_trigger, DMA_TRIGACT_BURST, _dma_tcmpl_rx, dev,
#if MODULE_ADS131M02_EVENT
                  0, DMA_TRIGACT_BLOCK
#else
                  spi_config[dev->params->spi].tx_trigger, DMA_TRIGACT_BURST,
#endif
                  _dma_tcmpl_tx, dev);

    spi_dma_prepare(dev->params->spi, NULL, NULL, sizeof(ads131m02_frame_t),
                    DMA_BLOCKACT_NONE, DMA_BLOCKACT_NONE);
    spi_dma_set_desc_rx(dev->params->spi, dev->buf->desc);
}

static void _dma_cancel(ads131m02_t *dev)
{
    dma_cancel(spi_dma_tx(dev->params->spi));
    dma_cancel(spi_dma_rx(dev->params->spi));
}

static void _drdy_enable(ads131m02_t *dev)
{
#if MODULE_ADS131M02_IRQ
    gpio_irq_enable(dev->params->ready_pin);
#else /* ADS131M02_EVENT */
    gpio_event_enable(dev->params->ready_pin);
#endif
}

static void _drdy_disable(ads131m02_t *dev)
{
#if MODULE_ADS131M02_IRQ
    gpio_irq_disable(dev->params->ready_pin);
#else /* ADS131M02_EVENT */
    gpio_event_disable(dev->params->ready_pin);
#endif
}

#if MODULE_ADS131M02_IRQ
static void _drdy(void *ctx)
{
    ads131m02_t *dev = (ads131m02_t *)ctx;
    gpio_clear(dev->params->cs_pin); /* start frame */
    spi_dma_start_tx(dev->params->spi);
}
#endif

static int _drdy_init(ads131m02_t *dev)
{
#if MODULE_ADS131M02_IRQ
    return gpio_init_int(dev->params->ready_pin, GPIO_IN, GPIO_FALLING, _drdy, dev);
#else /* ADS131M02_EVENT */
    return gpio_init_event(dev->params->ready_pin, GPIO_FALLING);
#endif
}

static void _event_setup(ads131m02_t *dev)
{
#if MODULE_ADS131M02_EVENT
    dma_event_use(spi_dma_tx(dev->params->spi), dev->event);
    gpio_event_gen(dev->params->ready_pin, dev->event,
                   EVENT_PATH_SYNCHRONOUS,
                   EVENT_EDGE_FALLING);
    dma_event_setup(spi_dma_tx(dev->params->spi), DMA_EVACT_TRIG);
    dma_event_input_enable(spi_dma_tx(dev->params->spi));
#endif
    (void)dev;
}

static void _event_teardown(ads131m02_t *dev)
{
#if MODULE_ADS131M02_EVENT
    dma_event_input_disable(spi_dma_tx(dev->params->spi));
    dma_event_disuse(spi_dma_tx(dev->params->spi), dev->event);
#endif
    (void)dev;
}

static void _ads131m02_mux(ads131m02_t *dev, ads131m02_channel_t ch, ads131m02_mux_t mux)
{
    uint16_t addr = ADS131M02_REG_CH0_CFG +
                    ((ADS131M02_REG_CH1_CFG - ADS131M02_REG_CH0_CFG) *
                    ADS131M02_CHANNEL(ch));
    uint16_t reg = _read_reg(dev, addr);
    reg &= ~(ADS131M02_CH0_CFG_MUX_MASK);
    reg |= mux;
    _write_reg(dev, addr, reg);
}

void __attribute__((weak)) board_ads131m02_clk_ctrl(ads131m02_t *dev, bool on)
{
    (void)dev;
    (void)on;
}

void ads131m02_frame_buffer_alloc(ads131m02_t *dev, ads131m02_frame_buffer_t *frame_buf)
{
    unsigned frames_numof = dev->frame_bufs[1].frames - dev->frame_bufs[0].frames;
    frame_buf->desc = spi_dma_get_desc(dev->params->spi);
    spi_dma_prepare_desc(dev->params->spi, frame_buf->desc, NULL,
                         NULL, frame_buf->frames[0].f, sizeof(frame_buf->frames[0].f),
                         DMA_BLOCKACT_NONE, DMA_BLOCKACT_NONE);
    for (unsigned i = 1; i < frames_numof; ++i) {
        void *next = spi_dma_get_desc(dev->params->spi);
        assert(next);
        spi_dma_prepare_desc(dev->params->spi, next, NULL,
                             NULL, frame_buf->frames[i].f, sizeof(frame_buf->frames[i].f),
                             DMA_BLOCKACT_NONE, DMA_BLOCKACT_NONE);
        dma_append_descriptor(frame_buf->desc, next);
    }
}

void ads131m02_frame_buffer_free(ads131m02_t *dev, ads131m02_frame_buffer_t *frame_buf)
{
    const void *free = frame_buf->desc;
    const void *next;
    do {
        next = dma_get_next_descriptor(free);
        spi_dma_put_desc(dev->params->spi, free);
        free = next;
    } while (next);
    frame_buf->desc = NULL;
}

int ads131m02_init(ads131m02_t *dev, const ads131m02_params_t *params,
                   const ads131m02_init_t *init)
{
    board_ads131m02_clk_ctrl(dev, false);
    int res;
    memset(dev, 0, sizeof(*dev));
    dev->params = params;
    dev->frame_bufs = init->frame_bufs;
    dev->frame_bufs_numof = init->numof;
    if ((res = spi_init_cs(params->spi, params->cs_pin))) {
        return res;
    }
    if (gpio_is_valid(dev->params->sync_pin)) {
        gpio_set(dev->params->sync_pin);
        if ((res = gpio_init(dev->params->sync_pin, GPIO_OUT))) {
            return res;
        }
    }
    if (gpio_is_valid(dev->params->ready_pin)) {
        _drdy_disable(dev);
    }

    board_ads131m02_clk_ctrl(dev, true);
    xtimer_usleep(ADS131M02_POR_DELAY_US);
    SPI_ACQUIRE(dev);
    res = _read_reg(dev, ADS131M02_REG_ID);
    /* the upper byte of the ID register holds the fixed device ID,
     * the lower byte is reserved/revision info */
    if ((res >> 8) != ADS131M02_DEVICE_ID) {
        board_ads131m02_clk_ctrl(dev, false);
        DEBUG("ads131m02: Device ID mismatch %x\n", res);
        res = -EINVAL;
        goto out;
    }

    res = _ads131m02_reset(dev);
    if (res) {
        board_ads131m02_clk_ctrl(dev, false);
        DEBUG("ads131m02: Device reset failed %d\n", res);
        goto out;
    }
    /* default clock register with ADC disabled */
    _write_reg(dev, ADS131M02_REG_CLOCK,
               ADS131M02_CLOCK_OSR_1024 |
               ADS131M02_CLOCK_PWR_HIGH_RES);
    /* default mode register with DRDY pullup configuration */
    _write_reg(dev, ADS131M02_REG_MODE,
               ADS131M02_MODE_RESET_MASK |
               ADS131M02_MODE_WLENGTH_24BIT |
               ADS131M02_MODE_TIMEOUT_MASK |
               (dev->params->ready_pin_pullup ? ADS131M02_MODE_DRDY_HiZ_MASK : 0));

    /* DRDY is active low - a new conversion result is signaled by a
     * high-to-low transition. (The one-time low-to-high transition
     * after reset, which indicates the SPI interface being ready, is
     * not relied upon here - we wait out tREGACQ instead, see
     * ads131m02_device_reset()) */
    if (gpio_is_valid(dev->params->ready_pin)) {
        if ((res = _drdy_init(dev))) {
            board_ads131m02_clk_ctrl(dev, false);
            goto out;
        }
        _drdy_disable(dev);
    }
#if MODULE_ADS131M02_EVENT
    dev->event = periph_event_channel_alloc();
#endif
    _event_setup(dev);
out:
    SPI_RELEASE(dev);
    return res;
}

int ads131m02_start(ads131m02_t *dev, ads131m02_start_t *start, uint32_t f_clkin_hz,
                    ads131m02_drdy_cb_t drdy)
{
    uint16_t clock = 0;
    uint16_t clock_mask = ADS131M02_CLOCK_PWR_MASK | ADS131M02_CLOCK_OSR_MASK;
    /* Table 8-2. OSR Settings and Data Rates for Nominal Master Clock Frequencies */
    if (f_clkin_hz == 8192000) {
        clock |= ADS131M02_CLOCK_PWR_HIGH_RES;
        if (start->sps >= 32000) {
            start->sps = 32000;
            clock |= ADS131M02_CLOCK_OSR_128;
        }
        else if (start->sps >= 16000) {
            start->sps = 16000;
            clock |= ADS131M02_CLOCK_OSR_256;
        }
        else if (start->sps >= 8000) {
            start->sps = 8000;
            clock |= ADS131M02_CLOCK_OSR_512;
        }
        else if (start->sps >= 4000) {
            start->sps = 4000;
            clock |= ADS131M02_CLOCK_OSR_1024;
        }
        else if (start->sps >= 2000) {
            start->sps = 2000;
            clock |= ADS131M02_CLOCK_OSR_2048;
        }
        else if (start->sps >= 1000) {
            start->sps = 1000;
            clock |= ADS131M02_CLOCK_OSR_4096;
        }
        else if (start->sps >= 500) {
            start->sps = 500;
            clock |= ADS131M02_CLOCK_OSR_8192;
        }
        else {
            /* lowest possible data rate */
            start->sps = 250;
            clock |= ADS131M02_CLOCK_OSR_16384;
        }
    }
    else if (f_clkin_hz == 4096000) {
        clock |= ADS131M02_CLOCK_PWR_LOW;
        if (start->sps >= 16000) {
            start->sps = 16000;
            clock |= ADS131M02_CLOCK_OSR_128;
        }
        else if (start->sps >= 8000) {
            start->sps = 8000;
            clock |= ADS131M02_CLOCK_OSR_256;
        }
        else if (start->sps >= 4000) {
            start->sps = 4000;
            clock |= ADS131M02_CLOCK_OSR_512;
        }
        else if (start->sps >= 2000) {
            start->sps = 2000;
            clock |= ADS131M02_CLOCK_OSR_1024;
        }
        else if (start->sps >= 1000) {
            start->sps = 1000;
            clock |= ADS131M02_CLOCK_OSR_2048;
        }
        else if (start->sps >= 500) {
            start->sps = 500;
            clock |= ADS131M02_CLOCK_OSR_4096;
        }
        else if (start->sps >= 250) {
            start->sps = 250;
            clock |= ADS131M02_CLOCK_OSR_8192;
        }
        else {
            /* lowest possible data rate */
            start->sps = 125;
            clock |= ADS131M02_CLOCK_OSR_16384;
        }
    }
    else if (f_clkin_hz == 2048000) {
        clock |= ADS131M02_CLOCK_PWR_VERY_LOW;
        if (start->sps >= 8000) {
            start->sps = 8000;
            clock |= ADS131M02_CLOCK_OSR_128;
        }
        else if (start->sps >= 4000) {
            start->sps = 4000;
            clock |= ADS131M02_CLOCK_OSR_256;
        }
        else if (start->sps >= 2000) {
            start->sps = 2000;
            clock |= ADS131M02_CLOCK_OSR_512;
        }
        else if (start->sps >= 1000) {
            start->sps = 1000;
            clock |= ADS131M02_CLOCK_OSR_1024;
        }
        else if (start->sps >= 500) {
            start->sps = 500;
            clock |= ADS131M02_CLOCK_OSR_2048;
        }
        else if (start->sps >= 250) {
            start->sps = 250;
            clock |= ADS131M02_CLOCK_OSR_4096;
        }
        else {
            /* lowest possible data rate */
            start->sps = 125;
            clock |= ADS131M02_CLOCK_OSR_8192;
        }
    }
    else {
        return -EINVAL;
    }
    uint16_t gain = 0;
    uint16_t gain_mask = 0;
    for (ads131m02_channel_t ch = 0; ch < ADS131M02_CHANNELS_NUMOF; ++ch) {
        if (!(start->ch_mask & ADS131M02_CHANNEL_MASK(ch))) {
            continue;
        }
        if (start->ch[ch].abs_mv > (ADS131M02_VREF_NV / 1000 / 1000)) {
            return -EINVAL; /* prevents overflow */
        }
        uint8_t div = 1;
        while (div) { /* (uint8_t)256 == 0 */
            if (ADS131M02_VREF_NV / (div << 1)
                < ((uint32_t)start->ch[ch].abs_mv) * 1000u * 1000u) {
                break;
            }
            div <<= 1;
        }
        if (div == 0) {
            return -EINVAL;
        }
        dev->gain[ch] = div;
        gain |= (ADS131M02_GAIN(dev->gain[ch]) << ADS131M02_GAIN_SHIFT(ch));
        gain_mask |= (ADS131M02_GAIN_PGAGAIN0_MASK << ADS131M02_GAIN_SHIFT(ch));
        clock |= (ADS131M02_CLOCK_CH0_EN_MASK << ch);
        clock_mask |= (ADS131M02_CLOCK_CH0_EN_MASK << ch);
    }
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    _drdy_disable(dev);
    dev->drdy = drdy;
    dev->buf = &dev->frame_bufs[0];
    dev->active = true;
    _write_reg(dev, ADS131M02_REG_GAIN,
            (_read_reg(dev, ADS131M02_REG_GAIN) & ~gain_mask) | gain);
    _write_reg(dev, ADS131M02_REG_CLOCK,
            (_read_reg(dev, ADS131M02_REG_CLOCK) & ~clock_mask) | clock);
    spi_acquire_dma(dev->params->spi);
    _dma_setup(dev);
    spi_dma_start_rx(dev->params->spi);
#if MODULE_ADS131M02_EVENT
    gpio_clear(dev->params->cs_pin); /* start frame */
    spi_dma_start_tx(dev->params->spi);
#endif
    _drdy_enable(dev);
    _ads131m02_sync(dev);
    return 0;
}

int ads131m02_pause(ads131m02_t *dev, const ads131m02_pause_t *pause)
{
    uint16_t clock_mask = 0;
    for (ads131m02_channel_t ch = 0; ch < ADS131M02_CHANNELS_NUMOF; ++ch) {
        if (!(pause->ch_mask & ADS131M02_CHANNEL_MASK(ch))) {
            continue;
        }
        clock_mask |= (ADS131M02_CLOCK_CH0_EN_MASK << ch);
    }
    if (dev->standby) {
        return -ECANCELED;
    }
    _drdy_disable(dev); /* stop the event */
    _dma_cancel(dev); /* cancel any ongoing DMA transfers */
    spi_release_dma(dev->params->spi); /* SPI still acquired, just dont use DMA */
    _write_reg(dev, ADS131M02_REG_CLOCK, _read_reg(dev, ADS131M02_REG_CLOCK) & ~clock_mask);
    return 0;
}

int ads131m02_resume(ads131m02_t *dev, const ads131m02_resume_t *resume)
{
    uint16_t clock_mask = 0;
    for (ads131m02_channel_t ch = 0; ch < ADS131M02_CHANNELS_NUMOF; ++ch) {
        if (!(resume->ch_mask & ADS131M02_CHANNEL_MASK(ch))) {
            continue;
        }
        clock_mask |= (ADS131M02_CLOCK_CH0_EN_MASK << ch);
    }
    if (dev->standby) {
        return -ECANCELED;
    }
    _write_reg(dev, ADS131M02_REG_CLOCK, _read_reg(dev, ADS131M02_REG_CLOCK) | clock_mask);
    spi_acquire_dma(dev->params->spi); /* use DMA again */
    _dma_setup(dev); /* reconfigure first DMA descriptor */
    spi_dma_start_rx(dev->params->spi);
#if MODULE_ADS131M02_EVENT
    gpio_clear(dev->params->cs_pin); /* start frame */
    spi_dma_start_tx(dev->params->spi);
#endif
    _drdy_enable(dev); /* reenable the event */
    _ads131m02_sync(dev); /* synchronize the device */
    return 0;
}

int ads131m02_stop(ads131m02_t *dev)
{
    if (dev->standby) {
        return -ECANCELED;
    }
    /* stop all */
    const ads131m02_pause_t stop = {
        .ch_mask = (unsigned)(-1),
    };
    /* keeping track of active sampling does not stall the SPI bus,
       when stop is called befor start */
    if (dev->active) {
        ads131m02_pause(dev, &stop); /* pause channels */
        _drdy_disable(dev); /* stop the event */
        _dma_cancel(dev); /* cancel any ongoing DMA transfers */
        spi_release_dma(dev->params->spi); /* release DMA */
        _event_teardown(dev); /* teardown the event */
        dev->active = false;
        SPI_RELEASE(dev); /* release the SPI bus */
    }
    return 0;
}

ads131m02_channel_mask_t ads131m02_parse_frame(ads131m02_t *dev, int32_t chan[], bool nanovolt,
                                               ads131m02_frame_t *frame)
{
    /* the STATUS register (16 bit) is returned left-justified in the
     * 24-bit response word, i.e. in its upper two bytes */
    unsigned chan_numof = ADS131M02_CHANNELS_NUMOF;
    uint16_t status = byteorder_bebuftohs(&frame->f[0 * ADS131M02_WORD_LEN]);
    for (unsigned i = 0; i < chan_numof; i++) {
        if (chan[i] && (status & (ADS131M02_STATUS_DRDY0_MASK << i))) {
            chan[i] = _ads131m02_word_to_int32(&frame->f[(i + 1) * ADS131M02_WORD_LEN],
                                               ads131m02_wlength_24bit);
            if (nanovolt) {
                chan[i] = _ads131m02_int32_scale_nv(chan[i], dev->gain[i]);
            }
        }
    }
    return ((status & ADS131M02_STATUS_DRDY0_MASK) ? ads131m02_channel_mask_0 : 0) |
           ((status & ADS131M02_STATUS_DRDY1_MASK) ? ads131m02_channel_mask_1 : 0);
}

static ads131m02_channel_mask_t  _read_sample(ads131m02_t *dev, int32_t chan[], bool nanovolt)
{
    ads131m02_frame_t frame;
    /* clock out a NULL command while reading back the previous
     * conversion's status + channel data in the same full-duplex frame */
    spi_transfer_bytes(SPI_PARAM(dev), false, NULL, frame.f, sizeof(frame.f));
    return ads131m02_parse_frame(dev, chan, nanovolt, &frame);
}

int ads131m02_sample(ads131m02_t *dev, int32_t chan[], bool nanovolt)
{
    /* The internal mechanism that outputs data contains a first-in-first-out (FIFO)
       buffer that can store two samples of data per channel at a time. The DRDY flag
       for each channel in the STATUS register remains set until both samples for each
       channel are read from the device. This condition is not obvious under normal
       circumstances when the host is reading each consecutive sample from the device.
       In that case, the samples are cleared from the device each time new data are
       generated so the DRDY flag for each channel in the STATUS register is cleared
       with each read. However, both slots of the FIFO are full if a sample is missed
       or if data are not read for a period of time. Either strobe the SYNC/RESET pin
       to re-synchronize conversions and clear the FIFOs, or quickly read two data
       packets when data are read for the first time or after a gap in reading data.
       This process ensures predictable DRDY pin behavior. See the Synchronization
       section for information about the synchronization feature. */

    /* using ADS131M02_CHANNELS_NUMOF is a simplification because only m02 is supported so far */
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    ads131m02_channel_mask_t status = _read_sample(dev, chan, nanovolt);
    SPI_RELEASE(dev);
    return status;
}

int ads131m02_standby(ads131m02_t *dev)
{
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    int ret = _ads131m02_standby(dev);
    SPI_RELEASE(dev);
    return ret;
}

int ads131m02_wakeup(ads131m02_t *dev)
{
    SPI_ACQUIRE(dev);
    if (!dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    int ret = _ads131m02_wakeup(dev);
    SPI_RELEASE(dev);
    return ret;
}

int ads131m02_mux_test_signal_positive(ads131m02_t *dev, ads131m02_channel_t ch)
{
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    _ads131m02_mux(dev, ch, ads131m02_mux_pos_dc);
    SPI_RELEASE(dev);
    return 0;
}

int ads131m02_mux_test_signal_negative(ads131m02_t *dev, ads131m02_channel_t ch)
{
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    _ads131m02_mux(dev, ch, ads131m02_mux_neg_dc);
    SPI_RELEASE(dev);
    return 0;
}

int ads131m02_mux_ain(ads131m02_t *dev, ads131m02_channel_t ch)
{
    SPI_ACQUIRE(dev);
    if (dev->standby) {
        SPI_RELEASE(dev);
        return -ECANCELED;
    }
    _ads131m02_mux(dev, ch, ads131m02_mux_ain0p_ain0n);
    SPI_RELEASE(dev);
    return 0;
}

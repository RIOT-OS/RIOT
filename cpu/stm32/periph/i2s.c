/*
 * Copyright (C) 2023 Freie Universität Berlin
 *               2023 Koen Zandberg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_stm32
 * @ingroup     drivers_periph_i2s
 * @{
 *
 * @file
 * @brief       Low-level i2s driver implementation
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @}
 */

#include "macros/utils.h"
#include "cpu.h"
#include "irq.h"
#include "byteorder.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/i2s.h"
#include "periph/pm.h"
#include "clist.h"

typedef struct {
    i2s_event_cb_t event_callback;
    i2s_transaction_cb_t transaction_callback;
    void *arg;
    clist_node_t transactions;
    i2s_transaction_t *current_transaction;
    size_t transaction_bytes;
    size_t frames_transmitted;
    uint8_t bytes_per_frame;
    bool clock_state;
} i2s_runtime_state_t;

static i2s_runtime_state_t runtime_state[I2S_NUMOF];

static void _dma_transfer_callback(dma_t dma, void *arg);

static inline SPI_TypeDef *periph(i2s_t dev)
{
    return i2s_config[dev].dev;
}

static inline i2s_runtime_state_t *state(i2s_t dev)
{
    return &runtime_state[dev];
}

static bool _enable_clock(i2s_t dev)
{
    bool previous_state = state(dev)->clock_state;
    pm_block(STM32_PM_STOP);
    periph_clk_en(i2s_config[dev].apbbus, i2s_config[dev].rccmask);
    return previous_state;
}

static void _restore_clock(i2s_t dev, bool previous_state)
{
    if (!previous_state) {
        periph_clk_dis(i2s_config[dev].apbbus, i2s_config[dev].rccmask);
        pm_unblock(STM32_PM_STOP);
    }
    state(dev)->clock_state = previous_state;
}

int i2s_init(i2s_t dev, i2s_event_cb_t event_callback, i2s_transaction_cb_t transaction_callback, void *arg)
{
    if (dev >= I2S_NUMOF) {
        return I2S_NODEV;
    }
    i2s_runtime_state_t *rstate = state(dev);
    rstate->event_callback = event_callback;
    rstate->transaction_callback = transaction_callback;
    rstate->arg = arg;
    return I2S_OK;
}

static void _config_gpio(i2s_t dev, i2s_mode_t mode, i2s_direction_t dir)
{
    if (mode == I2S_MODE_CONTROLLER) {
        gpio_init(i2s_config[dev].bclk_pin, GPIO_OUT);
        gpio_init(i2s_config[dev].wclk_pin, GPIO_OUT);
        if (gpio_is_valid(i2s_config[dev].mclk_pin)) {
            gpio_init(i2s_config[dev].mclk_pin, GPIO_OUT);
            gpio_init_af(i2s_config[dev].mclk_pin, i2s_config[dev].mclk_af);
        }
    }
    else {
        gpio_init(i2s_config[dev].bclk_pin, GPIO_IN);
        gpio_init(i2s_config[dev].wclk_pin, GPIO_IN);
    }

    if (dir == I2S_DIRECTION_TRANSMIT) {
        gpio_init(i2s_config[dev].data_pin, GPIO_OUT);
    }
    else {
        gpio_init(i2s_config[dev].data_pin, GPIO_IN);
    }

    gpio_init_af(i2s_config[dev].wclk_pin, i2s_config[dev].wclk_af);
    gpio_init_af(i2s_config[dev].bclk_pin, i2s_config[dev].bclk_af);
    gpio_init_af(i2s_config[dev].data_pin, i2s_config[dev].data_af);

}

static uint32_t _i2scfg(i2s_mode_t mode, i2s_direction_t dir)
{
    return ((mode == I2S_MODE_CONTROLLER ? 1 : 0) << 1 |
        (dir ==  I2S_DIRECTION_RECEIVE ? 1 : 0))
        << SPI_I2SCFGR_I2SCFG_Pos;
}

static uint32_t _i2sdatlen(uint8_t length)
{
    switch(length) {
        case 16:
            return 0;
        case 24:
            return 1 << SPI_I2SCFGR_DATLEN_Pos;
        case 32:
            return 2 << SPI_I2SCFGR_DATLEN_Pos;
    }
    assert (false);
    return 0;
}

static uint8_t _datlen2bits(uint32_t reg)
{
    uint8_t datlen = (reg & SPI_I2SCFGR_DATLEN_Msk) >> SPI_I2SCFGR_DATLEN_Pos;
    switch(datlen) {
        case 0:
            return 16;
        case 1:
            return 24;
        case 2:
            return 32;
    }
    assert (false);
    return 0;
}

static i2s_direction_t _reg2dir(uint32_t cfgr)
{
    return (cfgr & SPI_I2SCFGR_I2SCFG_0) ? I2S_DIRECTION_TRANSMIT : I2S_DIRECTION_RECEIVE;
}

static i2s_mode_t _reg2mode(uint32_t cfgr)
{
    return (cfgr & SPI_I2SCFGR_I2SCFG_1) ? I2S_MODE_CONTROLLER : I2S_MODE_TARGET;
}

static void _set_clock_config(i2s_t dev, uint32_t frame_clock, uint8_t frame_length)
{
    uint32_t reg_value = 0;
    if (gpio_is_valid(i2s_config[dev].mclk_pin)) {
        uint32_t divisor = i2s_config[dev].input_clock / (256 * frame_clock);
        reg_value = SPI_I2SPR_MCKOE |
            ((divisor & 1) << SPI_I2SPR_ODD_Pos) |
             (divisor/2 & SPI_I2SPR_I2SDIV_Msk);
    }
    else {
        uint32_t divisor = i2s_config[dev].input_clock / (frame_length * frame_clock);
        reg_value =
            ((divisor & 1) << SPI_I2SPR_ODD_Pos) |
             (divisor/2 & SPI_I2SPR_I2SDIV_Msk);
    }
    periph(dev)->I2SPR = reg_value;
}

static uint32_t _get_clock_config(i2s_t dev, uint8_t frame_length)
{
    uint32_t reg_value = periph(dev)->I2SPR;
    uint32_t input_clock = i2s_config[dev].input_clock;
    uint32_t divisor = (reg_value & SPI_I2SPR_I2SDIV_Msk) * 2 +
            ((reg_value & SPI_I2SPR_ODD_Msk) ? 1 : 0);
    if (divisor == 0) {
        return 0;
    }
    if (reg_value & SPI_I2SPR_MCKOE) {
        return input_clock / (256 * divisor);
    }
    else {
        return input_clock / (2 * frame_length * divisor);
    }
}

int i2s_configure(i2s_t dev, const i2s_config_t *config)
{
    if (dev >= I2S_NUMOF) {
        return I2S_NODEV;
    }

    if (config->direction != I2S_DIRECTION_RECEIVE &&
            config->direction != I2S_DIRECTION_TRANSMIT) {
        return I2S_NODIR;
    }


    uint8_t data_length = config->word_size;
    uint8_t frame_length = (config->word_size > 16 ? 32 : 16) * 2;

    if (data_length != 16 && data_length != 24 && data_length != 32) {
        return I2S_NOWIDTH;
    }
    if (config->channels != 2) {
        return I2S_NOCHANNEL;
    }


    bool clk = _enable_clock(dev);

    periph(dev)->I2SCFGR =
        SPI_I2SCFGR_I2SMOD |
        _i2scfg(config->mode, config->direction) |
        _i2sdatlen(data_length) |
        (data_length > 16 ? 1 : 0) << SPI_I2SCFGR_CHLEN_Pos;

    if (config->mode == I2S_MODE_CONTROLLER) {
        _set_clock_config(dev, config->frame_clock, frame_length);
    }

    if (config->direction == I2S_DIRECTION_RECEIVE) {
        periph(dev)->CR2 = SPI_CR2_RXDMAEN;
    }
    else {
        periph(dev)->CR2 = SPI_CR2_TXDMAEN;
    }

    state(dev)->bytes_per_frame = i2s_bytes_per_frame(frame_length, config->channels);

    _restore_clock(dev, clk);
    return I2S_OK;
}

int i2s_get_configuration(i2s_t dev, i2s_config_t *config)
{
    bool clk = _enable_clock(dev);
    uint32_t cfgr = periph(dev)->I2SCFGR;
    uint8_t frame_length = (cfgr & SPI_I2SCFGR_CHLEN) ? 64 : 32;
    uint8_t data_length = _datlen2bits(cfgr);

    config->mode = (cfgr & SPI_I2SCFGR_I2SCFG_1) ? I2S_MODE_CONTROLLER : I2S_MODE_TARGET;
    config->direction = _reg2dir(cfgr);
    config->word_size = data_length;
    config->frame_clock = _get_clock_config(dev, frame_length);
    config->channels = 2;
    config->transaction_bytes = state(dev)->transaction_bytes;

    _restore_clock(dev, clk);
    return I2S_OK;
}

uint32_t i2s_get_frame_clock(i2s_t dev)
{
    bool clk = _enable_clock(dev);
    uint8_t frame_length = (periph(dev)->I2SCFGR & SPI_I2SCFGR_CHLEN) ? 64 : 32;
    uint32_t frame_clock = _get_clock_config(dev, frame_length);
    _restore_clock(dev, clk);
    return frame_clock;
}

void _start_transaction(i2s_t dev)
{
    i2s_transaction_t *transaction = container_of(clist_lpop(&state(dev)->transactions), i2s_transaction_t, node);
    state(dev)->current_transaction = transaction;
    dma_prepare(i2s_config[dev].dma, transaction->buf, state(dev)->transaction_bytes / 2, 1);
    dma_start(i2s_config[dev].dma);
}

void _append_transaction(i2s_t dev)
{
    i2s_transaction_t *transaction = container_of(clist_lpop(&state(dev)->transactions), i2s_transaction_t, node);
    state(dev)->current_transaction = transaction;
    transaction = container_of(clist_lpeek(&state(dev)->transactions), i2s_transaction_t, node);
    dma_double_buffer_set_other(i2s_config[dev].dma, transaction->buf);
}

int i2s_start(i2s_t dev)
{
    _enable_clock(dev);

    clist_node_t *transactions = &(state(dev)->transactions);

    if (clist_is_empty(transactions)) {
        return I2S_NODATA;
    }

    /* Clear out data register just in case */
    uint16_t tmp = periph(dev)->DR;
    (void)tmp;

    state(dev)->frames_transmitted = 0;

    periph(dev)->CR2 |= SPI_CR2_ERRIE;
    NVIC_EnableIRQ(i2s_config[dev].irqn);
    uint32_t cfgr = periph(dev)->I2SCFGR;
    i2s_direction_t dir = _reg2dir(cfgr);
    i2s_mode_t mode = _reg2mode(cfgr);

    dma_acquire(i2s_config[dev].dma);
    dma_mode_t dma_mode = dir == I2S_DIRECTION_RECEIVE ? DMA_PERIPH_TO_MEM : DMA_MEM_TO_PERIPH;
    dma_setup_full(i2s_config[dev].dma, i2s_config[dev].dma_chan, (uint32_t*)&(periph(dev)->DR), dma_mode, DMA_DATA_WIDTH_WORD, DMA_DATA_WIDTH_HALF_WORD, 0);
    dma_double_buffer_enable(i2s_config[dev].dma);
    dma_set_callback(i2s_config[dev].dma, _dma_transfer_callback, (void*)(intptr_t)dev);
    _start_transaction(dev);

    i2s_transaction_t *transaction = container_of(clist_lpeek(&state(dev)->transactions), i2s_transaction_t, node);
    dma_double_buffer_set_other(i2s_config[dev].dma, transaction->buf);

    periph(dev)->I2SCFGR |= SPI_I2SCFGR_I2SE;
    _config_gpio(dev, mode, dir);

    return I2S_OK;
}

void i2s_stop(i2s_t dev)
{
    dma_stop(i2s_config[dev].dma);
    dma_release(i2s_config[dev].dma);
    periph(dev)->I2SCFGR &= ~SPI_I2SCFGR_I2SE;
    _restore_clock(dev, false);
}

void i2s_add_transaction(i2s_t dev, i2s_transaction_t *transaction)
{
    unsigned irq_state = irq_disable();
    clist_rpush(&(state(dev)->transactions), &transaction->node);
    irq_restore(irq_state);
}

static void _dma_transfer_callback(dma_t dma, void *arg)
{
    (void)dma;
    i2s_t dev = (i2s_t)(intptr_t)arg;
    clist_node_t *transactions = &(state(dev)->transactions);
    i2s_transaction_t *previous = state(dev)->current_transaction;
    /* pop the previous transaction */
    state(dev)->frames_transmitted += state(dev)->transaction_bytes / state(dev)->bytes_per_frame;
    if (!clist_is_empty(transactions)) {
        _append_transaction(dev);
    }
    else {
        i2s_stop(dev);
        if (state(dev)->event_callback) {
            state(dev)->event_callback(dev, I2S_STATE_ERROR, I2S_EVENT_ERR_TX_UDR, state(dev)->arg);
        }
    }
    if (state(dev)->transaction_callback) {
        state(dev)->transaction_callback(dev, previous, state(dev)->arg);
    }
}

size_t i2s_frames_transmitted(i2s_t dev)
{
    unsigned irq_state = irq_disable();
    size_t num_transmitted = state(dev)->frames_transmitted;
    size_t dma_bytes_remaining = dma_items_remaining(i2s_config[dev].dma) * 2;
    size_t dma_bytes_transmitted = state(dev)->transaction_bytes - dma_bytes_remaining;
    irq_restore(irq_state);
    num_transmitted += dma_bytes_transmitted / state(dev)->bytes_per_frame;
    return num_transmitted;
}

size_t i2s_write_transaction_buffer(i2s_t dev, const void *source, size_t bytes, i2s_transaction_t *transaction, size_t offset)
{
    assert(transaction && transaction->buf);
    size_t word_size = periph(dev)->I2SCFGR & SPI_I2SCFGR_CHLEN ? 32 : 16;
    size_t transaction_bytes = state(dev)->transaction_bytes;
    size_t bytes_to_copy = MIN(bytes, transaction_bytes - offset);
    if (word_size == 32) {
        /* swap half words */
        for (size_t i = 0; i < bytes_to_copy/4; i++) {
            uint32_t val = unaligned_get_u32((uint8_t*)source + (4 * i));
            uint32_t *target = ((uint32_t*)(void*)transaction->buf) + i; /* Should be aligned already */
            *target = (val << 16) | (val >> 16);
        }
    }
    else {
        /* memcpy */
        uint8_t *target = transaction->buf + offset;
        memcpy(target, (uint8_t*)source, bytes_to_copy);
    }
    return bytes_to_copy;
}

static inline void _irq_handler(i2s_t dev)
{
    if (state(dev)->event_callback) {
        uint32_t sr = periph(dev)->SR;
        if (sr & SPI_SR_UDR) {
            state(dev)->event_callback(dev, I2S_STATE_ERROR, I2S_EVENT_ERR_TX_UDR, state(dev)->arg);
        }
        if (sr & SPI_SR_OVR) {
            state(dev)->event_callback(dev, I2S_STATE_ERROR, I2S_EVENT_ERR_RX_OVR, state(dev)->arg);
        }
        if (sr & SPI_SR_FRE) {
            state(dev)->event_callback(dev, I2S_STATE_ERROR, I2S_EVENT_ERR_FRAME, state(dev)->arg);
        }
    }
}

#ifdef I2S_0_ISR
void I2S_0_ISR(void)
{
    _irq_handler(I2S_DEV(0));
}
#endif

#ifdef I2S_1_ISR
void I2S_1_ISR(void)
{
    _irq_handler(I2S_DEV(1));
}
#endif

#ifdef I2S_2_ISR
void I2S_2_ISR(void)
{
    _irq_handler(I2S_DEV(2));
}
#endif

#ifdef I2S_3_ISR
void I2S_3_ISR(void)
{
    _irq_handler(I2S_DEV(3));
}
#endif

#ifdef I2S_4_ISR
void I2S_4_ISR(void)
{
    _irq_handler(I2S_DEV(4));
}
#endif

#ifdef I2S_5_ISR
void I2S_5_ISR(void)
{
    _irq_handler(I2S_DEV(5));
}
#endif

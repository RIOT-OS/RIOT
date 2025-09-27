/*
 * SPDX-FileCopyrightText: 2021 Otto-von-Guericke Universität Magdeburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_rpx0xx
 * @{
 *
 * @file
 * @brief       PIO program to emulate an I2C interface
 *
 * @author      Fabian Hüßler <fabian.huessler@ovgu.de>
 *
 * @}
 */
#include <errno.h>
#include "byteorder.h"
#include "periph_conf.h"
#include "periph/gpio.h"
#include "periph/pio/i2c.h"
#include "pio/pio.h"
#include "i2c_set_scl_sda.pio.h"
#include "i2c.pio.h"

#define I2C_PIO_IRQN        0 /* use irq flag 0 */
#define I2C_PIO_IRQN_SM(sm) PIO_IRQ_REL_MASK(I2C_PIO_IRQN, sm) /* IRQ is relative */

#define INSTR_I2C_SCL0_SDA0 (((pio_instr_t[])PIO_SET_SCL_SDA_PROGRAM)[0])
#define INSTR_I2C_SCL0_SDA1 (((pio_instr_t[])PIO_SET_SCL_SDA_PROGRAM)[1])
#define INSTR_I2C_SCL1_SDA0 (((pio_instr_t[])PIO_SET_SCL_SDA_PROGRAM)[2])
#define INSTR_I2C_SCL1_SDA1 (((pio_instr_t[])PIO_SET_SCL_SDA_PROGRAM)[3])

#define I2C_INSTR_FRAME(icount) ((uint32_t)(((uint32_t)(icount)) << 10))
#define I2C_INSTR(instr) ((uint32_t)(instr))
#define I2C_DATA_FRAME(final, data, nak)        \
    ((uint32_t)((((uint32_t)(final)) << 9) |    \
                (((uint32_t)(data)) << 1)  |    \
                 ((uint32_t)(nak))))

enum {
    I2C_REPSTART = 0x40 /* make sure this flag is not used by the I2C driver */
};

#if defined(PIO_I2C_NUMOF) || defined(DOXYGEN)
/**
 * @brief   Internal PIO I2C buses initialized from PIO_I2C_CONFIG
 */
static pio_i2c_bus_t _bus[PIO_I2C_NUMOF];
#endif

/**
 * @brief   Shared programs between I2C buses per PIO
 */
static pio_program_i2c_t _prog[PIO_NUMOF];

/**
 * @brief    PIO interrupt mask for all state machines per PIO
 */
static volatile uint32_t _irq[PIO_NUMOF];

static void pio_i2c_init_pins(pio_t pio, pio_sm_t sm, gpio_t sda, gpio_t scl)
{
    const gpio_pad_ctrl_t pad_ctrl = {
        .pull_up_enable = 1,
        .input_enable = 1,
        .drive_strength = DRIVE_STRENGTH_4MA,
        .schmitt_trig_enable = 1,
    };
    const gpio_io_ctrl_t io_ctrl = {
        .function_select = pio ? FUNCTION_SELECT_PIO1 : FUNCTION_SELECT_PIO0,
        .output_enable_override = OUTPUT_ENABLE_OVERRIDE_INVERT
    };
    /* Assume that SDA is mapped as the first set pin (bit 0),
       and SCL is mapped as the second set pin (bit 1).
       Try to avoid glitching the bus while connecting the IOs. Get things set
       up so that pin is driven down when PIO asserts OE low, and pulled up
       otherwise. */
    gpio_set_pad_config(scl, pad_ctrl);
    gpio_set_pad_config(sda, pad_ctrl);
    pio_sm_set_pins_with_mask(pio, sm, (1u << sda) | (1u << scl), (1u << sda) | (1u << scl));
    pio_sm_set_pindirs_with_mask(pio, sm, (1u << sda) | (1u << scl), (1u << sda) | (1u << scl));
    gpio_set_io_config(scl, io_ctrl);
    gpio_set_io_config(sda, io_ctrl);
    pio_sm_set_pins_with_mask(pio, sm, 0, (1u << sda) | (1u << scl));
}

static inline bool pio_i2c_check_error(pio_t pio, pio_sm_t sm)
{
    bool error;
    uint32_t status = irq_disable();
    error = !!(_irq[pio] & I2C_PIO_IRQN_SM(sm));
    irq_restore(status);
    return error;
}

static inline void pio_i2c_clear_error(pio_t pio, pio_sm_t sm)
{
    uint32_t status = irq_disable();
    _irq[pio] &= ~I2C_PIO_IRQN_SM(sm);
    irq_restore(status);
}

static void pio_i2c_resume_after_error(pio_t pio, pio_sm_t sm)
{
    PIO0_Type *dev = pio_config[pio].dev;
    pio_sm_ctrl_regs_t *ctrl = &PIO_SM_CTRL_BASE(dev)[sm];
    uint32_t jmp_addr = (ctrl->execctrl & PIO0_SM0_EXECCTRL_WRAP_BOTTOM_Msk)
                         >> PIO0_SM0_EXECCTRL_WRAP_BOTTOM_Pos;
    pio_sm_exec(pio, sm, pio_inst_jmp(PIO_INST_JMP_COND_NONE, jmp_addr));
}

static inline bool pio_i2c_check_idle(pio_t pio, pio_sm_t sm)
{
    PIO0_Type *dev = pio_config[pio].dev;
    pio_sm_ctrl_regs_t *ctrl = &PIO_SM_CTRL_BASE(dev)[sm];
    uint32_t idle = (ctrl->execctrl & PIO0_SM0_EXECCTRL_WRAP_BOTTOM_Msk)
                    >> PIO0_SM0_EXECCTRL_WRAP_BOTTOM_Pos;
    return ctrl->addr == idle;
}

static inline void _pio_i2c_put(pio_t pio, pio_sm_t sm, uint32_t word)
{
    while (pio_sm_transmit_word(pio, sm, word)) {
        /* do not block on unexpected receive */
        pio_sm_receive_word(pio, sm, NULL);
    }
}

static void pio_i2c_start(pio_t pio, pio_sm_t sm)
{
    _pio_i2c_put(pio, sm, I2C_INSTR_FRAME(1) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL1_SDA0) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL0_SDA0) << 16);
}

static void pio_i2c_stop(pio_t pio, pio_sm_t sm)
{
    _pio_i2c_put(pio, sm, I2C_INSTR_FRAME(2) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL0_SDA0) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL1_SDA0) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL1_SDA1) << 16);
}

static void pio_i2c_repstart(pio_t pio, pio_sm_t sm)
{
    _pio_i2c_put(pio, sm, I2C_INSTR_FRAME(3) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL0_SDA1) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL1_SDA1) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL1_SDA0) << 16);
    _pio_i2c_put(pio, sm, I2C_INSTR(INSTR_I2C_SCL0_SDA0) << 16);
}

static void _irq_sm(pio_t pio, unsigned irq)
{
    _irq[pio] |= PIO_IRQ_MASK(irq);
    pio_sm_t sm = pio_irq_rel_sm(I2C_PIO_IRQN, irq);
    pio_i2c_resume_after_error(pio, sm);
}

static pio_isr_sm_vec_t _vec = {
    .sm = _irq_sm,
};

pio_i2c_bus_t *pio_i2c_get(pio_i2c_t id)
{
#ifdef PIO_I2C_NUMOF
    return (id < PIO_I2C_NUMOF) ? &_bus[id] : NULL;
#endif
    (void)id;
    return NULL;
}

unsigned pio_i2c_numof(void)
{
#ifdef PIO_I2C_NUMOF
    return PIO_I2C_NUMOF;
#endif
    return 0;
}

const pio_program_i2c_t *pio_i2c_get_program(pio_t pio)
{
    assert(pio < PIO_NUMOF);
    return &_prog[pio];
}

int pio_i2c_init_program(pio_t pio)
{
    int ret;
    if (!_prog[pio].base.instr_numof) {
        _prog[pio].base = pio_i2c_create_program();
    }
    if (_prog[pio].base.location == PIO_PROGRAM_NOT_LOADED) {
        if ((ret = pio_alloc_program(pio, &_prog[pio].base))) {
            return ret;
        }
        if (!_prog[pio].base.written) {
            if ((ret = pio_i2c_write_program(pio, &_prog[pio]))) {
                return ret;
            }
        }
    }
    return 0;
}

void pio_i2c_deinit_program(pio_t pio)
{
    pio_free_program(pio, &_prog[pio].base);
}

pio_sm_t pio_i2c_sm_lock(pio_t pio, pio_i2c_bus_t *i2c)
{
    pio_sm_t sm = i2c->sm;
    if (!(_prog[pio].ref_mask & PIO_SM_MASK(sm))) {
        if ((sm = pio_sm_lock(pio)) >= 0) {
            _prog[pio].ref_mask |= PIO_SM_MASK(sm);
            i2c->pio = pio;
            i2c->sm = sm;
            mutex_init(&i2c->mtx);
        }
    }
    return sm;
}

void pio_i2c_sm_unlock(pio_i2c_bus_t *i2c)
{
    _prog[i2c->pio].ref_mask &= ~PIO_SM_MASK(i2c->sm);
    pio_sm_unlock(i2c->pio, i2c->sm);
}

void pio_i2c_start_programs(void)
{
    for (int i = 0; i < (int)pio_i2c_numof(); i++) {
        pio_i2c_bus_t *i2c = pio_i2c_get(i);
        if (i2c) {
            pio_sm_start(i2c->pio, i2c->sm);
        }
    }
}

void pio_i2c_stop_programs(void)
{
    for (int i = 0; i < (int)pio_i2c_numof(); i++) {
        pio_i2c_bus_t *i2c = pio_i2c_get(i);
        if (i2c) {
            pio_sm_stop(i2c->pio, i2c->sm);
        }
    }
}

int pio_i2c_write_program(pio_t pio, pio_program_i2c_t *pro)
{
    pio_instr_t instr[] = PIO_I2C_PROGRAM;
    int ret;
    if ((ret = pio_write_program(pio, &pro->base, instr))) {
        return ret;
    }
    return 0;
}

int pio_i2c_init(pio_i2c_bus_t *bus,
                 const pio_program_i2c_t *pro,
                 gpio_t sda, gpio_t scl, unsigned irq)
{
    pio_program_conf_t conf = PIO_I2C_PROGRAM_CONF;
    pio_t pio = bus->pio;
    pio_sm_t sm = bus->sm;
    int ret;
    if (sda >= 32 || scl >= 32 || irq >= 2) {
        return -ENOTSUP;
    }
    if ((ret = pio_sm_init_common(pio, sm, &pro->base, &conf))) {
        return ret;
    }
    pio_sm_set_out_pins(pio, sm, sda, 1);
    pio_sm_set_set_pins(pio, sm, sda, 2);
    pio_sm_set_in_pins(pio, sm, sda);
    pio_sm_set_sideset_pins(pio, sm, scl);
    pio_sm_set_out_shift(pio, sm, false, true, 16);
    pio_sm_set_in_shift(pio, sm, false, true, 8);
    pio_sm_set_jmp_pin(pio, sm, sda);
    pio_sm_set_clkdiv(pio, sm, pio_sm_clkdiv(3200000));
    pio_i2c_init_pins(pio, sm, sda, scl);
    pio_irq_clear(pio, pio_irq_rel_index(I2C_PIO_IRQN, sm));
    /* Given the absolute interrupt index I2C_PIO_IRQN
       and the relative interrupt instruction in the PIO program,
       sm will raise the interrupt flag pio_irq_rel_index(I2C_PIO_IRQN, sm)
       in the IRQ register. */
    pio_set_isr_sm_vec(pio, pio_irq_rel_index(I2C_PIO_IRQN, sm), &_vec);
    pio_irq_enable(pio, irq, PIO_IRQ_SM_0 << pio_irq_rel_index(I2C_PIO_IRQN, sm));
    pio_sm_clear_fifos(pio, sm);
    pio_sm_restart(pio, sm);
    return 0;
}

void pio_i2c_acquire(pio_i2c_bus_t *bus)
{
    assert(bus);
    mutex_lock(&bus->mtx);
}

void pio_i2c_release(pio_i2c_bus_t *bus)
{
    assert(bus);
    mutex_unlock(&bus->mtx);
}

int pio_i2c_read_regs(pio_t pio, pio_sm_t sm, uint16_t addr,
                      uint16_t reg, void *data, size_t len, uint8_t flags)
{
    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }
    reg = htons(reg);
    int error;
    if ((error = pio_i2c_write_bytes(pio, sm, addr,
                                     (flags & I2C_REG16) ? ((uint8_t *)&reg)
                                                         : ((uint8_t *)&reg) + 1,
                                     (flags & I2C_REG16) ? 2 : 1,
                                     flags | I2C_NOSTOP))) {
        return error;
    }
    if ((error = pio_i2c_read_bytes(pio, sm, addr, data, len,
                                    flags | I2C_REPSTART))) {
        return error;
    }
    return 0;
}

int pio_i2c_read_bytes(pio_t pio, pio_sm_t sm, uint16_t addr,
                       void *data, size_t len, uint8_t flags)
{
    bool first = true;
    unsigned len_rx = len;
    uint32_t word;
    int error = 0; /* potential error */
    int status = 0; /* actual error */
    pio_i2c_clear_error(pio, sm);
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    addr = (uint8_t)addr;
    if (!(flags & I2C_NOSTART)) {
        if (flags & I2C_REPSTART) {
            pio_i2c_repstart(pio, sm);
        }
        else {
            pio_i2c_start(pio, sm);
        }
    }
    while (!pio_sm_receive_word(pio, sm, NULL)) {}
    if (!(flags & I2C_NOSTART)) {
        _pio_i2c_put(pio, sm, I2C_DATA_FRAME(0, (addr << 1) | I2C_READ, 1) << 16);
        error = -ENXIO;
        len_rx += 1;
        while (!pio_sm_tx_fifo_empty(pio, sm) || !pio_i2c_check_idle(pio, sm)) {}
        if ((status = pio_i2c_check_error(pio, sm))) {
            pio_sm_clear_fifos(pio, sm);
        }
    }
    while (!pio_sm_tx_fifo_empty(pio, sm) || !pio_i2c_check_idle(pio, sm)) {}
    while ((len_rx || len) && !status) {
        if ((status = pio_i2c_check_error(pio, sm))) {
            pio_sm_clear_fifos(pio, sm);
            break;
        }
        /* len == 1 means final (last byte in transfer) */
        /* 0xff is a dummy data byte */
        /* Any NAK except for the last byte will cause the transfer to fail */
        /* see frame documentation in i2c.pio */
        if (len && !pio_sm_transmit_word(pio, sm, I2C_DATA_FRAME(len == 1, 0xff, len == 1) << 16)) {
            len--;
            error = -EIO;
        }
        /* read a byte for every byte we sent */
        if (len_rx && !pio_sm_receive_word(pio, sm, &word)) {
            if (!first) {
                *((uint8_t *)data) = (uint8_t)word;
                data = ((uint8_t *)data) + 1;
            }
            else {
                first = false;
            }
            len_rx--;
        }
    }
    while (!pio_sm_tx_fifo_empty(pio, sm)) {}
    if (!(flags & I2C_NOSTOP) || status || (status = pio_i2c_check_error(pio, sm))) {
        pio_i2c_stop(pio, sm);
    }
    while (!pio_sm_tx_fifo_empty(pio, sm)) {}
    return status ? error : (pio_i2c_check_error(pio, sm) ? -EIO : 0);
}

int pio_i2c_write_bytes(pio_t pio, pio_sm_t sm, uint16_t addr,
                        const void *data, size_t len, uint8_t flags)
{
    unsigned len_rx = len;
    int error = 0; /* potential error */
    int status = 0; /* actual error */
    pio_i2c_clear_error(pio, sm);
    if (flags & I2C_ADDR10) {
        return -EOPNOTSUPP;
    }
    addr = (uint8_t)addr;
    if (!(flags & I2C_NOSTART)) {
        if (flags & I2C_REPSTART) {
            pio_i2c_repstart(pio, sm);
        }
        else {
            pio_i2c_start(pio, sm);
        }
    }
    while (!pio_sm_receive_word(pio, sm, NULL)) {}
    if (!(flags & I2C_NOSTART)) {
        _pio_i2c_put(pio, sm, I2C_DATA_FRAME(0, (addr << 1), 1) << 16);
        error = -ENXIO;
        len_rx += 1;
        while (!pio_sm_tx_fifo_empty(pio, sm) || !pio_i2c_check_idle(pio, sm)) {}
        if ((status = pio_i2c_check_error(pio, sm))) {
            pio_sm_clear_fifos(pio, sm);
        }
    }
    while (!pio_sm_tx_fifo_empty(pio, sm) || !pio_i2c_check_idle(pio, sm)) {}
    while ((len_rx || len) && !status) {
        if ((status = pio_i2c_check_error(pio, sm))) {
            pio_sm_clear_fifos(pio, sm);
            break;
        }
        /* len == 1 means final (last byte in transfer) */
        /* transmit next data byte */
        /* don´t abort on NAK */
        /* see frame documentation in i2c.pio */
        if (len && !pio_sm_transmit_word(pio, sm, I2C_DATA_FRAME(len == 1, *(uint8_t *)data, 1) << 16)) {
            len--;
            data = ((uint8_t *)data) + 1;
            error = -EIO;
        }
        /* dummy receive for every sent byte */
        if (len_rx && !pio_sm_receive_word(pio, sm, NULL)) {
            /* https://github.com/raspberrypi/pico-examples/issues/168
             * Switching autopush on for Rx and off for Tx seems to bring a mysterious bug with it.
             * So instead we also drain the Rx FIFO while transmitting. */
            len_rx--;
        }
    }
    while (!pio_sm_tx_fifo_empty(pio, sm)) {}
    if (!(flags & I2C_NOSTOP) || status || (status = pio_i2c_check_error(pio, sm))) {
        pio_i2c_stop(pio, sm);
    }
    while (!pio_sm_tx_fifo_empty(pio, sm)) {}
    return status ? error : (pio_i2c_check_error(pio, sm) ? -EIO : 0);
}

int pio_i2c_write_regs(pio_t pio, pio_sm_t sm, uint16_t addr,
                       uint16_t reg, const void *data, size_t len, uint8_t flags)
{
    if (flags & (I2C_NOSTOP | I2C_NOSTART)) {
        return -EOPNOTSUPP;
    }
    reg = htons(reg);
    int error;
    if ((error = pio_i2c_write_bytes(pio, sm, addr,
                                     (flags & I2C_REG16) ? ((uint8_t *)&reg)
                                                         : ((uint8_t *)&reg) + 1,
                                     (flags & I2C_REG16) ? 2 : 1,
                                     flags | I2C_NOSTOP))) {
        return error;
    }
    if ((error = pio_i2c_write_bytes(pio, sm, addr, data, len, flags | I2C_NOSTART))) {
        return error;
    }
    return 0;
}

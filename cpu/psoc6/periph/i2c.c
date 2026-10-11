#include <errno.h>
#include <stdbool.h>
#include <stdint.h>

#include "assert.h"
#include "cpu.h"
#include "periph/gpio.h"
#include "periph/i2c.h"
#include "periph_conf.h"
#include "periph_cpu.h"
#include "ztimer.h"

#define I2C_BUS_TIMEOUT_US      (50000UL)
#define I2C_BYTE_TIMEOUT_US     (10000UL)

static i2c_state_t _i2c_state[I2C_NUMOF];
static uint32_t _last_intr_m_err[I2C_NUMOF];

static inline bool _master_error(uint32_t intr_m)
{
    return (intr_m & PSOC6_I2C_ERROR_MASK) != 0UL;
}

static void _delay_us(uint32_t usec)
{
    if (usec > 0U) {
        ztimer_sleep(ZTIMER_USEC, usec);
    }
}

static uint32_t _timestamp_now(void)
{
    return ztimer_now(ZTIMER_USEC);
}

static bool _timeout_expired(uint32_t start, uint32_t timeout_us)
{
    return ((uint32_t)(_timestamp_now() - start) >= timeout_us);
}

// reset scb state after abort
static void _i2c_recover(i2c_t dev)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    SCB_I2C_M_CMD(scb) = SCB_I2C_M_CMD_M_STOP_Msk | SCB_I2C_M_CMD_M_NACK_Msk;

    SCB_TX_FIFO_CTRL(scb) |= SCB_TX_FIFO_CTRL_CLEAR_Msk;
    SCB_TX_FIFO_CTRL(scb) &= ~SCB_TX_FIFO_CTRL_CLEAR_Msk;
    SCB_RX_FIFO_CTRL(scb) |= SCB_RX_FIFO_CTRL_CLEAR_Msk;
    SCB_RX_FIFO_CTRL(scb) &= ~SCB_RX_FIFO_CTRL_CLEAR_Msk;

    SCB_INTR_M(scb) = 0xFFFFFFFFUL;
    SCB_INTR_RX(scb) = 0xFFFFFFFFUL;
    SCB_INTR_TX(scb) = 0xFFFFFFFFUL;

    while ((SCB_I2C_STATUS(scb) & SCB_I2C_STATUS_BUS_BUSY_Msk) != 0UL) {
        if (_timeout_expired(start, I2C_BUS_TIMEOUT_US)) {
            break;
        }
    }

    _i2c_state[dev] = I2C_STATE_IDLE;
}

// send start + addr byte
static int _master_send_start(i2c_t dev, uint8_t addr7, bool read)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    if (_i2c_state[dev] != I2C_STATE_IDLE) {
        return -EIO;
    }

    SCB_INTR_S_MASK(scb) = 0UL;
    SCB_INTR_M(scb) = CY_SCB_I2C_MASTER_INTR_ALL;
    SCB_INTR_RX(scb) = SCB_INTR_RX_NOT_EMPTY_Msk;
    SCB_TX_FIFO_CTRL(scb) |= SCB_TX_FIFO_CTRL_CLEAR_Msk;
    SCB_TX_FIFO_CTRL(scb) &= ~SCB_TX_FIFO_CTRL_CLEAR_Msk;

    while ((SCB_I2C_STATUS(scb) & SCB_I2C_STATUS_BUS_BUSY_Msk) != 0UL) {
        if (_timeout_expired(start, I2C_BUS_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }

    SCB_I2C_M_CMD(scb) = SCB_I2C_M_CMD_M_START_ON_IDLE_Msk;
    _delay_us(50U);
    SCB_TX_FIFO_WR(scb) = _VAL2FLD(CY_SCB_I2C_ADDRESS, addr7) |
                          (read ? CY_SCB_I2C_READ_XFER : CY_SCB_I2C_WRITE_XFER);

    start = _timestamp_now();
    while (true) {
        uint32_t intr = SCB_INTR_M(scb);

        if (_master_error(intr)) {
            _last_intr_m_err[dev] = intr & PSOC6_I2C_ERROR_MASK;
            _i2c_recover(dev);
            return -EIO;
        }

        if ((intr & PSOC6_I2C_TX_DONE_MASK) != 0UL) {
            SCB_INTR_M(scb) = PSOC6_I2C_TX_DONE_MASK;
            _i2c_state[dev] = read ? I2C_STATE_MASTER_RX0 : I2C_STATE_MASTER_TX;
            return 0;
        }

        if (_timeout_expired(start, I2C_BYTE_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }
}

static int _master_write_byte(i2c_t dev, uint8_t byte)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    if (_i2c_state[dev] != I2C_STATE_MASTER_TX) {
        return -EIO;
    }

    SCB_INTR_M(scb) = CY_SCB_I2C_MASTER_INTR_ALL;
    SCB_TX_FIFO_WR(scb) = byte;

    while (true) {
        uint32_t intr = SCB_INTR_M(scb);

        if (_master_error(intr)) {
            _last_intr_m_err[dev] = intr & PSOC6_I2C_ERROR_MASK;
            _i2c_recover(dev);
            return -EIO;
        }

        if ((intr & PSOC6_I2C_TX_DONE_MASK) != 0UL) {
            SCB_INTR_M(scb) = PSOC6_I2C_TX_DONE_MASK;
            return 0;
        }

        if (_timeout_expired(start, I2C_BYTE_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }
}

static int _master_send_restart(i2c_t dev, uint8_t addr7, bool read)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    if ((_i2c_state[dev] == I2C_STATE_IDLE) || (_i2c_state[dev] == I2C_STATE_UNKNOWN)) {
        return -EIO;
    }

    _i2c_state[dev] = I2C_STATE_MASTER_ADDR;
    SCB_INTR_M(scb) = CY_SCB_I2C_MASTER_INTR_ALL;
    SCB_TX_FIFO_CTRL(scb) |= SCB_TX_FIFO_CTRL_CLEAR_Msk;
    SCB_TX_FIFO_CTRL(scb) &= ~SCB_TX_FIFO_CTRL_CLEAR_Msk;
    SCB_I2C_M_CMD(scb) = SCB_I2C_M_CMD_M_START_Msk;

    _delay_us(50U);
    SCB_TX_FIFO_WR(scb) = _VAL2FLD(CY_SCB_I2C_ADDRESS, addr7) |
                          (read ? CY_SCB_I2C_READ_XFER : CY_SCB_I2C_WRITE_XFER);

    while (true) {
        uint32_t intr = SCB_INTR_M(scb);

        if (_master_error(intr)) {
            _last_intr_m_err[dev] = intr & PSOC6_I2C_ERROR_MASK;
            _i2c_recover(dev);
            return -EIO;
        }

        if ((intr & PSOC6_I2C_TX_DONE_MASK) != 0UL) {
            SCB_INTR_M(scb) = PSOC6_I2C_TX_DONE_MASK;
            _i2c_state[dev] = read ? I2C_STATE_MASTER_RX0 : I2C_STATE_MASTER_TX;
            return 0;
        }

        if (_timeout_expired(start, I2C_BYTE_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }
}

static int _master_read_byte(i2c_t dev, bool ack, uint8_t *byte)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    if (byte == NULL) {
        _i2c_state[dev] = I2C_STATE_IDLE;
        return -EINVAL;
    }

    while (true) {
        uint32_t intr = SCB_INTR_M(scb);

        if (_master_error(intr)) {
            _last_intr_m_err[dev] = intr & PSOC6_I2C_ERROR_MASK;
            _i2c_recover(dev);
            return -EIO;
        }

        if (_FLD2VAL(SCB_RX_FIFO_STATUS_USED, SCB_RX_FIFO_STATUS(scb)) > 0UL) {
            *byte = (uint8_t)SCB_RX_FIFO_RD(scb);
            SCB_I2C_M_CMD(scb) = ack ? SCB_I2C_M_CMD_M_ACK_Msk :
                                       SCB_I2C_M_CMD_M_NACK_Msk;
            _i2c_state[dev] = I2C_STATE_MASTER_RX0;
            return 0;
        }

        if (_timeout_expired(start, I2C_BYTE_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }
}

static int _master_send_stop(i2c_t dev)
{
    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;
    uint32_t start = _timestamp_now();

    SCB_I2C_M_CMD(scb) = SCB_I2C_M_CMD_M_STOP_Msk | SCB_I2C_M_CMD_M_NACK_Msk;

    while (true) {
        uint32_t intr = SCB_INTR_M(scb);

        if (_master_error(intr)) {
            _last_intr_m_err[dev] = intr & PSOC6_I2C_ERROR_MASK;
            _i2c_recover(dev);
            return -EIO;
        }

        if ((intr & PSOC6_I2C_STOP_DONE_MASK) != 0UL) {
            SCB_INTR_M(scb) = PSOC6_I2C_STOP_DONE_MASK;
            break;
        }

        if (_timeout_expired(start, I2C_BYTE_TIMEOUT_US)) {
            _i2c_recover(dev);
            return -ETIMEDOUT;
        }
    }

    _i2c_recover(dev);
    return 0;
}

// init scb + pin mux
void i2c_init(i2c_t dev)
{
    assert(dev < I2C_NUMOF);

    const i2c_conf_t *cfg = &i2c_config[dev];
    CySCB_Type *scb = cfg->dev;

    scb->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    scb->CTRL = _BOOL2FLD(SCB_CTRL_ADDR_ACCEPT, false) |
                _BOOL2FLD(SCB_CTRL_EC_AM_MODE, false);
    scb->CTRL |= SCB_CTRL_BYTE_MODE_Msk;
    scb->I2C_CTRL = _BOOL2FLD(SCB_I2C_CTRL_S_GENERAL_IGNORE, true) |
                    _VAL2FLD(SCB_I2C_CTRL_HIGH_PHASE_OVS, cfg->high_phase_ovs - 1U) |
                    _VAL2FLD(SCB_I2C_CTRL_LOW_PHASE_OVS, cfg->low_phase_ovs - 1U) |
                    _VAL2FLD(SCB_I2C_CTRL_MODE, 2U);
    scb->RX_CTRL = _BOOL2FLD(SCB_RX_CTRL_MEDIAN, false) | SCB_I2C_RX_CTRL;
    scb->I2C_CFG = SCB_I2C_ENABLE_ANALOG_FILTER;
    scb->TX_CTRL = _VAL2FLD(SCB_TX_CTRL_DATA_WIDTH, 7UL) |
                   SCB_TX_CTRL_MSB_FIRST_Msk | SCB_TX_CTRL_OPEN_DRAIN_Msk;
    scb->RX_FIFO_CTRL = 0UL;
    scb->TX_FIFO_CTRL = 0UL;
    scb->RX_MATCH = _VAL2FLD(SCB_RX_MATCH_ADDR, 0U) |
                    _VAL2FLD(SCB_RX_MATCH_MASK, 0U);
    scb->INTR_RX_MASK = 0UL;
    scb->INTR_TX_MASK = 0UL;
    scb->INTR_M_MASK = 0UL;
    scb->INTR_S_MASK = 0UL;
    scb->INTR_SPI_EC_MASK = 0UL;
    scb->INTR_I2C_EC_MASK = 0UL;

    psoc6_set_drive_mode(cfg->scl_pin, PSOC6_DM_OD_SLOW);
    psoc6_set_drive_mode(cfg->sda_pin, PSOC6_DM_OD_SLOW);
    psoc6_set_hsiom(cfg->scl_pin, cfg->hsiom_scl);
    psoc6_set_hsiom(cfg->sda_pin, cfg->hsiom_sda);

    PERI->CLOCK_CTL[cfg->pclk_ctl_index] =
        _VAL2FLD(CY_PERI_CLOCK_CTL_TYPE_SEL, 0U) |
        _VAL2FLD(CY_PERI_CLOCK_CTL_DIV_SEL, cfg->div8_index);
    _CLR_SET_REG32(PERI->DIV_8_CTL[cfg->div8_index], PERI_DIV_8_CTL_INT8_DIV, cfg->div8_int_div);
    PERI->DIV_CMD = PERI_DIV_CMD_ENABLE_Msk |
                    CY_PERI_DIV_CMD_PA_DIV_SEL_Msk |
                    CY_PERI_DIV_CMD_PA_TYPE_SEL_Msk |
                    _VAL2FLD(CY_PERI_DIV_CMD_TYPE_SEL, 0U) |
                    _VAL2FLD(CY_PERI_DIV_CMD_DIV_SEL, cfg->div8_index);

    _i2c_state[dev] = I2C_STATE_IDLE;
}

void i2c_init_pins(i2c_t dev)
{
    i2c_init(dev);
}

void i2c_acquire(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        i2c_config[dev].dev->CTRL |= SCB_CTRL_ENABLED_Msk;
    }
}

void i2c_release(i2c_t dev)
{
    if (dev < I2C_NUMOF) {
        i2c_config[dev].dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    }
}

int i2c_read_reg(i2c_t dev, uint16_t addr, uint16_t reg, void *data, uint8_t flags)
{
    return i2c_read_regs(dev, addr, reg, data, 1U, flags);
}

static void _backoff_sleep(unsigned attempt)
{
    unsigned ms = 2U << ((attempt > 4U) ? 4U : attempt);
    _delay_us(ms * 1000U);
}

// read reg window via restart
int i2c_read_regs(i2c_t dev, uint16_t addr, uint16_t reg, void *data, size_t len, uint8_t flags)
{
    if ((dev >= I2C_NUMOF) || (data == NULL) || (len == 0U)) {
        return -EINVAL;
    }

    if ((flags & (I2C_ADDR10 | I2C_NOSTART)) != 0U) {
        return -EOPNOTSUPP;
    }

    uint8_t *rx = (uint8_t *)data;
    uint8_t addr7 = (uint8_t)(addr & 0x7FU);

    for (unsigned attempt = 0; attempt < 6U; attempt++) {
        _last_intr_m_err[dev] = 0U;
        _i2c_state[dev] = I2C_STATE_IDLE;

        if (_master_send_start(dev, addr7, false) < 0) {
            goto retry_or_fail;
        }

        if ((flags & I2C_REG16) != 0U) {
            if (_master_write_byte(dev, (uint8_t)(reg >> 8)) < 0) {
                goto retry_or_fail;
            }
        }

        if (_master_write_byte(dev, (uint8_t)reg) < 0) {
            goto retry_or_fail;
        }

        if (_master_send_restart(dev, addr7, true) < 0) {
            goto retry_or_fail;
        }

        for (size_t i = 0; i < len; i++) {
            if (_master_read_byte(dev, (i + 1U) < len, &rx[i]) < 0) {
                goto retry_or_fail;
            }
        }

        if ((flags & I2C_NOSTOP) == 0U) {
            return _master_send_stop(dev);
        }
        return 0;

retry_or_fail:
        // retry on nack, else fail
        _i2c_recover(dev);
        if ((_last_intr_m_err[dev] & SCB_INTR_M_I2C_NACK_Msk) != 0U) {
            _backoff_sleep(attempt);
            continue;
        }
        return -EIO;
    }

    return -EIO;
}

int i2c_write_reg(i2c_t dev, uint16_t addr, uint16_t reg, uint8_t data, uint8_t flags)
{
    return i2c_write_regs(dev, addr, reg, &data, 1U, flags);
}

// write reg window, opt no stop
int i2c_write_regs(i2c_t dev, uint16_t addr, uint16_t reg, const void *data, size_t len, uint8_t flags)
{
    if ((dev >= I2C_NUMOF) || (data == NULL) || (len == 0U)) {
        return -EINVAL;
    }

    if ((flags & (I2C_ADDR10 | I2C_NOSTART)) != 0U) {
        return -EOPNOTSUPP;
    }

    const uint8_t *tx = (const uint8_t *)data;
    uint8_t addr7 = (uint8_t)(addr & 0x7FU);

    _last_intr_m_err[dev] = 0U;
    _i2c_state[dev] = I2C_STATE_IDLE;

    if (_master_send_start(dev, addr7, false) < 0) {
        return -EIO;
    }

    if ((flags & I2C_REG16) != 0U) {
        if (_master_write_byte(dev, (uint8_t)(reg >> 8)) < 0) {
            return -EIO;
        }
    }

    if (_master_write_byte(dev, (uint8_t)reg) < 0) {
        return -EIO;
    }

    for (size_t i = 0; i < len; i++) {
        if (_master_write_byte(dev, tx[i]) < 0) {
            return -EIO;
        }
    }

    if ((flags & I2C_NOSTOP) != 0U) {
        return 0;
    }

    return _master_send_stop(dev);
}

#include <errno.h>

#include "assert.h"
#include "cpu.h"
#include "mutex.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "periph_conf.h"
#include "periph_cpu.h"

static mutex_t _locks[SPI_NUMOF] = { MUTEX_INIT };

static inline uint32_t _clk_hz(spi_clk_t clk)
{
    switch (clk) {
        case SPI_CLK_100KHZ:
            return 100000UL;
        case SPI_CLK_400KHZ:
            return 400000UL;
        case SPI_CLK_1MHZ:
            return 1000000UL;
        case SPI_CLK_5MHZ:
            return 5000000UL;
        case SPI_CLK_10MHZ:
            return 10000000UL;
        default:
            return 1000000UL;
    }
}

static inline void _cs_assert(spi_cs_t cs)
{
    if (cs != SPI_CS_UNDEF) {
        gpio_clear((gpio_t)cs);
    }
}

static inline void _cs_deassert(spi_cs_t cs)
{
    if (cs != SPI_CS_UNDEF) {
        gpio_set((gpio_t)cs);
    }
}

static void _spi_route_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    const spi_conf_t *cfg = &spi_config[bus];

    psoc6_set_hsiom(cfg->miso_pin, cfg->hsiom_miso);
    psoc6_set_hsiom(cfg->mosi_pin, cfg->hsiom_mosi);
    psoc6_set_hsiom(cfg->sclk_pin, cfg->hsiom_sclk);

    psoc6_set_drive_mode(cfg->miso_pin, PSOC6_DM_HIGHZ);
    psoc6_set_drive_mode(cfg->mosi_pin, PSOC6_DM_STRONG);
    psoc6_set_drive_mode(cfg->sclk_pin, PSOC6_DM_STRONG);
}

static void _spi_clock_cfg(const spi_conf_t *cfg, uint32_t divider)
{
    PERI->CLOCK_CTL[cfg->pclk_ctl_index] =
        _VAL2FLD(CY_PERI_CLOCK_CTL_TYPE_SEL, 0U) |
        _VAL2FLD(CY_PERI_CLOCK_CTL_DIV_SEL, cfg->div8_index);
    _CLR_SET_REG32(PERI->DIV_8_CTL[cfg->div8_index], PERI_DIV_8_CTL_INT8_DIV, divider);
    PERI->DIV_CMD = PERI_DIV_CMD_ENABLE_Msk |
                    CY_PERI_DIV_CMD_PA_TYPE_SEL_Msk |
                    CY_PERI_DIV_CMD_PA_DIV_SEL_Msk |
                    _VAL2FLD(CY_PERI_DIV_CMD_TYPE_SEL, 0U) |
                    _VAL2FLD(CY_PERI_DIV_CMD_DIV_SEL, cfg->div8_index);
}

// init pin mux, keep scb off
void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    spi_config[bus].dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    _spi_route_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    spi_init(bus);
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    if (bus >= SPI_NUMOF) {
        return -ENXIO;
    }

    if (gpio_is_equal((gpio_t)cs, SPI_CS_UNDEF) || !gpio_is_valid((gpio_t)cs)) {
        return -EINVAL;
    }

    gpio_init((gpio_t)cs, GPIO_OUT);
    gpio_set((gpio_t)cs);
    return 0;
}

// cfg txn + enable scb
void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert(bus < SPI_NUMOF);

    const spi_conf_t *cfg = &spi_config[bus];
    CySCB_Type *dev = cfg->dev;
    uint32_t psoc_spi_mode = 0U;
    uint32_t target_sclk = _clk_hz(clk);
    uint32_t target_scb_clk = target_sclk * cfg->oversample;
    uint32_t divider = PERI_CLK / target_scb_clk;

    mutex_lock(&_locks[bus]);

    _spi_route_pins(bus);
    dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    dev->CTRL = _BOOL2FLD(SCB_CTRL_EC_AM_MODE, 0UL) |
                _VAL2FLD(SCB_CTRL_OVS, cfg->oversample - 1UL) |
                _VAL2FLD(SCB_CTRL_MODE, 1UL);
    dev->CTRL |= _BOOL2FLD(SCB_CTRL_BYTE_MODE, 1UL);

    if (divider == 0UL) {
        divider = 1UL;
    }
    divider -= 1UL;

    _spi_clock_cfg(cfg, divider);

    switch (mode) {
        case SPI_MODE_1:
            psoc_spi_mode = 2U;
            break;
        case SPI_MODE_2:
            psoc_spi_mode = 1U;
            break;
        case SPI_MODE_3:
            psoc_spi_mode = 3U;
            break;
        case SPI_MODE_0:
        default:
            psoc_spi_mode = 0U;
            break;
    }

    // map riot mode to psoc spi mode
    dev->SPI_CTRL = _BOOL2FLD(SCB_SPI_CTRL_SSEL_CONTINUOUS, 1UL) |
                    _BOOL2FLD(SCB_SPI_CTRL_SELECT_PRECEDE, 0UL) |
                    _BOOL2FLD(SCB_SPI_CTRL_LATE_MISO_SAMPLE, 1UL) |
                    _BOOL2FLD(SCB_SPI_CTRL_SCLK_CONTINUOUS, 0UL) |
                    _BOOL2FLD(SCB_SPI_CTRL_MASTER_MODE, 1UL) |
                    _VAL2FLD(CY_SCB_SPI_CTRL_CLK_MODE, psoc_spi_mode) |
                    _VAL2FLD(CY_SCB_SPI_CTRL_SSEL_POLARITY, 0UL) |
                    _VAL2FLD(SCB_SPI_CTRL_MODE, 0U);

    dev->RX_CTRL = _BOOL2FLD(SCB_RX_CTRL_MSB_FIRST, 1UL) |
                   _BOOL2FLD(SCB_RX_CTRL_MEDIAN, 0UL) |
                   _VAL2FLD(SCB_RX_CTRL_DATA_WIDTH, 7UL);
    dev->TX_CTRL = _BOOL2FLD(SCB_TX_CTRL_MSB_FIRST, 1UL) |
                   _VAL2FLD(SCB_TX_CTRL_DATA_WIDTH, 7UL);
    dev->RX_FIFO_CTRL = _VAL2FLD(SCB_RX_FIFO_CTRL_TRIGGER_LEVEL, 0UL);
    dev->TX_FIFO_CTRL = _VAL2FLD(SCB_TX_FIFO_CTRL_TRIGGER_LEVEL, 0UL);
    dev->TX_FIFO_CTRL |= SCB_TX_FIFO_CTRL_CLEAR_Msk;
    dev->TX_FIFO_CTRL &= ~SCB_TX_FIFO_CTRL_CLEAR_Msk;
    dev->RX_FIFO_CTRL |= SCB_RX_FIFO_CTRL_CLEAR_Msk;
    dev->RX_FIFO_CTRL &= ~SCB_RX_FIFO_CTRL_CLEAR_Msk;
    dev->INTR_RX = 0xFFFFFFFFUL;
    dev->INTR_TX = 0xFFFFFFFFUL;
    dev->INTR_M = 0xFFFFFFFFUL;
    dev->INTR_S = 0xFFFFFFFFUL;
    dev->INTR_SPI_EC = 0xFFFFFFFFUL;
    dev->INTR_RX_MASK = 0UL;
    dev->INTR_TX_MASK = 0UL;
    dev->INTR_M_MASK = 0UL;
    dev->INTR_S_MASK = 0UL;
    dev->INTR_SPI_EC_MASK = 0UL;
    dev->CTRL |= SCB_CTRL_ENABLED_Msk;
    (void)cs;
}

void spi_release(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    spi_config[bus].dev->CTRL &= ~SCB_CTRL_ENABLED_Msk;
    mutex_unlock(&_locks[bus]);
}

uint8_t spi_transfer_byte(spi_t bus, spi_cs_t cs, bool cont, uint8_t out)
{
    const spi_conf_t *cfg = &spi_config[bus];
    CySCB_Type *dev = cfg->dev;
    uint32_t fifo_size;

    // sw-controlled cs
    if (cs != SPI_CS_UNDEF) {
        _cs_assert(cs);
    }

    fifo_size = _FLD2BOOL(SCB_CTRL_BYTE_MODE, dev->CTRL) ? 128UL : 64UL;
    while (_FLD2VAL(SCB_TX_FIFO_STATUS_USED, dev->TX_FIFO_STATUS) >= fifo_size) {
    }

    dev->TX_FIFO_WR = (uint32_t)out;
    while (_FLD2VAL(SCB_RX_FIFO_STATUS_USED, dev->RX_FIFO_STATUS) == 0UL) {
    }

    uint8_t in = (uint8_t)dev->RX_FIFO_RD;

    if (!cont && (cs != SPI_CS_UNDEF)) {
        _cs_deassert(cs);
    }

    return in;
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont, const void *out, void *in, size_t len)
{
    const spi_conf_t *cfg = &spi_config[bus];
    CySCB_Type *dev = cfg->dev;
    const uint8_t *tx = (const uint8_t *)out;
    uint8_t *rx = (uint8_t *)in;
    uint32_t fifo_size = _FLD2BOOL(SCB_CTRL_BYTE_MODE, dev->CTRL) ? 128UL : 64UL;

    // sw-controlled cs
    if (cs != SPI_CS_UNDEF) {
        _cs_assert(cs);
    }

    for (size_t i = 0; i < len; i++) {
        uint8_t value = tx ? tx[i] : 0xFFU;

        // push tx byte, pull rx byte
        while (_FLD2VAL(SCB_TX_FIFO_STATUS_USED, dev->TX_FIFO_STATUS) >= fifo_size) {
        }
        dev->TX_FIFO_WR = (uint32_t)value;

        while (_FLD2VAL(SCB_RX_FIFO_STATUS_USED, dev->RX_FIFO_STATUS) == 0UL) {
        }

        if (rx) {
            rx[i] = (uint8_t)dev->RX_FIFO_RD;
        }
        else {
            (void)dev->RX_FIFO_RD;
        }
    }

    // wait shift complete
    while ((_FLD2VAL(SCB_TX_FIFO_STATUS_USED, dev->TX_FIFO_STATUS) +
            _FLD2VAL(SCB_TX_FIFO_STATUS_SR_VALID, dev->TX_FIFO_STATUS)) != 0UL) {
    }

    if (!cont && (cs != SPI_CS_UNDEF)) {
        _cs_deassert(cs);
    }
}

uint8_t spi_transfer_reg(spi_t bus, spi_cs_t cs, uint8_t reg, uint8_t out)
{
    uint8_t in;

    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1U);
    spi_transfer_bytes(bus, cs, false, &out, &in, 1U);
    return in;
}

void spi_transfer_regs(spi_t bus, spi_cs_t cs, uint8_t reg, const void *out, void *in, size_t len)
{
    spi_transfer_bytes(bus, cs, true, &reg, NULL, 1U);
    spi_transfer_bytes(bus, cs, false, out, in, len);
}

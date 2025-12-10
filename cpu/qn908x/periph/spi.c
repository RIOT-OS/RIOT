/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     cpu_qn908x
 * @ingroup     drivers_periph_spi
 *
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      iosabi <iosabi@protonmail.com>
 *
 * @}
 */

#include <assert.h>

#include "bitarithm.h"
#include "mutex.h"

#include "cpu.h"
#include "periph_conf.h"
#include "periph/spi.h"

#include "vendor/drivers/fsl_clock.h"
#include "flexcomm.h"
#include "gpio_mux.h"

#define ENABLE_DEBUG        0
#include "debug.h"

typedef struct {
    uint8_t *in;          /**< The RX buffer pointer or NULL if unused. */
    uint32_t in_len;      /**< The remaining bytes to receive or 0 if unused. */

    const uint8_t *out;   /**< The TX buffer pointer or NULL if unused. */

    /**
     * @brief The remaining transfer length.
     *
     * This value is set even if we are not transferring any data, in which case
     * it indicates the remaining 8-bit clock pulses needed to be sent to the
     * FIFO to finish the transfer.
     */
    uint32_t tr_len;

    uint32_t tx_mask;     /** FIFOWR mask used when transmitting. */
} spi_pending_transfer_t;

/**
 * @brief   Mutex for accessing each SPI bus.
 */
static mutex_t locks[SPI_NUMOF];

/**
 * @brief   Bitmask of Port A pins that use Function 4 for the FLEXCOMM2.
 *
 * SPI pins are either function 4 or 5 depending on the pin and flexcomm.
 * All FLEXCOMM3 possible pins are mapped to function 5, while in the
 * case of FLEXCOMM2 some are in function 4. Some pins can act as a function
 * in FLEXCOMM2 (function 4) while act as another function in FLEXCOM3 (function
 * 5)
 */
static const uint32_t _spi_func5_mask_fc2 =
    (1u << 0) | /* FC2_SSEL3 */
    (1u << 1) | /* FC2_SSEL2 */
    (1u << 2) | /* FC2_SSEL1 */
    (1u << 3) | /* FC2_SSEL0 */
    (1u << 4) | /* FC2_COPI */
    (1u << 5);  /* FC2_CIPO */

/**
 * @brief Set the clock divided for the target frequency.
 */
static void _spi_controller_set_speed(SPI_Type *spi_bus, uint32_t speed_hz)
{
    /* The SPI clock source is based on the FLEXCOMM clock with a simple
     * frequency divider between /1 and /65536. */
    const uint32_t bus_freq = CLOCK_GetFreq(kCLOCK_BusClk);
    uint32_t divider = (bus_freq + speed_hz / 2) / speed_hz;

    if (divider == 0) {
        divider = 1;
    }
    else if (divider > (1u << 16)) {
        divider = 1u << 16;
    }
    DEBUG("[spi] clock requested: %" PRIu32 " Hz, actual: %" PRIu32
          " Hz, divider: /%" PRIu32 "\n", speed_hz, bus_freq / divider,
          divider);
    /* The value stored in DIV is always (divider - 1), meaning that a value of
     * 0 divides by 1. */
    spi_bus->DIV = divider - 1;
}

void spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    const spi_conf_t *const conf = &spi_config[bus];
    SPI_Type *const spi_bus = conf->dev;

    int flexcomm_num = flexcomm_init((FLEXCOMM_Type *)spi_bus, FLEXCOMM_ID_SPI);
    DEBUG("[spi] init: bus=%u, flexcomm=%d\n", (unsigned)bus, flexcomm_num);
    assert(flexcomm_num >= 0);

    /* Set controller mode, but don't enable it. All CS are active low. MSB
     * first bit order (standard). */
    spi_bus->CFG = SPI_CFG_MASTER_MASK;
    /* Configure to use the RX and TX FIFO. */
    spi_bus->FIFOCFG = SPI_FIFOCFG_ENABLETX_MASK | SPI_FIFOCFG_ENABLERX_MASK;
    locks[bus] = (mutex_t)MUTEX_INIT_LOCKED;
    spi_init_pins(bus);
}

void spi_init_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    const spi_conf_t *const conf = &spi_config[bus];

    const uint32_t mask = conf->dev == (SPI_Type *)FLEXCOMM2_BASE
                          ? _spi_func5_mask_fc2
                          : 0xffffffff;
    gpio_init_mux(conf->copi_pin,
                  ((1u << GPIO_T_PIN(conf->copi_pin)) & mask) ? 5 : 4);
    gpio_init_mux(conf->cipo_pin,
                  ((1u << GPIO_T_PIN(conf->cipo_pin)) & mask) ? 5 : 4);
    gpio_init_mux(conf->clk_pin,
                  ((1u << GPIO_T_PIN(conf->clk_pin)) & mask) ? 5 : 4);
    /* Enables the SPI block and sets it to idle. */
    conf->dev->CFG |= SPI_CFG_ENABLE_MASK;
    mutex_unlock(&locks[bus]);
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    /* Initializing the CS pin doesn't require to acquire the mutex since each
     * peripheral has its own independent CS pin. */
    if (bus >= SPI_NUMOF) {
        return SPI_NODEV;
    }
    const spi_conf_t *const conf = &spi_config[bus];
    gpio_t pin = cs;

    if (GPIO_T_IS_HWCS(cs)) {
        /* The gpio_t value comes from the board config rather than the cs
         * variable itself when a HWCS number is passed. */
        pin = conf->cs_pin[GPIO_T_HWCS(cs)];
    }
    if (!gpio_is_valid(pin)) {
        return SPI_NOCS;
    }
    DEBUG("[spi] init_cs: cs=0x%.4" PRIx16 " pin=0x%.4" PRIx16 "\n", cs, pin);

    if (GPIO_T_IS_HWCS(cs)) {
        const uint32_t mask = conf->dev == (SPI_Type *)FLEXCOMM2_BASE
                              ? _spi_func5_mask_fc2
                              : 0xffffffff;
        gpio_init_mux(pin, ((1u << GPIO_T_PIN(pin)) & mask) ? 5 : 4);
    }
    else {
        gpio_init(pin, GPIO_OUT);
        gpio_set(pin);
    }

    return SPI_OK;
}

#ifdef MODULE_PERIPH_SPI_RECONFIGURE
void spi_deinit_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    mutex_lock(&locks[bus]);
    const spi_conf_t *const conf = &spi_config[bus];

    /* Disables the SPI block. It must be already idle. */
    conf->dev->CFG &= ~SPI_CFG_ENABLE_MASK;

    gpio_init(conf->copi_pin, GPIO_IN);
    gpio_init(conf->cipo_pin, GPIO_IN);
    gpio_init(conf->clk_pin, GPIO_IN);
}
#endif /* MODULE_PERIPH_SPI_RECONFIGURE */

void spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    assert((unsigned)bus < SPI_NUMOF);
    assert((mode & ~(SPI_CFG_CPHA_MASK | SPI_CFG_CPOL_MASK)) == 0);
    const spi_conf_t *const conf = &spi_config[bus];

    mutex_lock(&locks[bus]);

    /* Set SPI clock speed. This silently chooses the closest frequency, no
     * matter how far it is from the requested one. */
    _spi_controller_set_speed(conf->dev, clk);

    DEBUG("[spi] acquire: mode CPHA=%d CPOL=%d, cs=0x%" PRIx32 "\n",
          !!(mode & SPI_CFG_CPHA_MASK), !!(mode & SPI_CFG_CPOL_MASK),
          (uint32_t)cs);

    conf->dev->CFG =
        (conf->dev->CFG & ~(SPI_CFG_CPHA_MASK | SPI_CFG_CPOL_MASK)) | mode;
}

void spi_release(spi_t bus)
{
    assert(bus < SPI_NUMOF);
    DEBUG("[spi] release\n");
    mutex_unlock(&locks[bus]);
}

/**
 * @brief: Wait for the FIFO to be empty.
 */
static void _spi_wait_txempty(SPI_Type *spi_bus)
{
    while (!(spi_bus->FIFOSTAT & SPI_FIFOSTAT_TXEMPTY_MASK)) {}
}

/**
 * @brief Bitmask for the FIFOWR register with all the HWCS deasserted.
 */
#define SPI_HWCS_DEASSERT_ALL \
    (((1u << SPI_HWCS_NUMOF) - 1) << SPI_FIFOWR_TXSSEL0_N_SHIFT)

/**
 * @brief Initialize a SPI transfer given the transfer parameters.
 */
static void _spi_config_transfer(spi_pending_transfer_t *tr, spi_cs_t cs,
                                 bool cont, const void *out, void *in,
                                 size_t len)
{
    tr->in = in;
    tr->in_len = in ? len : 0;
    tr->out = out;
    tr->tr_len = len;
    tr->tx_mask = SPI_HWCS_DEASSERT_ALL;
    if (GPIO_T_IS_HWCS(cs)) {
        /* Flag that the TX should assert this HWCS by clearing the bit. */
        tr->tx_mask &= ~(1u << (SPI_FIFOWR_TXSSEL0_N_SHIFT + GPIO_T_HWCS(cs)));
        if (!cont) {
            /* Flag the End of Transfer (EOT) in the mask. This will only be
             * used in the last byte. */
            tr->tx_mask |= SPI_FIFOWR_EOT_MASK;
        }
    }
    if (!in) {
        /* Ignores the RX side when the @p in is NULL so we don't need to read
         * the FIFO at all. */
        tr->tx_mask |= SPI_FIFOWR_RXIGNORE_MASK;
    }
    tr->tx_mask |= SPI_FIFOWR_LEN(7);  /* Data transfers of 8 bits. */
}

/**
 * @brief Perform a blocking SPI transfer.
 */
static void _spi_transfer_blocking(spi_t bus, spi_pending_transfer_t *tr)
{
    SPI_Type *const spi_bus = spi_config[bus].dev;

    /* Configure to use the RX and TX fifo, and empty them. */
    spi_bus->FIFOCFG = SPI_FIFOCFG_ENABLETX_MASK
                     | SPI_FIFOCFG_ENABLERX_MASK
                     | SPI_FIFOCFG_EMPTYTX_MASK | SPI_FIFOCFG_EMPTYRX_MASK;
    spi_bus->FIFOSTAT = SPI_FIFOSTAT_TXERR_MASK | SPI_FIFOSTAT_RXERR_MASK;

    while (tr->in_len || tr->tr_len) {
        /* Read from RX FIFO if possible. */
        if (spi_bus->FIFOSTAT & SPI_FIFOSTAT_RXNOTEMPTY_MASK) {
            uint32_t rd = spi_bus->FIFORD;
            if (tr->in_len) {
                *(tr->in++) = (uint8_t)rd;
                tr->in_len--;
            }
        }

        /* Write when able to write and we have data to send or bogus (0) bytes
         * to send when in receive-only mode. */
        if ((spi_bus->FIFOSTAT & SPI_FIFOSTAT_TXNOTFULL_MASK) && tr->tr_len) {
            uint32_t wr = tr->tx_mask;
            if (tr->out) {
                wr |= *(tr->out++);
            }

            /* If this is *not* the last byte, remove the EOT flag if any. */
            tr->tr_len--;
            if (tr->tr_len) {
                wr &= ~SPI_FIFOWR_EOT_MASK;
            }
            /* Push the data to the FIFO. */
            spi_bus->FIFOWR = wr;
        }
    }
    _spi_wait_txempty(spi_bus);
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    spi_pending_transfer_t tr;

    _spi_config_transfer(&tr, cs, cont, out, in, len);

    /* At least one of input or one output buffer is given */
    assert(bus < SPI_NUMOF);

    if (!GPIO_T_IS_HWCS(cs)) {
        /* Assert CS using a gpio. */
        gpio_clear((gpio_t)cs);
    }

    DEBUG("[spi] transfer: cs=0x%.4" PRIx16 " cont=%d len=%" PRIu32 "\n",
          cs, cont, (uint32_t)len);
    _spi_transfer_blocking(bus, &tr);

    /* Deassert the CS only in gpio mode. HWCS deassert are handled by the
     * hardware when EOT is set in the mask. */
    if (!cont && !GPIO_T_IS_HWCS(cs)) {
        gpio_set((gpio_t)cs);
    }
}

/* ISR routine called for FLEXCOMM devices configured as SPI. */
void isr_flexcomm_spi(USART_Type *dev, uint32_t flexcomm_num)
{
    // TODO: Set up async mode with interrupts.
    (void)dev;
    (void)flexcomm_num;

    cortexm_isr_end();
}

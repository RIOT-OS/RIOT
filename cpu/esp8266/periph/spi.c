/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @ingroup     drivers_periph_spi
 * @{
 *
 * @file
 * @brief       Low-level SPI driver implementation
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#define ENABLE_DEBUG (0)
#include "debug.h"
#include "common.h"
#include "log.h"

#if defined(MODULE_PERIPH_SPI)

#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#include "esp/iomux_regs.h"
#include "esp/spi_regs.h"

#include "gpio_common.h"

#define SPI_BUS_NUM     2
#define SPI_BLOCK_SIZE  64         /* number of bytes per SPI transfer */

static mutex_t _spi_lock[SPI_BUS_NUM] = { MUTEX_INIT };

/* indicate whether SPI interface were already initilized */
static bool _spi_initialized[SPI_BUS_NUM] = { false };

/* indicate whether pins of the SPI interface were already initilized */
static bool _spi_pins_initialized[SPI_BUS_NUM] = { false };

/*
 * GPIOs that were once initialized as SPI interface pins can not be used
 * afterwards for anything else. Therefore, SPI interfaces are not initialized
 * until they are used for the first time. The *spi_init* function is just a
 * dummy for source code compatibility. The initialization of an SPI interface
 * is performed by the *_spi_init_internal* function, which is called either by
 * the *spi_init_cs* function or the *spi_acquire* function when the interface
 * is used for the first time.
 */
void IRAM spi_init (spi_t bus)
{
    return;
}

void _spi_init_internal(spi_t bus)
{
    /* only one physical SPI(1) bus (HSPI) can be used for peripherals */
    /* RIOT's SPI_DEV(0) is mapped to SPI(1) bus (HSPI) */
    /* TODO SPI overlap mode SPI and HSPI */
    CHECK_PARAM (bus == SPI_DEV(0));

    /* avoid multiple initializations */
    if (_spi_initialized[bus]) {
        return;
    }
    _spi_initialized[bus] = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    /* initialize pins */
    spi_init_pins(bus);

    /* check whether pins could be initialized, otherwise return, CS is not
       initialized in spi_init_pins */
    if (_gpio_pin_usage[SPI0_SCK_GPIO] != _SPI &&
        _gpio_pin_usage[SPI0_MOSI_GPIO] != _SPI &&
        _gpio_pin_usage[SPI0_MISO_GPIO] != _SPI) {
        return;
    }

    /* set bus into a defined state */
    SPI(bus).USER0  = SPI_USER0_MOSI | SPI_USER0_CLOCK_IN_EDGE | SPI_USER0_DUPLEX;
    SPI(bus).USER0 |= SPI_USER0_CS_SETUP | SPI_USER0_CS_HOLD;

    /* set byte order to little endian for read and write operations */
    SPI(bus).USER0 &= ~(SPI_USER0_WR_BYTE_ORDER | SPI_USER0_RD_BYTE_ORDER);

    /* set bit order to most significant first for read and write operations */
    SPI(bus).CTRL0 = 0; /* ~(SPI_CTRL0_WR_BIT_ORDER | SPI_CTRL0_RD_BIT_ORDER); */

    DEBUG("%s SPI(bus).USER0=%08x SPI(bus).CTRL0=%08x\n",
          __func__,  SPI(bus).USER0, SPI(bus).CTRL0);
}

void spi_init_pins(spi_t bus)
{
    /* see spi_init */
    CHECK_PARAM (bus == SPI_DEV(0));

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi_initialized[bus]) {
        _spi_init_internal(bus);
    }

    /* avoid multiple pin initializations */
    if (_spi_pins_initialized[bus]) {
        return;
    }
    _spi_pins_initialized[bus] = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    uint32_t iomux_func = (bus == 0) ? IOMUX_FUNC(1) : IOMUX_FUNC(2);

    /*
     * CS is handled as normal GPIO ouptut. Due to the small number of GPIOs
     * we have, we do not initialize the default CS pin here. Either the app
     * uses spi_init_cs to initialize the CS pin explicitly, or we initialize
     * the default CS when spi_aquire is used first time.
     */
    IOMUX.PIN[_gpio_to_iomux[SPI0_MISO_GPIO]] &= ~IOMUX_PIN_FUNC_MASK;
    IOMUX.PIN[_gpio_to_iomux[SPI0_MOSI_GPIO]] &= ~IOMUX_PIN_FUNC_MASK;
    IOMUX.PIN[_gpio_to_iomux[SPI0_SCK_GPIO]]  &= ~IOMUX_PIN_FUNC_MASK;

    IOMUX.PIN[_gpio_to_iomux[SPI0_MISO_GPIO]] |= iomux_func;
    IOMUX.PIN[_gpio_to_iomux[SPI0_MOSI_GPIO]] |= iomux_func;
    IOMUX.PIN[_gpio_to_iomux[SPI0_SCK_GPIO]]  |= iomux_func;

    _gpio_pin_usage [SPI0_MISO_GPIO] = _SPI;  /* pin cannot be used for anything else */
    _gpio_pin_usage [SPI0_MOSI_GPIO] = _SPI;  /* pin cannot be used for anything else */
    _gpio_pin_usage [SPI0_SCK_GPIO]  = _SPI;  /* pin cannot be used for anything else */
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    DEBUG("%s bus=%u cs=%u\n", __func__, bus, cs);

    /* see spi_init */
    CHECK_PARAM_RET (bus == SPI_DEV(0), SPI_NODEV);

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi_initialized[bus]) {
        _spi_init_internal(bus);
    }

    /* return if pin is already initialized as SPI CS signal */
    if (_gpio_pin_usage [cs] == _SPI) {
        return SPI_OK;
    }

    if (_gpio_pin_usage [cs] != _GPIO) {
        return SPI_NOCS;
    }

    gpio_init(cs, GPIO_OUT);
    gpio_set (cs);

    _gpio_pin_usage [cs]  = _SPI;  /* pin cannot be used for anything else */

    return SPI_OK;
}

int spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("%s bus=%u cs=%u mode=%u clk=%u\n", __func__, bus, cs, mode, clk);

    /* see spi_init */
    CHECK_PARAM_RET (bus == SPI_DEV(0), SPI_NODEV);

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi_initialized[bus]) {
        _spi_init_internal(bus);
    }

    /* if parameter cs is GPIO_UNDEF, the default CS pin is used */
    cs = (cs == GPIO_UNDEF) ? SPI0_CS0_GPIO : cs;

    /* if the CS pin used is not yet initialized, we do it now */
    if (_gpio_pin_usage[cs] != _SPI && spi_init_cs(bus, cs) != SPI_OK) {
        LOG_ERROR("SPI_DEV(%d) CS signal could not be initialized\n", bus);
        return SPI_NOCS;
    }

    /* lock the bus */
    mutex_lock(&_spi_lock[bus]);

    /* set SPI mode */
    bool cpha = (mode == SPI_MODE_1 || mode == SPI_MODE_3);
    bool cpol = (mode == SPI_MODE_2 || mode == SPI_MODE_3);

    if (cpol) {
        cpha = !cpha;  /* CPHA must be inverted when CPOL = 1 */
    }

    if (cpha) {
        SPI(bus).USER0 |= SPI_USER0_CLOCK_OUT_EDGE;
    }
    else {
        SPI(bus).USER0 &= ~SPI_USER0_CLOCK_OUT_EDGE;
    }

    if (cpol) {
        SPI(bus).PIN |= SPI_PIN_IDLE_EDGE;
    }
    else {
        SPI(bus).PIN &= ~SPI_PIN_IDLE_EDGE;
    }

    /* set SPI clock
     * see ESP8266 Technical Reference Appendix 2 - SPI registers
     * https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
     */

    uint32_t spi_clkdiv_pre;
    uint32_t spi_clkcnt_N;

    switch (clk) {
        case SPI_CLK_10MHZ:  spi_clkdiv_pre = 2;    /* predivides 80 MHz to 40 MHz */
                             spi_clkcnt_N = 4;      /* 4 cycles results into 10 MHz */
                             break;
        case SPI_CLK_5MHZ:   spi_clkdiv_pre = 2;    /* predivides 80 MHz to 40 MHz */
                             spi_clkcnt_N = 8;      /* 8 cycles results into 5 MHz */
                             break;
        case SPI_CLK_1MHZ:   spi_clkdiv_pre = 2;    /* predivides 80 MHz to 40 MHz */
                             spi_clkcnt_N = 40;     /* 40 cycles results into 1 MHz */
                             break;
        case SPI_CLK_400KHZ: spi_clkdiv_pre = 20;   /* predivides 80 MHz to 4 MHz */
                             spi_clkcnt_N = 10;     /* 10 cycles results into 400 kHz */
                             break;
        case SPI_CLK_100KHZ: spi_clkdiv_pre = 20;   /* predivides 80 MHz to 4 MHz */
                             spi_clkcnt_N = 40;     /* 20 cycles results into 100 kHz */
                             break;
        default: spi_clkdiv_pre = 20;   /* predivides 80 MHz to 4 MHz */
                 spi_clkcnt_N = 40;     /* 20 cycles results into 100 kHz */
    }

    /* register values are set to deviders-1 */
    spi_clkdiv_pre--;
    spi_clkcnt_N--;

    DEBUG("%s spi_clkdiv_prev=%u spi_clkcnt_N=%u\n", __func__, spi_clkdiv_pre, spi_clkcnt_N);

    /* SPI clock is derived from system bus frequency and should not be affected by */
    /* CPU clock */

    IOMUX.CONF &= ~(bus == 0 ? IOMUX_CONF_SPI0_CLOCK_EQU_SYS_CLOCK
                             : IOMUX_CONF_SPI1_CLOCK_EQU_SYS_CLOCK);
    SPI(bus).CLOCK  = VAL2FIELD_M (SPI_CLOCK_DIV_PRE, spi_clkdiv_pre) |
                      VAL2FIELD_M (SPI_CLOCK_COUNT_NUM, spi_clkcnt_N) |
                      VAL2FIELD_M (SPI_CLOCK_COUNT_HIGH, (spi_clkcnt_N+1)/2-1) |
                      VAL2FIELD_M (SPI_CLOCK_COUNT_LOW, spi_clkcnt_N);

    DEBUG("%s IOMUX.CONF=%08x SPI(bus).CLOCK=%08x\n",
          __func__, IOMUX.CONF, SPI(bus).CLOCK);

    return SPI_OK;
}

void spi_release(spi_t bus)
{
    /* see spi_init */
    CHECK_PARAM (bus == SPI_DEV(0));

    /* release the bus */
    mutex_unlock(&_spi_lock[bus]);
}

/*
 * Following functions are from the hardware SPI driver of the esp-open-rtos
 * project.
 *
 * Copyright (c) Ruslan V. Uss, 2016
 * BSD Licensed as described in the file LICENSE
 * https://github.com/SuperHouse/esp-open-rtos/blob/master/LICENSE
 */

inline static void _set_size(uint8_t bus, uint8_t bytes)
{
    uint32_t bits = ((uint32_t)bytes << 3) - 1;
    SPI(bus).USER1 = SET_FIELD(SPI(bus).USER1, SPI_USER1_MISO_BITLEN, bits);
    SPI(bus).USER1 = SET_FIELD(SPI(bus).USER1, SPI_USER1_MOSI_BITLEN, bits);
}

inline static void _wait(uint8_t bus)
{
    while (SPI(bus).CMD & SPI_CMD_USR) {}
}

inline static void _start(uint8_t bus)
{
    SPI(bus).CMD |= SPI_CMD_USR;
}

inline static void _store_data(uint8_t bus, const void *data, size_t len)
{
    uint8_t words = len / 4;
    uint8_t tail = len % 4;

    memcpy((void *)SPI(bus).W, data, len - tail);

    if (!tail) {
        return;
    }

    uint32_t last = 0;
    uint8_t *offs = (uint8_t *)data + len - tail;
    for (uint8_t i = 0; i < tail; i++) {
        last = last | (offs[i] << (i * 8));
    }
    SPI(bus).W[words] = last;
}

static const uint8_t spi_empty_out[SPI_BLOCK_SIZE] = { 0 };

static void _spi_buf_transfer(uint8_t bus, const void *out, void *in, size_t len)
{
    DEBUG("%s bus=%u out=%p in=%p len=%u\n", __func__, bus, out, in, len);

    /* transfer one block data */
    _wait(bus);
    _set_size(bus, len);
    _store_data(bus, out ? out : spi_empty_out, len);
    _start(bus);
    _wait(bus);
    if (in) {
        memcpy(in, (void *)SPI(bus).W, len);
    }
}

void spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                        const void *out, void *in, size_t len)
{
    /* see spi_init */
    CHECK_PARAM (bus == SPI_DEV(0));

    DEBUG("%s bus=%u cs=%u cont=%d out=%p in=%p len=%u\n",
          __func__, bus, cs, cont, out, in, len);

    if (!len) {
        return;
    }

    #if ENABLE_DEBUG
    if (out) {
        DEBUG("out = ");
        for (size_t i = 0; i < len; i++) {
            DEBUG("%02x ", ((const uint8_t *)out)[i]);
        }
        DEBUG("\n");
    }
    #endif

    if (cs != SPI_CS_UNDEF) {
        gpio_clear (cs);
    }

    size_t blocks = len / SPI_BLOCK_SIZE;
    uint8_t tail = len % SPI_BLOCK_SIZE;

    DEBUG("%s bus=%u cs=%u blocks=%d tail=%d\n",
          __func__, bus, cs, blocks, tail);

    for (size_t i = 0; i < blocks; i++) {
        _spi_buf_transfer(bus,
                          out ? (const uint8_t *)out + i * SPI_BLOCK_SIZE : NULL,
                          in  ? (uint8_t *)in + i * SPI_BLOCK_SIZE : NULL, SPI_BLOCK_SIZE);
    }

    if (tail) {
        _spi_buf_transfer(bus,
                          out ? (const uint8_t *)out + blocks * SPI_BLOCK_SIZE : 0,
                          in  ? (uint8_t *)in + blocks * SPI_BLOCK_SIZE : NULL, tail);
    }

    if (!cont && (cs != SPI_CS_UNDEF)) {
        gpio_set (cs);
    }

    #if ENABLE_DEBUG
    if (in) {
        DEBUG("in = ");
        for (size_t i = 0; i < len; i++) {
            DEBUG("%02x ", ((const uint8_t *)in)[i]);
        }
        DEBUG("\n");
    }
    #endif
}

void spi_print_config(void)
{
    LOG_INFO("\tSPI_DEV(0): ");
    LOG_INFO("sck=%d " , SPI0_SCK_GPIO);
    LOG_INFO("miso=%d ", SPI0_MISO_GPIO);
    LOG_INFO("mosi=%d ", SPI0_MOSI_GPIO);
    LOG_INFO("cs=%d\n" , SPI0_CS0_GPIO);
}

#else /* MODULE_PERIPH_SPI */

void spi_print_config(void)
{
    LOG_INFO("\tSPI: no devices\n");
}

#endif /* MODULE_PERIPH_SPI */

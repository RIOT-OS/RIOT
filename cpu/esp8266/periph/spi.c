/*
 * Copyright (C) 2022 Gunar Schorcht
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
 * @brief       Low-level SPI driver implementation for ESP8266
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "esp_common.h"
#include "log.h"

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"
#include "macros/units.h"

#include "esp_attr.h"
#include "gpio_arch.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "esp/iomux_regs.h"
#include "esp8266/spi_register.h"
#include "esp8266/spi_struct.h"

#define SPI_DOUTDIN (BIT(0))

#define SPI_BLOCK_SIZE  64  /* number of bytes per SPI transfer */

/** structure which describes all properties of one SPI bus */
struct _spi_bus_t {
    spi_dev_t* regs;       /* pointer to register data struct of the SPI device */
    mutex_t lock;          /* mutex for each possible SPI interface */
    bool initialized;      /* interface already initialized */
    bool pins_initialized; /* pins interface initialized */
};

static struct _spi_bus_t _spi[] = {
#ifdef SPI0_CTRL
    {
        .initialized = false,
        .pins_initialized = false,
        .lock = MUTEX_INIT
    },
#endif
};

/*
 * GPIOs that were once initialized as SPI interface pins can not be used
 * afterwards for anything else. Therefore, SPI interfaces are not initialized
 * until they are used for the first time. The *spi_init* function is just a
 * dummy for source code compatibility. The initialization of an SPI interface
 * is performed by the *_spi_init_internal* function, which is called either by
 * the *spi_init_cs* function or the *spi_acquire* function when the interface
 * is used for the first time.
 */
void IRAM_ATTR spi_init(spi_t bus)
{
    assert(bus < SPI_NUMOF_MAX);
    assert(bus < SPI_NUMOF);

    if (spi_config[bus].ctrl == HSPI) {
        _spi[bus].regs = &SPI1;
    }
    else {
        LOG_TAG_ERROR("spi", "invalid SPI interface controller "
                      "used for SPI_DEV(%d)\n", bus);
    }
    return;
}

/* Internal initialization function when the interface is used the first time */
static void IRAM_ATTR _spi_init_internal(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* avoid multiple initializations */
    if (_spi[bus].initialized) {
        return;
    }
    _spi[bus].initialized = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    /* initialize pins */
    spi_init_pins(bus);

    /* check whether pins could be initialized, otherwise return, CS is not
       initialized in spi_init_pins */
    if (gpio_get_pin_usage(spi_config[bus].sck) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].miso) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].mosi) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].cs) != _SPI) {
        return;
    }

    /* bring the bus into a defined state */
    _spi[bus].regs->user.val = SPI_USR_MOSI | SPI_CK_I_EDGE | SPI_DOUTDIN |
                               SPI_CS_SETUP | SPI_CS_HOLD;

    /* set byte order to little endian for read and write operations */
    _spi[bus].regs->user.wr_byte_order = 0;
    _spi[bus].regs->user.rd_byte_order = 0;

    /* set bit order to most significant first for read and write operations */
    _spi[bus].regs->ctrl.wr_bit_order = 0;
    _spi[bus].regs->ctrl.rd_bit_order = 0;

    /* reset all DIO or QIO flags */
    _spi[bus].regs->ctrl.fread_qio = 0;
    _spi[bus].regs->ctrl.fread_dio = 0;
    _spi[bus].regs->ctrl.fread_quad = 0;
    _spi[bus].regs->ctrl.fread_dual = 0;

    /* disable fast read mode and write protection */
    _spi[bus].regs->ctrl.fastrd_mode = 0;

    /* acquire and release to set default parameters */
    spi_acquire(bus, GPIO_UNDEF, SPI_MODE_0, SPI_CLK_1MHZ);
    spi_release(bus);
}

void spi_init_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi[bus].initialized) {
        _spi_init_internal(bus);
    }

    /* avoid multiple pin initializations */
    if (_spi[bus].pins_initialized) {
        return;
    }
    _spi[bus].pins_initialized = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    if (gpio_init(spi_config[bus].sck, GPIO_OUT) ||
        gpio_init(spi_config[bus].mosi, GPIO_OUT) ||
        gpio_init(spi_config[bus].miso, GPIO_IN)) {
        LOG_TAG_ERROR("spi",
                      "SPI_DEV(%d) pins could not be initialized\n", bus);
        return;
    }
    if (spi_init_cs(bus, spi_config[bus].cs) != SPI_OK) {
        LOG_TAG_ERROR("spi",
                      "SPI_DEV(%d) CS signal could not be initialized\n",
                      bus);
        return;
    }

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(spi_config[bus].sck, _SPI);
    gpio_set_pin_usage(spi_config[bus].mosi, _SPI);
    gpio_set_pin_usage(spi_config[bus].miso, _SPI);

    /*
     * CS is handled as normal GPIO output. Due to the small number of GPIOs
     * we have, we do not initialize the default CS pin here. Either the app
     * uses spi_init_cs to initialize the CS pin explicitly, or we initialize
     * the default CS when spi_aquire is used first time.
     */
    uint32_t iomux_func = IOMUX_FUNC(2);

    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].miso]] &= ~IOMUX_PIN_FUNC_MASK;
    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].mosi]] &= ~IOMUX_PIN_FUNC_MASK;
    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].sck]]  &= ~IOMUX_PIN_FUNC_MASK;

    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].miso]] |= iomux_func;
    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].mosi]] |= iomux_func;
    IOMUX.PIN[_gpio_to_iomux[spi_config[bus].sck]]  |= iomux_func;
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    DEBUG("%s bus=%u cs=%u\n", __func__, bus, cs);

    assert(bus < SPI_NUMOF);

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi[bus].initialized) {
        _spi_init_internal(bus);
    }

    /* return if pin is already initialized as SPI CS signal */
    if (gpio_get_pin_usage(cs) == _SPI) {
        return SPI_OK;
    }

    /* check whether CS pin is used otherwise */
    if (gpio_get_pin_usage(cs) != _GPIO) {
        return SPI_NOCS;
    }

    /* initialize the pin */
    gpio_init(cs, GPIO_OUT);
    gpio_set(cs);

    /* pin cannot be used for anything else */
    gpio_set_pin_usage(cs, _SPI);

    return SPI_OK;
}

void IRAM_ATTR spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("%s bus=%u cs=%u mode=%u clk=%u\n", __func__, bus, cs, mode, clk);

    assert(bus < SPI_NUMOF);

    /* call initialization of the SPI interface if it is not initialized yet */
    if (!_spi[bus].initialized) {
        _spi_init_internal(bus);
    }

    /* if parameter cs is GPIO_UNDEF, the default CS pin is used */
    cs = (cs == GPIO_UNDEF) ? spi_config[bus].cs : cs;

    /* if the CS pin used is not yet initialized, we do it now */
    if (gpio_get_pin_usage(cs) != _SPI && spi_init_cs(bus, cs) != SPI_OK) {
        LOG_TAG_ERROR("spi",
                      "SPI_DEV(%d) CS signal could not be initialized\n",
                      bus);
        assert(0);
    }

    /* lock the bus */
    mutex_lock(&_spi[bus].lock);

    /*
     * set SPI mode
     * see ESP32 Technical Reference, Table 27 and Section 7.4.1
     */
    _spi[bus].regs->pin.ck_idle_edge = (mode == SPI_MODE_2 || mode == SPI_MODE_3);
    _spi[bus].regs->user.ck_out_edge = (mode == SPI_MODE_1 || mode == SPI_MODE_2);
    _spi[bus].regs->ctrl2.miso_delay_mode = (mode == SPI_MODE_0 || mode == SPI_MODE_3) ? 2 : 1;
    _spi[bus].regs->ctrl2.miso_delay_num = 0;
    _spi[bus].regs->ctrl2.mosi_delay_mode = 0;
    _spi[bus].regs->ctrl2.mosi_delay_num = 0;

    /* set SPI clock
     * see ESP8266 Technical Reference Appendix 2 - SPI registers
     * https://www.espressif.com/sites/default/files/documentation/esp8266-technical_reference_en.pdf
     */

    uint32_t spi_clkdiv_pre;    /* 13 bit */
    uint32_t spi_clkcnt_N;      /*  6 bit */

    spi_clkcnt_N = 2;
    spi_clkdiv_pre = (MHZ(80)/spi_clkcnt_N) / clk;

    /* register values are set to deviders-1 */
    spi_clkdiv_pre--;
    spi_clkcnt_N--;

    DEBUG("%s spi_clkdiv_prev=%u spi_clkcnt_N=%u\n",
          __func__, spi_clkdiv_pre, spi_clkcnt_N);

    IOMUX.CONF &= ~IOMUX_CONF_SPI1_CLOCK_EQU_SYS_CLOCK;

    /* SPI clock is derived from APB clock by dividers */
    _spi[bus].regs->clock.clk_equ_sysclk = 0;

    /* set SPI clock dividers */
    _spi[bus].regs->clock.clkdiv_pre = spi_clkdiv_pre;
    _spi[bus].regs->clock.clkcnt_n = spi_clkcnt_N;
    _spi[bus].regs->clock.clkcnt_h = (spi_clkcnt_N+1)/2-1;
    _spi[bus].regs->clock.clkcnt_l = spi_clkcnt_N;

    DEBUG("%s bus %d: SPI_CLOCK_REG=%08x\n",
          __func__, bus, _spi[bus].regs->clock.val);
}

void IRAM_ATTR spi_release(spi_t bus)
{
    DEBUG("%s bus=%u\n", __func__, bus);

    assert(bus < SPI_NUMOF);

    /* release the bus */
    mutex_unlock(&_spi[bus].lock);
}

static const char* _spi_names[] = { "FSPI", "HSPI" };

void spi_print_config(void)
{
    for (unsigned bus = 0; bus < SPI_NUMOF; bus++) {
        printf("\tSPI_DEV(%u)\t%s ", bus, _spi_names[spi_config[bus].ctrl]);
        printf("sck=%d ", spi_config[bus].sck);
        printf("miso=%d ", spi_config[bus].miso);
        printf("mosi=%d ", spi_config[bus].mosi);
        printf("cs=%d\n", spi_config[bus].cs);
    }
}

/*
 * Following functions are from the hardware SPI driver of the esp-open-rtos
 * project.
 *
 * Copyright (c) Ruslan V. Uss, 2016
 * BSD Licensed as described in the file LICENSE
 * https://github.com/SuperHouse/esp-open-rtos/blob/master/LICENSE
 */

inline static void IRAM_ATTR _set_size(uint8_t bus, uint8_t bytes)
{
    uint32_t bits = ((uint32_t)bytes << 3) - 1;

    _spi[bus].regs->user1.usr_mosi_bitlen = bits;
    _spi[bus].regs->user1.usr_miso_bitlen = bits;
}

inline static void IRAM_ATTR _wait(uint8_t bus)
{
    /* SPI_CMD_REG.SPI_USR is cleared when operation has been finished */
    while (_spi[bus].regs->cmd.usr) {}
}

inline static void IRAM_ATTR _start(uint8_t bus)
{
    /* set SPI_CMD_REG.SPI_USR to start an operation */
    _spi[bus].regs->cmd.usr = 1;
}

inline static void IRAM_ATTR _store_data(uint8_t bus, const void *data, size_t len)
{
    uint8_t words = len / 4;
    uint8_t tail = len % 4;

    memcpy((void *)_spi[bus].regs->data_buf, data, len - tail);

    if (!tail) {
        return;
    }

    uint32_t last = 0;
    uint8_t *offs = (uint8_t *)data + len - tail;
    for (uint8_t i = 0; i < tail; i++) {
        last = last | (offs[i] << (i * 8));
    }
    _spi[bus].regs->data_buf[words] = last;
}

static const uint8_t spi_empty_out[SPI_BLOCK_SIZE] = { 0 };

static void IRAM_ATTR _spi_buf_transfer(uint8_t bus, const void *out, void *in, size_t len)
{
    DEBUG("%s bus=%u out=%p in=%p len=%u\n", __func__, bus, out, in, len);

    /* transfer one block data */
    _wait(bus);
    _set_size(bus, len);
    _store_data(bus, out ? out : spi_empty_out, len);
    _start(bus);
    _wait(bus);
    if (in) {
        memcpy(in, (void *)_spi[bus].regs->data_buf, len);
    }
}

void IRAM_ATTR spi_transfer_bytes(spi_t bus, spi_cs_t cs, bool cont,
                                  const void *out, void *in, size_t len)
{
    assert(bus < SPI_NUMOF);

    DEBUG("%s bus=%u cs=%u cont=%d out=%p in=%p len=%u\n",
          __func__, bus, cs, cont, out, in, len);

    if (!len) {
        return;
    }

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (out) {
            DEBUG("out = ");
            for (size_t i = 0; i < len; i++) {
                DEBUG("%02x ", ((const uint8_t *)out)[i]);
            }
            DEBUG("\n");
        }
    }

    if (cs != SPI_CS_UNDEF) {
        gpio_clear(cs);
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

    if (IS_ACTIVE(ENABLE_DEBUG)) {
        if (in) {
            DEBUG("in = ");
            for (size_t i = 0; i < len; i++) {
                DEBUG("%02x ", ((const uint8_t *)in)[i]);
            }
            DEBUG("\n");
        }
    }
}

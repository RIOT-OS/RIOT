/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
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
#include "esp_common.h"
#include "log.h"

#include <string.h>

#include "cpu.h"
#include "mutex.h"
#include "periph/spi.h"

#include "driver/periph_ctrl.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"
#include "soc/spi_reg.h"
#include "soc/spi_struct.h"

#include "gpio_arch.h"

#if defined(SPI0_CTRL) || defined(SPI1_CTRL)

#define SPI_BLOCK_SIZE  64  /* number of bytes per SPI transfer */

/* pins of FSI are fixed */
#define FSPI_SCK    GPIO6
#define FSPI_MISO   GPIO7
#define FSPI_MOSI   GPIO8

/** structure which describes all properties of one SPI bus */
struct _spi_bus_t {
    spi_dev_t* regs;       /* pointer to register data struct of the SPI device */
    uint8_t mod;           /* peripheral hardware module of the SPI interface */
    uint8_t int_src;       /* peripheral interrupt source used by the SPI device */
    uint8_t signal_sck;    /* SCK signal from the controller */
    uint8_t signal_mosi;   /* MOSI signal from the controller */
    uint8_t signal_miso;   /* MISO signal to the controller */
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
    #ifdef SPI1_CTRL
    {
        .initialized = false,
        .pins_initialized = false,
        .lock = MUTEX_INIT
    },
    #endif
};

#define CHECK_SPI_DEV(bus) { \
    CHECK_PARAM(bus < SPI_NUMOF); \
    if (_spi[bus].regs == NULL) { \
        LOG_TAG_ERROR("spi", "SPI_DEV(%d) is not available\n", bus); \
        return; \
    } \
}

#define CHECK_SPI_DEV_RET(bus,error) { \
    CHECK_PARAM_RET(bus < SPI_NUMOF, error); \
    if (_spi[bus].regs == NULL) { \
        LOG_TAG_ERROR("spi", "SPI_DEV(%d) is not available\n", bus); \
        return error; \
    } \
}

/*
 * GPIOs that were once initialized as SPI interface pins can not be used
 * afterwards for anything else. Therefore, SPI interfaces are not initialized
 * until they are used for the first time. The *spi_init* function is just a
 * dummy for source code compatibility. The initialization of an SPI interface
 * is performed by the *_spi_init_internal* function, which is called either by
 * the *spi_init_cs* function or the *spi_acquire* function when the interface
 * is used for the first time.
 */

void IRAM_ATTR spi_init (spi_t bus)
{
    CHECK_PARAM(bus < SPI_NUMOF);

    switch (spi_config[bus].ctrl) {
        case HSPI:  _spi[bus].regs = &SPI2;
                    _spi[bus].mod = PERIPH_HSPI_MODULE;
                    _spi[bus].int_src = ETS_SPI2_INTR_SOURCE;
                    _spi[bus].signal_sck  = HSPICLK_OUT_IDX;
                    _spi[bus].signal_mosi = HSPID_OUT_IDX;
                    _spi[bus].signal_miso = HSPIQ_IN_IDX;
                    break;
        case VSPI:  _spi[bus].regs = &SPI3;
                    _spi[bus].mod = PERIPH_VSPI_MODULE;
                    _spi[bus].int_src = ETS_SPI3_INTR_SOURCE;
                    _spi[bus].signal_sck  = VSPICLK_OUT_IDX;
                    _spi[bus].signal_mosi = VSPID_OUT_IDX;
                    _spi[bus].signal_miso = VSPIQ_IN_IDX;
                    break;
        default:    LOG_TAG_ERROR("spi", "invalid SPI interface controller "
                                         "used for SPI_DEV(%d)\n", bus);
                    break;
    }
    return;
}

/* Internal initialization function when the interface is used the first time */
static void _spi_init_internal (spi_t bus)
{
    CHECK_SPI_DEV(bus);

    /* avoid multiple initializations */
    if (_spi[bus].initialized) {
        return;
    }
    _spi[bus].initialized = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    /* initialize pins */
    spi_init_pins(bus);

    /* check whether pins could be initialized, otherwise return */
    if (gpio_get_pin_usage(spi_config[bus].sck) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].miso) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].mosi) != _SPI &&
        gpio_get_pin_usage(spi_config[bus].cs) != _SPI) {
        return;
    }

    /* enable (power on) the according SPI module */
    periph_module_enable(_spi[bus].mod);

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
    _spi[bus].regs->ctrl.wp = 0;

    /* acquire and release to set default parameters */
    spi_acquire(bus, GPIO_UNDEF, SPI_MODE_0, SPI_CLK_1MHZ);
    spi_release(bus);
}

void spi_init_pins(spi_t bus)
{
    CHECK_SPI_DEV(bus);

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

    /* in case of SPI_DEV(2) all pins are already initialized
       as SPI pins */
    if (bus != SPI_DEV(2)) {
        /* if not already initialized as SPI, try to initialize the pins */
        if (gpio_init (spi_config[bus].sck, GPIO_OUT) ||
            gpio_init (spi_config[bus].mosi, GPIO_OUT) ||
            gpio_init (spi_config[bus].miso, GPIO_IN)) {
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

        /* connect SCK and MOSI pins to the output signal through the GPIO matrix */
        GPIO.func_out_sel_cfg[spi_config[bus].sck].func_sel = _spi[bus].signal_sck;
        GPIO.func_out_sel_cfg[spi_config[bus].mosi].func_sel = _spi[bus].signal_mosi;
        /* connect MISO input signal to the MISO pin through the GPIO matrix */
        GPIO.func_in_sel_cfg[_spi[bus].signal_miso].sig_in_sel = 1;
        GPIO.func_in_sel_cfg[_spi[bus].signal_miso].sig_in_inv = 0;
        GPIO.func_in_sel_cfg[_spi[bus].signal_miso].func_sel = spi_config[bus].miso;
    }
    else {
        LOG_TAG_WARNING("spi", "Using SPI_DEV(2) is dangerous\n");
    }
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    DEBUG("%s bus=%u cs=%u\n", __func__, bus, cs);

    CHECK_SPI_DEV_RET(bus, SPI_NODEV);

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
    gpio_set (cs);

    /* pin cannot be used for anything else */
    gpio_set_pin_usage(cs, _SPI);

    return SPI_OK;
}

int IRAM_ATTR spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("%s bus=%u cs=%u mode=%u clk=%u\n", __func__, bus, cs, mode, clk);

    CHECK_SPI_DEV_RET(bus, SPI_NODEV);

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
        return SPI_NOCS;
    }

    /* lock the bus */
    mutex_lock(&_spi[bus].lock);

    /* set SPI mode, see Table 25 and Section 7.4.2 in Technical Reference */
    _spi[bus].regs->pin.ck_idle_edge = (mode == SPI_MODE_2 || mode == SPI_MODE_3);
    _spi[bus].regs->user.ck_out_edge = (mode == SPI_MODE_1 || mode == SPI_MODE_2);
    _spi[bus].regs->ctrl2.miso_delay_mode = (mode == SPI_MODE_0 || mode == SPI_MODE_3) ? 2 : 1;
    _spi[bus].regs->ctrl2.miso_delay_num = 0;
    _spi[bus].regs->ctrl2.mosi_delay_mode = 0;
    _spi[bus].regs->ctrl2.mosi_delay_num = 0;

    /* set SPI clock, see Technical Reference */

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

    DEBUG("%s spi_clkdiv_prev=%u spi_clkcnt_N=%u\n",
          __func__, spi_clkdiv_pre, spi_clkcnt_N);

    /* SPI clock is derived from APB clock by dividers */
    _spi[bus].regs->clock.clk_equ_sysclk = 0;

    /* set SPI clock dividers */
    _spi[bus].regs->clock.clkdiv_pre = spi_clkdiv_pre;
    _spi[bus].regs->clock.clkcnt_n = spi_clkcnt_N;
    _spi[bus].regs->clock.clkcnt_h = (spi_clkcnt_N+1)/2-1;
    _spi[bus].regs->clock.clkcnt_l = spi_clkcnt_N;

    DEBUG("%s bus %d: SPI_CLOCK_REG=%08x\n",
          __func__, bus, _spi[bus].regs->clock.val);

    return SPI_OK;
}

void IRAM_ATTR spi_release(spi_t bus)
{
    CHECK_SPI_DEV(bus);

    /* release the bus */
    mutex_unlock(&_spi[bus].lock);
}

static const char* _spi_names[] = { "CSPI", "FSPI", "HSPI", "VSPI"  };

void spi_print_config(void)
{
    for (unsigned bus = 0; bus < SPI_NUMOF; bus++) {
        printf("\tSPI_DEV(%u)\t%s ", bus, _spi_names[spi_config[bus].ctrl]);
        printf("sck=%d " , spi_config[bus].sck);
        printf("miso=%d ", spi_config[bus].miso);
        printf("mosi=%d ", spi_config[bus].mosi);
        printf("cs=%d\n" , spi_config[bus].cs);
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

    _spi[bus].regs->mosi_dlen.val = bits;
    _spi[bus].regs->miso_dlen.val = bits;
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
    CHECK_SPI_DEV(bus);

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

#endif /* defined(SPI0_CTRL) || defined(SPI1_CTRL) */

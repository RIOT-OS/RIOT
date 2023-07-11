/*
 * Copyright (C) 2022 Gunar Schorcht
 *               2023 Hugues Larrive
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
 * @brief       Low-level SPI driver implementation for ESP32 SoCs
 *
 * The implementation uses the ESP-IDF Low level interface in polling mode
 * without DMA.
 *
 * @TODO
 * - transaction interrupts to avoid busy waiting in polling mode
 * - DMA transfer
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @author      Hugues Larrive <hugues.larrive@pm.me>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "esp_common.h"
#include "log.h"

#include "cpu.h"
#include "gpio_arch.h"
#include "macros/math.h"
#include "mutex.h"
/* periph/spi.h must be included after soc/rtc.h since it include macros/units.h */
#include "syscalls.h"

#include "driver/periph_ctrl.h"
#include "esp_attr.h"
#include "esp_rom_gpio.h"
#include "hal/spi_hal.h"
#include "hal/spi_types.h"
#include "soc/rtc.h" /* define an unwanted MHZ macro */

#include "esp_idf_api/gpio.h"

#undef MHZ
/* periph/spi.h already include macros/units.h which define riot's MHZ macro */
#include "periph/spi.h"

#define ENABLE_DEBUG 0
#include "debug.h"

/* Ensure that the SPIn_* symbols define SPI_DEV(n) */
#if !defined(SPI0_CTRL) && defined(SPI1_CTRL)
#error "SPI_DEV(1) is used but SPI_DEV(0) is not defined"
#endif

/* SPI bus descriptor structure */
struct _spi_bus_t {
    mutex_t lock;                    /* mutex for each SPI interface */
    spi_host_device_t hostid;        /* SPI hostid as used by ESP-IDF */
    const spi_signal_conn_t *periph; /* SPI peripheral descriptor */
    uint8_t mode_last;               /* SPI mode used last time */
    bool pins_initialized;           /* SPI pins initialized */
};

static struct _spi_bus_t _spi[] = {
#ifdef SPI0_CTRL
    {
        .pins_initialized = false,
        .lock = MUTEX_INIT_LOCKED,
        .hostid = spi_config[0].ctrl,
        .periph = &spi_periph_signal[spi_config[0].ctrl],
        .mode_last = UINT8_MAX,
    },
#endif
#ifdef SPI1_CTRL
    {
        .pins_initialized = false,
        .lock = MUTEX_INIT_LOCKED,
        .hostid = spi_config[1].ctrl,
        .periph = &spi_periph_signal[spi_config[1].ctrl],
        .mode_last = UINT8_MAX,
    },
#endif
};

_Static_assert(SPI_NUMOF == ARRAY_SIZE(_spi),
               "Size of bus descriptor table doesn't match SPI_NUMOF");
_Static_assert(SPI_NUMOF <= SPI_NUMOF_MAX,
               "Number of defined SPI devices is greater than the number of supported devices");

void IRAM_ATTR spi_init(spi_t bus)
{
    DEBUG("%s bus=%u\n", __func__, bus);

    assert(bus < SPI_NUMOF_MAX);
    assert(bus < SPI_NUMOF);

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

    /* enable (power on) the according SPI module */
    periph_module_enable(_spi[bus].periph->module);

    /* initialize SPI peripheral */
    spi_ll_master_init(_spi[bus].periph->hw);

    /* bring the bus into a defined state (one-line mode) */
    spi_ll_master_set_line_mode(_spi[bus].periph->hw, (spi_line_mode_t){ 1, 1, 1 });
    spi_ll_set_rx_lsbfirst(_spi[bus].periph->hw, false);
    spi_ll_set_tx_lsbfirst(_spi[bus].periph->hw, false);

    /* acquire and release to set default parameters */
    spi_acquire(bus, GPIO_UNDEF, SPI_MODE_0, spi_get_clk(bus, SPI_CLK_100KHZ));
    spi_release(bus);

    return;
}

static int _init_spi_pin(gpio_t pin, gpio_mode_t mode)
{
    int res;

    if (!gpio_is_valid(pin)) {
        return 0;
    }

    res = gpio_init(pin, mode);
    if (res) {
        return res;
    }

    /* store the usage type in GPIO table */
    gpio_set_pin_usage(pin, _SPI);
    return 0;
}

void spi_init_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* avoid multiple pin initializations */
    if (_spi[bus].pins_initialized) {
        return;
    }
    _spi[bus].pins_initialized = true;

    DEBUG("%s bus=%u\n", __func__, bus);

    if (_init_spi_pin(spi_config[bus].sck, GPIO_OUT) ||
        _init_spi_pin(spi_config[bus].mosi, GPIO_OUT) ||
        _init_spi_pin(spi_config[bus].miso, GPIO_IN)) {
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

    /* TODO  the IO_MUX should be used instead of GPIO matrix routing for
             lower delays and higher clock rates whenever possible */

    /* connect SCK and MOSI pins to the output signal through the GPIO matrix */
    if (gpio_is_valid(spi_config[bus].sck)) {
        esp_rom_gpio_connect_out_signal(spi_config[bus].sck,
                                        _spi[bus].periph->spiclk_out, false, false);
    }
    if (gpio_is_valid(spi_config[bus].mosi)) {
        esp_rom_gpio_connect_out_signal(spi_config[bus].mosi,
                                        _spi[bus].periph->spid_out, false, false);
    }

    /* connect MISO input signal to the MISO pin through the GPIO matrix */
    if (gpio_is_valid(spi_config[bus].miso)) {
        esp_rom_gpio_connect_in_signal(spi_config[bus].miso,
                                       _spi[bus].periph->spiq_in, false);
    }

    mutex_unlock(&_spi[bus].lock);
}

int spi_init_cs(spi_t bus, spi_cs_t cs)
{
    DEBUG("%s bus=%u cs=%u\n", __func__, bus, cs);

    assert(bus < SPI_NUMOF);

    if (!gpio_is_valid(cs)) {
        return SPI_OK;
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

void spi_deinit_pins(spi_t bus)
{
    assert(bus < SPI_NUMOF);

    /* avoid multiple pin deinitializations */
    if (!_spi[bus].pins_initialized) {
        return;
    }
    _spi[bus].pins_initialized = false;

    if (gpio_is_valid(spi_config[bus].sck)) {
        esp_idf_gpio_reset_pin(spi_config[bus].sck);
        gpio_set_pin_usage(spi_config[bus].sck, _GPIO);
    }

    if (gpio_is_valid(spi_config[bus].mosi)) {
        esp_idf_gpio_reset_pin(spi_config[bus].mosi);
        gpio_set_pin_usage(spi_config[bus].mosi, _GPIO);
    }

    if (gpio_is_valid(spi_config[bus].miso)) {
        esp_idf_gpio_reset_pin(spi_config[bus].miso);
        gpio_set_pin_usage(spi_config[bus].miso, _GPIO);
    }

    if (gpio_is_valid(spi_config[bus].cs)) {
        esp_idf_gpio_reset_pin(spi_config[bus].cs);
        gpio_set_pin_usage(spi_config[bus].cs, _GPIO);
    }

    mutex_lock(&_spi[bus].lock);
}

spi_clk_t IRAM_ATTR spi_get_clk(spi_t bus, uint32_t freq)
{
    (void)bus;
    /*
     * set SPI clock
     * see ESP32 Technical Reference, Section 7.8 SPI_CLOCK_REG
     * https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
     */

    uint32_t apb_clk = rtc_clk_apb_freq_get();
    uint32_t clk_reg;

    /* Section 7.4 GP­SPI Clock Control:
     * f_spi = f_apb / ((spi_clkcnt_n + 1) * (cpi__clkdiv_pre + 1))
     * spi_clkdiv_pre 0..8191 (13 bit)
     * spi_clkcnt_N 1..63 (6 bit)
     */
    if (freq > apb_clk / 5) {
        LOG_TAG_ERROR("spi", "APB clock rate (%"PRIu32" Hz) has to be at "
                      "least 5 times SPI clock rate (%"PRIu32" Hz)\n",
                      apb_clk, freq);
        freq = apb_clk / 5;
    }
    else if (freq < DIV_ROUND_UP(apb_clk, (8192 * 64))) {
        /* As apb_clk / (8192 * 64) result in very low frequency (153 Hz for
         * an apb_clk frequency of 80 MHz), this should never happen... */
        return (spi_clk_t){ .err = -EDOM };
    }

    /* duty cycle is measured in is 1/256th, 50% = 128 */
    uint32_t _freq = spi_ll_master_cal_clock(apb_clk, freq, 128, &clk_reg);

    DEBUG("%s bus %d: SPI clock frequency: freq=%"PRIu32" eff=%"PRIu32" "
          "reg=%08"PRIx32"\n",
          __func__, bus, freq, _freq, clk_reg);

    return (spi_clk_t){ .reg_psc_bits = clk_reg };
}

int32_t IRAM_ATTR spi_get_freq(spi_t bus, spi_clk_t clk)
{
    (void)bus;
    if (clk.err) {
        return -EINVAL;
    }
    uint32_t apb_clk = rtc_clk_apb_freq_get();
    uint16_t spi_clkdiv_pre = (clk.reg_psc_bits >> 18) & 0x1fff;
    uint8_t spi_clkcnt_N = (clk.reg_psc_bits >> 12) & 0x3f;
    return (apb_clk / (spi_clkdiv_pre + 1) / (spi_clkcnt_N +1));
}

void IRAM_ATTR spi_acquire(spi_t bus, spi_cs_t cs, spi_mode_t mode, spi_clk_t clk)
{
    DEBUG("%s bus=%u cs=%u mode=%u clk=%"PRIu32"\n",
            __func__, bus, cs, mode, clk.reg_psc_bits);

    assert(bus < SPI_NUMOF);
    if (clk.err) {
        return;
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
     * see ESP32 Technical Reference, Section 7.4.1, Table 27
     * https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf
     */

    /* hardware CS not used (TODO) */
    spi_ll_master_select_cs(_spi[bus].periph->hw, INT_MAX);
    spi_ll_master_set_cs_setup(_spi[bus].periph->hw, 2);
    spi_ll_master_set_mode(_spi[bus].periph->hw, mode);
    spi_ll_set_half_duplex(_spi[bus].periph->hw, false);

    int delay_mode = (mode == SPI_MODE_0 || mode == SPI_MODE_3) ? 2 : 1;
    spi_ll_set_miso_delay(_spi[bus].periph->hw, delay_mode, 0);
    spi_ll_set_mosi_delay(_spi[bus].periph->hw, 0, 0);

    /* set SPI clock */
    spi_ll_master_set_clock_by_reg(_spi[bus].periph->hw, &clk.reg_psc_bits);

#if defined(CPU_FAM_ESP32C3) || defined(CPU_FAM_ESP32S3)
    /*
     * If the SPI mode has been changed, the clock signal is only set to the
     * correct level at the beginning of the transfer on the ESP32x3. However,
     * if a generic GPIO is used as CS signal instead of the hardware CS,
     * the CS signal is already LOW at this time. Thus, the clock signal will
     * have the wrong level when the SPI mode is changed and the CS signal
     * becomes LOW.
     * The following is a workaround by receiving a dummy byte without pulling
     * the CS signal LOW when the mode has been changed.
     */
    if (_spi[bus].mode_last != mode) {
        uint8_t temp = 0xff;
        spi_transfer_bytes(bus, GPIO_UNDEF, false, &temp, &temp, 1);
        _spi[bus].mode_last = mode;
    }
#elif defined(CPU_FAM_ESP32) || defined(CPU_FAM_ESP32S2)
    /* This workaround isn't needed on ESP32 and ESP32-S2 */
#else
#error Platform implementation is missing
#endif
}

void IRAM_ATTR spi_release(spi_t bus)
{
    DEBUG("%s bus=%u\n", __func__, bus);

    assert(bus < SPI_NUMOF);

    /* release the bus */
    mutex_unlock(&_spi[bus].lock);
}

#if defined(CPU_FAM_ESP32)
static const char* _spi_names[] = { "CSPI/FSPI", "HSPI", "VSPI"  };
#elif defined(CPU_FAM_ESP32C3)
static const char* _spi_names[] = { "SPI", "FSPI"  };
#elif defined(CPU_FAM_ESP32S2)
static const char* _spi_names[] = { "SPI", "FSPI", "HSPI" };
#elif defined(CPU_FAM_ESP32S3)
static const char* _spi_names[] = { "SPI", "FSPI", "SPI" };
#else
#error Platform implementation required
#endif

_Static_assert(ARRAY_SIZE(_spi_names) == SOC_SPI_PERIPH_NUM,
               "Number of _spi_names doesn't match SOC_SPI_PERIPH_NUM");

void spi_print_config(void)
{
    for (unsigned bus = 0; bus < SPI_NUMOF; bus++) {
        printf("\tSPI_DEV(%u)\t%s ", bus, _spi_names[_spi[bus].hostid]);
        printf("sck=%d ", spi_config[bus].sck);
        printf("miso=%d ", spi_config[bus].miso);
        printf("mosi=%d ", spi_config[bus].mosi);
        printf("cs0=%d\n", spi_config[bus].cs);
    }
}

static const uint8_t _spi_empty_out[SOC_SPI_MAXIMUM_BUFFER_SIZE] = { 0 };

static void IRAM_ATTR _spi_transfer(uint8_t bus,
                                    const void *out, void *in, size_t len)
{
    /* transfer one block with a maximum size of SOC_SPI_MAXIMUM_BUFFER_SIZE */

    DEBUG("%s bus=%u out=%p in=%p len=%u\n", __func__, bus, out, in, len);

    /* wait until an existing transfer is finished */
    while (spi_ll_get_running_cmd(_spi[bus].periph->hw)) {}

    /* prepare the transfer */
    spi_ll_set_half_duplex(_spi[bus].periph->hw, false);
    spi_ll_set_command_bitlen(_spi[bus].periph->hw, 0);
    spi_ll_set_addr_bitlen(_spi[bus].periph->hw, 0);
    spi_ll_set_mosi_bitlen(_spi[bus].periph->hw, (uint32_t)len << 3);
    spi_ll_set_miso_bitlen(_spi[bus].periph->hw, (uint32_t)len << 3);
    spi_ll_enable_mosi(_spi[bus].periph->hw, 1);

    /* write output data to the buffer of the SPI controller */
    spi_ll_write_buffer(_spi[bus].periph->hw, out ? out : _spi_empty_out, len << 3);

    /* start the transfer */
    spi_ll_master_user_start(_spi[bus].periph->hw);

    /* wait until the transfer is finished */
    while (spi_ll_get_running_cmd(_spi[bus].periph->hw)) {}

    /* read input data from  the buffer of the SPI controller */
    if (in) {
        spi_ll_read_buffer(_spi[bus].periph->hw, in, len << 3);
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

    const uint8_t *blk_out = out;
    uint8_t *blk_in = in;
    size_t idx = 0;

    while (idx < len) {
        /* maximum non-DMA transfer size is SOC_SPI_MAXIMUM_BUFFER_SIZE */
        size_t blk_len = MIN(len - idx, SOC_SPI_MAXIMUM_BUFFER_SIZE);
        _spi_transfer(bus, blk_out, blk_in, blk_len);
        blk_out = (out) ? blk_out + blk_len : NULL;
        blk_in = (in) ? blk_in + blk_len : NULL;
        idx += blk_len;
        system_wdt_feed();
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

/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @ingroup     drivers_periph_gpio
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#define ENABLE_DEBUG 0
#include "debug.h"

#include <assert.h>
#include <stdbool.h>

#include "log.h"
#include "periph/gpio.h"    /* RIOT gpio.h */

#include "esp/common_macros.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"
#include "soc/rtc_io_reg.h"
#include "soc/rtc_io_struct.h"
#include "xtensa/xtensa_api.h"

#include "esp_common.h"
#include "esp_sleep.h"
#include "adc_arch.h"
#include "adc_ctrl.h"
#include "gpio_arch.h"
#include "irq_arch.h"
#include "syscalls.h"

#define ESP_PM_WUP_PINS_ANY_HIGH    ESP_EXT1_WAKEUP_ANY_HIGH
#define ESP_PM_WUP_PINS_ALL_LOW     ESP_EXT1_WAKEUP_ALL_LOW

#ifndef ESP_PM_WUP_LEVEL
#define ESP_PM_WUP_LEVEL    ESP_PM_WUP_PINS_ANY_HIGH
#endif

#define GPIO_PRO_CPU_INTR_ENA      (BIT(2))

/* GPIO to IOMUX register mapping (see Technical Reference, Section 4.12 Register Summary)
   https://www.espressif.com/sites/default/files/documentation/esp32_technical_reference_manual_en.pdf */

const uint32_t _gpio_to_iomux_reg[GPIO_PIN_NUMOF] =
{
    PERIPHS_IO_MUX_GPIO0_U,     /* GPIO0 */
    PERIPHS_IO_MUX_U0TXD_U,     /* GPIO1 */
    PERIPHS_IO_MUX_GPIO2_U,     /* GPIO2 */
    PERIPHS_IO_MUX_U0RXD_U,     /* GPIO3 */
    PERIPHS_IO_MUX_GPIO4_U,     /* GPIO4 */
    PERIPHS_IO_MUX_GPIO5_U,     /* GPIO5 */
    PERIPHS_IO_MUX_SD_CLK_U,    /* GPIO6 used for FLASH */
    PERIPHS_IO_MUX_SD_DATA0_U,  /* GPIO7 used for FLASH */
    PERIPHS_IO_MUX_SD_DATA1_U,  /* GPIO8 used for FLASH */
    PERIPHS_IO_MUX_SD_DATA2_U,  /* GPIO9 used for FLASH in qio or qout mode */
    PERIPHS_IO_MUX_SD_DATA3_U,  /* GPIO10 used for FLASH in qio or qout mode */
    PERIPHS_IO_MUX_SD_CMD_U,    /* GPIO11 used for FLASH */
    PERIPHS_IO_MUX_MTDI_U,      /* GPIO12 used as JTAG for OCD */
    PERIPHS_IO_MUX_MTCK_U,      /* GPIO13 used as JTAG for OCD  */
    PERIPHS_IO_MUX_MTMS_U,      /* GPIO14 used as JTAG for OCD  */
    PERIPHS_IO_MUX_MTDO_U,      /* GPIO15 used as JTAG for OCD  */
    PERIPHS_IO_MUX_GPIO16_U,    /* GPIO16 used as CS for PSRAM TODO */
    PERIPHS_IO_MUX_GPIO17_U,    /* GPIO17 */
    PERIPHS_IO_MUX_GPIO18_U,    /* GPIO18 */
    PERIPHS_IO_MUX_GPIO19_U,    /* GPIO19 */
    0,                          /* GPIO20 not available */
    PERIPHS_IO_MUX_GPIO21_U,    /* GPIO21 */
    PERIPHS_IO_MUX_GPIO22_U,    /* GPIO22 */
    PERIPHS_IO_MUX_GPIO23_U,    /* GPIO23 */
    0,                          /* GPIO24 not available */
    PERIPHS_IO_MUX_GPIO25_U,    /* GPIO25 */
    PERIPHS_IO_MUX_GPIO26_U,    /* GPIO26 */
    PERIPHS_IO_MUX_GPIO27_U,    /* GPIO27 */
    0,                          /* GPIO28 not available */
    0,                          /* GPIO29 not available */
    0,                          /* GPIO30 not available */
    0,                          /* GPIO31 not available */
    PERIPHS_IO_MUX_GPIO32_U,    /* GPIO32 */
    PERIPHS_IO_MUX_GPIO33_U,    /* GPIO33 */
    PERIPHS_IO_MUX_GPIO34_U,    /* GPIO34 */
    PERIPHS_IO_MUX_GPIO35_U,    /* GPIO35 */
    PERIPHS_IO_MUX_GPIO36_U,    /* GPIO36 */
    PERIPHS_IO_MUX_GPIO37_U,    /* GPIO37 */
    PERIPHS_IO_MUX_GPIO38_U,    /* GPIO38 */
    PERIPHS_IO_MUX_GPIO39_U,    /* GPIO39 */
};

/*
 * Following table defines which GPIOs have to be handled using RTC_GPIO
 * registers since pull-up and pull-down resistors for pads with both GPIO
 * and RTC_GPIO functionality can only be controlled via RTC_GPIO registers
 * https://www.espressif.com/sites/default/files/documentation/eco_and_workarounds_for_bugs_in_esp32_en.pdf
 *
 * The table contains the RTC_GPIO num or -1 if it is not an RTC_GPIO pin.
 */

static const int8_t _gpio_to_rtc[GPIO_PIN_NUMOF] = {
    11,  /* gpio0 */
    -1,  /* gpio1 */
    12,  /* gpio2 */
    -1,  /* gpio3 */
    10,  /* gpio4 */
    -1,  /* gpio5 */
    -1,  /* gpio6 */
    -1,  /* gpio7 */
    -1,  /* gpio8 */
    -1,  /* gpio9 */
    -1,  /* gpio10 */
    -1,  /* gpio11 */
    15,  /* gpio12 */
    14,  /* gpio13 */
    16,  /* gpio14 */
    13,  /* gpio15 */
    -1,  /* gpio16 */
    -1,  /* gpio17 */
    -1,  /* gpio18 */
    -1,  /* gpio19 */
    -1,  /* gpio20 */
    -1,  /* gpio21 */
    -1,  /* gpio22 */
    -1,  /* gpio23 */
    -1,  /* gpio24 */
     6,  /* gpio25 */
     7,  /* gpio26 */
    17,  /* gpio27 */
    -1,  /* gpio28 */
    -1,  /* gpio29 */
    -1,  /* gpio30 */
    -1,  /* gpio31 */
     9,  /* gpio32 */
     8,  /* gpio33 */
     4,  /* gpio34 */
     5,  /* gpio35 */
     0,  /* gpio36 */
     1,  /* gpio37 */
     2,  /* gpio38 */
     9   /* gpio39 */
};

/**
 * @brief   Register information type for RTCIO GPIO pins (for internal use only)
 */
struct _rtc_gpio_t {
    uint8_t  num;      /**< RTC_GPIO pin number */
    uint32_t reg;      /**< register of the RTC_GPIO pin */
    uint8_t  mux;      /**< mux io/rtc bit [0..31] in the register, 32 - no mux */
    uint8_t  pullup;   /**< pullup bit [0..31] in the register, 32 - no pullup */
    uint8_t  pulldown; /**< pulldown bit [0..31] in the register, 32 - no pulldown */
    uint8_t  drive;    /**< drive strength start bit [0..30] in the register */
};

/* Table of RTCIO GPIO pins information */
static const struct _rtc_gpio_t _rtc_gpios[] = {
    {  0, RTC_IO_SENSOR_PADS_REG, 27, 32, 32, 32 }, /* rtc0 (gpio36) SENSOR_VP/SENSE 1 */
    {  1, RTC_IO_SENSOR_PADS_REG, 26, 32, 32, 32 }, /* rtc1 (gpio37) SENSOR_CAPP/SENSE 2 */
    {  2, RTC_IO_SENSOR_PADS_REG, 25, 32, 32, 32 }, /* rtc2 (gpio38) SENSOR_CAPN/SENSE 3 */
    {  3, RTC_IO_SENSOR_PADS_REG, 24, 32, 32, 32 }, /* rtc3 (gpio39) SENSOR_VN/SENSE 4*/
    {  4, RTC_IO_ADC_PAD_REG, 29, 32, 32, 32 },     /* rtc4 (gpio34) VDET_1/ADC1 */
    {  5, RTC_IO_ADC_PAD_REG, 28, 32, 32, 32 },     /* rtc5 (gpio35) VDET_2/ADC2 */
    {  6, RTC_IO_PAD_DAC1_REG, 17, 27, 28, 30 },    /* rtc6 (gpio25) DAC1 */
    {  7, RTC_IO_PAD_DAC2_REG, 17, 27, 28, 30 },    /* rtc7 (gpio26) DAC1 */
    {  8, RTC_IO_XTAL_32K_PAD_REG, 18, 27, 28, 30 },/* rtc8 (gpio33) XTAL_32K_N */
    {  9, RTC_IO_XTAL_32K_PAD_REG, 17, 22, 23, 25 },/* rtc9 (gpio32) XTAL_32K_P */
    { 10, RTC_IO_TOUCH_PAD0_REG, 19, 27, 28, 29 },  /* rtc10 (gpio4) TOUCH0 */
    { 11, RTC_IO_TOUCH_PAD1_REG, 19, 27, 28, 29 },  /* rtc11 (gpio0) TOUCH1 */
    { 12, RTC_IO_TOUCH_PAD2_REG, 19, 27, 28, 29 },  /* rtc12 (gpio2) TOUCH2 */
    { 13, RTC_IO_TOUCH_PAD3_REG, 19, 27, 28, 29 },  /* rtc13 (gpio15) TOUCH3 */
    { 14, RTC_IO_TOUCH_PAD4_REG, 19, 27, 28, 29 },  /* rtc14 (gpio13) TOUCH4 */
    { 15, RTC_IO_TOUCH_PAD5_REG, 19, 27, 28, 29 },  /* rtc15 (gpio12) TOUCH5 */
    { 16, RTC_IO_TOUCH_PAD6_REG, 19, 27, 28, 29 },  /* rtc16 (gpio14) TOUCH6 */
    { 17, RTC_IO_TOUCH_PAD7_REG, 19, 27, 28, 29 }   /* rtc17 (gpio27) TOUCH7 */
};

/* Table of the usage type of each GPIO pin */
gpio_pin_usage_t _gpio_pin_usage [GPIO_PIN_NUMOF] = {
    _GPIO,        /* gpio0 */
    _UART,        /* gpio1 configured as direct I/O UART0 RxD */
    _GPIO,        /* gpio2 */
    _UART,        /* gpio3 configured as direct I/O UART0 TxD */
    _GPIO,        /* gpio4 */
    _GPIO,        /* gpio5 configurable as direct I/O VSPI CS0 */
    _SPIF,        /* gpio6 not configurable, used as SPI SCK */
    _SPIF,        /* gpio7 not configurable, used as SPI MISO */
    _SPIF,        /* gpio8 not configurable, used as SPI MOSI */
    #if defined(FLASH_MODE_QIO) || defined(FLASH_MODE_QOUT)
    /* in qio and qout mode these pins are used for quad SPI */
    _SPIF,        /* gpio9 not configurable, used as SPI HD */
    _SPIF,        /* gpio10 not configurable, used as SPI WP */
    #else
    /* otherwise these pins can be used as GPIO */
    _GPIO,        /* gpio9 */
    _GPIO,        /* gpio10 */
    #endif
    _SPIF,        /* gpio11 not configurable, used as SPI CS0 */
    _GPIO,        /* gpio12 configurable as direct I/O HSPI MISO */
    _GPIO,        /* gpio13 configurable as direct I/O HSPI MOSI */
    _GPIO,        /* gpio14 configurable as direct I/O HSPI SCK */
    _GPIO,        /* gpio15 configurable as direct I/O HSPI CS0 */
    _GPIO,        /* gpio16 */
    _GPIO,        /* gpio17 */
    _GPIO,        /* gpio18 configurable as direct I/O VSPI SCK */
    _GPIO,        /* gpio19 configurable as direct I/O VSPI MISO */
    _NOT_EXIST,   /* gpio20 */
    _GPIO,        /* gpio21 */
    _GPIO,        /* gpio22 */
    _GPIO,        /* gpio23 configurable as direct I/O VSPI MOSI */
    _NOT_EXIST,   /* gpio24 */
    _GPIO,        /* gpio25 */
    _GPIO,        /* gpio26 */
    _GPIO,        /* gpio27 */
    _NOT_EXIST,   /* gpio28 */
    _NOT_EXIST,   /* gpio29 */
    _NOT_EXIST,   /* gpio30 */
    _NOT_EXIST,   /* gpio31 */
#if MODULE_ESP_RTC_TIMER_32K
    _NOT_EXIST,   /* gpio32 used for external 32K crystal */
    _NOT_EXIST,   /* gpio33 used for external 32K crystal */
#else
    _GPIO,        /* gpio32 */
    _GPIO,        /* gpio33 */
#endif
    _GPIO,        /* gpio34 */
    _GPIO,        /* gpio35 */
    _GPIO,        /* gpio36 */
    _GPIO,        /* gpio37 */
    _GPIO,        /* gpio38 */
    _GPIO         /* gpio39 */
};

/* String representation of usage types */
const char* _gpio_pin_usage_str[] =
{
    "GPIO", "ADC", "CAN", "DAC", "EMAC", "I2C", "PWM", "SPI", "SPI Flash", "UART", "N/A"
};

#define FUN_GPIO   2 /* the function number for all GPIOs */

#define GPIO_PIN_SET(b) if (b < 32) GPIO.out_w1ts = BIT(b); else GPIO.out1_w1ts.val = BIT(b-32)
#define GPIO_PIN_CLR(b) if (b < 32) GPIO.out_w1tc = BIT(b); else GPIO.out1_w1tc.val = BIT(b-32)
#define GPIO_PIN_GET(b) (b < 32) ? (GPIO.out >> b) & 1 : (GPIO.out1.val >> (b-32)) & 1

#define GPIO_REG_BIT_GET(l,h,b) ((b < 32) ? GPIO.l & BIT(b) : GPIO.h.val & BIT(b-32))
#define GPIO_REG_BIT_SET(l,h,b) if (b < 32) GPIO.l |=  BIT(b); else GPIO.h.val |=  BIT(b-32)
#define GPIO_REG_BIT_CLR(l,h,b) if (b < 32) GPIO.l &= ~BIT(b); else GPIO.h.val &= ~BIT(b-32)
#define GPIO_REG_BIT_XOR(l,h,b) if (b < 32) GPIO.l ^=  BIT(b); else GPIO.h.val ^=  BIT(b-32)
#define REG_SET_CLR_BIT(c,r,f) if (c) REG_SET_BIT(r,f); else REG_CLR_BIT(r,f)

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    CHECK_PARAM_RET(pin < GPIO_PIN_NUMOF, -1);

    /* check if the pin can be used as GPIO or if it is used for something else */
    if (_gpio_pin_usage[pin] != _GPIO) {
        LOG_TAG_ERROR("gpio", "GPIO%d is already used as %s signal\n", pin,
                      _gpio_pin_usage_str[_gpio_pin_usage[pin]]);
        return -1;
    }

    /* check additional limitations for GPIOs 34 ... 39 */
    if (pin > GPIO33) {
        switch (mode) {
            case GPIO_OUT:
            case GPIO_OD:
            case GPIO_OD_PU:
            case GPIO_IN_OUT:
            case GPIO_IN_OD:
            case GPIO_IN_OD_PU:
                /* GPIOs 34 ... 39 cannot be used as output */
                LOG_TAG_ERROR("gpio",
                              "GPIO%d can only be used as input\n", pin);
                return -1;

            case GPIO_IN_PD:
            case GPIO_IN_PU:
                /* GPIOs 34 ... 39 have no software controllable pullups/pulldowns */
                LOG_TAG_ERROR("gpio",
                              "GPIO%d has no pullups/pulldowns\n", pin);
                return -1;
            default: break;
        }
    }

    const struct _rtc_gpio_t* rtc = (_gpio_to_rtc[pin] != -1) ?
                                    &_rtc_gpios[_gpio_to_rtc[pin]] : NULL;

    /* if pin is a RTC_GPIO, reset RTC function to route RTC pin to GPIO pin */
    if (rtc) {
        REG_CLR_BIT(rtc->reg, BIT(rtc->mux));
   }

    switch (mode) {

        case GPIO_IN:
        case GPIO_IN_PD:
        case GPIO_IN_PU:
            /* according to Technical Reference it is not necessary
               to configure the GPIO matrix to read GPIOs from
               GPIO_IN_REG/GPIO_IN1_REG */
            #if 0
            /* configure the GPIO matrix for the inputs */
            GPIO.func_in_sel_cfg[signal].sig_in_sel = 0; /* route through GPIO matrix */
            GPIO.func_in_sel_cfg[signal].sig_in_inv = 0; /* do not invert input */
            GPIO.func_in_sel_cfg[signal].func_sel = pin; /* connect signal to GPIOx */
            #endif

            /* disable the output for input-only signals */
            GPIO.func_out_sel_cfg[pin].oen_sel = 1;
            GPIO_REG_BIT_SET(enable_w1tc, enable1_w1tc, pin);

            /* set the FUN_IE bit for input */
            REG_SET_BIT(_gpio_to_iomux_reg[pin], FUN_IE);

            /* FUN_GPIO / FUN_WPU / FUN_WPD are set later */
            break;

        case GPIO_OUT:
        case GPIO_OD:
        case GPIO_OD_PU:
            /* configure GPIO signal in the GPIO matrix */
            GPIO.func_out_sel_cfg[pin].func_sel = SIG_GPIO_OUT_IDX;

            /* enable the output */
            GPIO.func_out_sel_cfg[pin].oen_sel = 1;
            GPIO_REG_BIT_SET(enable_w1ts, enable1_w1ts, pin);

            /* set pad driver to one for open drain outputs */
            GPIO.pin[pin].pad_driver = (mode == GPIO_OD || mode == GPIO_OD_PU) ? 1 : 0;

            /* set pad driver also for RTC pin if it is a RTC_GPIO */
            if (rtc) {
               RTCIO.pin[rtc->num].pad_driver = GPIO.pin[pin].pad_driver;
            }

            /* clear the FUN_IE bit */
            REG_CLR_BIT(_gpio_to_iomux_reg[pin], FUN_IE);

            /* FUN_GPIO / FUN_WPU / FUN_WPD are set later */
            break;

        case GPIO_IN_OUT:
        case GPIO_IN_OD:
        case GPIO_IN_OD_PU:
            /* configure GPIO signal in the GPIO matrix */
            GPIO.func_out_sel_cfg[pin].func_sel = SIG_GPIO_OUT_IDX;

            /* enable the output */
            GPIO.func_out_sel_cfg[pin].oen_sel = 1;
            GPIO_REG_BIT_SET(enable_w1ts, enable1_w1ts, pin);

            /* set pad driver to one for open drain outputs */
            GPIO.pin[pin].pad_driver = (mode == GPIO_IN_OD || mode == GPIO_IN_OD_PU) ? 1 : 0;

            /* set pad driver also for RTC pin if it is a RTC_GPIO */
            if (rtc) {
               RTCIO.pin[rtc->num].pad_driver = GPIO.pin[pin].pad_driver;
            }

            /* enable the input */
            REG_SET_BIT(_gpio_to_iomux_reg[pin], FUN_IE);

            /* FUN_GPIO / FUN_WPU / FUN_WPD are set later */
            break;
    }
    /* select GPIO as IO_MUX function (FUN_GPIO) */
    REG_SET_FIELD(_gpio_to_iomux_reg[pin], MCU_SEL, FUN_GPIO);

    /* enable/disable the pull-up resistor (FUN_WPU) */
    REG_SET_CLR_BIT(mode == GPIO_IN_PU || mode == GPIO_OD_PU || mode == GPIO_IN_OD_PU,
                    _gpio_to_iomux_reg[pin], FUN_PU);

    /* enable/disable the pull-down resistor (FUN_WPD) */
    REG_SET_CLR_BIT(mode == GPIO_IN_PD, _gpio_to_iomux_reg[pin], FUN_PD);

    /* handle pull-up/pull-down resistors for RTC_GPIOs */
    if (rtc) {
        /* enable/disable the pull-up resistor (FUN_WPU) */
        REG_SET_CLR_BIT(mode == GPIO_IN_PU || mode == GPIO_OD_PU || mode == GPIO_IN_OD_PU,
                        rtc->reg, BIT(rtc->pullup));
        /* enable/disable the pull-down resistor (FUN_WPD) */
        REG_SET_CLR_BIT(mode == GPIO_IN_PD, rtc->reg, BIT(rtc->pulldown));
    }

    return 0;
}

#if MODULE_PERIPH_GPIO_IRQ

static gpio_isr_ctx_t gpio_isr_ctx_table [GPIO_PIN_NUMOF] = { };
static bool gpio_int_enabled_table [GPIO_PIN_NUMOF] = { };

void IRAM gpio_int_handler (void* arg)
{
    irq_isr_enter();
    (void)arg;

    for (unsigned i = 0; i < GPIO_PIN_NUMOF; i++) {
        if (GPIO_REG_BIT_GET(status, status1, i)) {
            GPIO_REG_BIT_SET(status_w1tc, status1_w1tc, i);
            if (gpio_int_enabled_table[i] && GPIO.pin[i].int_type) {
                gpio_isr_ctx_table[i].cb (gpio_isr_ctx_table[i].arg);
            }
        }
    }
    irq_isr_exit();
}

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    if (gpio_init(pin, mode)) {
        return -1;
    }

    gpio_isr_ctx_table[pin].cb  = cb;
    gpio_isr_ctx_table[pin].arg = arg;

    GPIO.pin[pin].int_type = flank;
    if (flank != GPIO_NONE) {
        gpio_int_enabled_table [pin] = (gpio_isr_ctx_table[pin].cb != NULL);
        GPIO.pin[pin].int_ena = GPIO_PRO_CPU_INTR_ENA;
        GPIO.pin[pin].int_type = flank;
        GPIO.pin[pin].wakeup_enable = 1;

        intr_matrix_set(PRO_CPU_NUM, ETS_GPIO_INTR_SOURCE, CPU_INUM_GPIO);
        xt_set_interrupt_handler(CPU_INUM_GPIO, gpio_int_handler, NULL);
        xt_ints_on(BIT(CPU_INUM_GPIO));
    }

    return 0;
}

void gpio_irq_enable (gpio_t pin)
{
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);

    gpio_int_enabled_table [pin] = true;
}

void gpio_irq_disable (gpio_t pin)
{
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);

    gpio_int_enabled_table [pin] = false;
}

#endif /* MODULE_PERIPH_GPIO_IRQ */

int gpio_read (gpio_t pin)
{
    CHECK_PARAM_RET(pin < GPIO_PIN_NUMOF, -1);
    int value;

    if (REG_GET_BIT(_gpio_to_iomux_reg[pin], FUN_IE)) {
        /* in case the pin is any kind of input, read from input register */
        value = GPIO_REG_BIT_GET(in, in1, pin) ? 1 : 0;
    }
    else {
        /* otherwise read the last value written to the output register */
        value = GPIO_PIN_GET(pin);
    }
    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    return value;
}

void gpio_write (gpio_t pin, int value)
{
    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);
    if (value) {
        GPIO_PIN_SET(pin);
    }
    else {
        GPIO_PIN_CLR(pin);
    }
}

void gpio_set (gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);
    GPIO_PIN_SET(pin);
}

void gpio_clear (gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);
    GPIO_PIN_CLR(pin);

}

void gpio_toggle (gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);
    GPIO_REG_BIT_XOR(out, out1, pin);
}

int gpio_set_pin_usage(gpio_t pin, gpio_pin_usage_t usage)
{
    CHECK_PARAM_RET(pin < GPIO_PIN_NUMOF, -1);
    _gpio_pin_usage [pin] = usage;
    return 0;
}

gpio_pin_usage_t gpio_get_pin_usage (gpio_t pin)
{
    return (pin < GPIO_PIN_NUMOF) ? _gpio_pin_usage[pin] : _NOT_EXIST;
}

const char* gpio_get_pin_usage_str(gpio_t pin)
{
    return _gpio_pin_usage_str[_gpio_pin_usage[((pin < GPIO_PIN_NUMOF) ? pin : _NOT_EXIST)]];
}

void gpio_pullup_dis (gpio_t pin)
{
    CHECK_PARAM(pin < GPIO_PIN_NUMOF);

    const struct _rtc_gpio_t* rtc = (_gpio_to_rtc[pin] != -1) ?
                                     &_rtc_gpios[_gpio_to_rtc[pin]] : NULL;

    REG_CLR_BIT(_gpio_to_iomux_reg[pin], FUN_PU);
    if (rtc) {
        REG_CLR_BIT(rtc->reg, BIT(rtc->pullup));
    }
}

int8_t gpio_is_rtcio (gpio_t pin)
{
    return _gpio_to_rtc[pin];
}

int gpio_set_direction(gpio_t pin, gpio_mode_t mode)
{
    /* TODO implementation, for the moment we simply initialize the GPIO */
    return gpio_init(pin, mode);
}

int gpio_set_drive_capability(gpio_t pin, gpio_drive_strength_t drive)
{
    assert(pin < GPIO_PIN_NUMOF);
    assert(pin < GPIO34);

    const struct _rtc_gpio_t* rtc = (_gpio_to_rtc[pin] != -1) ?
                                     &_rtc_gpios[_gpio_to_rtc[pin]] : NULL;

    SET_PERI_REG_BITS(_gpio_to_iomux_reg, FUN_DRV_V, drive, FUN_DRV_S);
    if (rtc) {
        SET_PERI_REG_BITS(rtc->reg, 0x3, drive, rtc->drive);
    }
    return 0;
}

#if MODULE_PERIPH_GPIO_IRQ
static uint32_t gpio_int_saved_type[GPIO_PIN_NUMOF];
#endif

void gpio_pm_sleep_enter(unsigned mode)
{
    /*
     * Activate the power domain for RTC peripherals either when
     * ESP_PM_GPIO_HOLD is defined or when light sleep mode is activated.
     * As long as the RTC peripherals are active, the pad state of RTC GPIOs
     * is held in deep sleep and the pad state of all GPIOs is held in light
     * sleep.
     */
#ifdef ESP_PM_GPIO_HOLD
    esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#else
    if (mode == ESP_PM_LIGHT_SLEEP) {
        esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
    }
#endif

    if (mode == ESP_PM_DEEP_SLEEP) {
#ifdef ESP_PM_WUP_PINS
        static const gpio_t wup_pins[] = { ESP_PM_WUP_PINS };
        /*
         * Prepare the wake-up pins if a single pin or a comma-separated list of
         * pins is defined for wake-up.
         */
        uint64_t wup_pin_mask = 0;
        for (unsigned i = 0; i < ARRAY_SIZE(wup_pins); i++) {
            wup_pin_mask |= 1ULL << wup_pins[i];
        }
        esp_sleep_enable_ext1_wakeup(wup_pin_mask, ESP_PM_WUP_LEVEL);
#endif /* ESP_PM_WUP_PINS */
    }
    else {
#if MODULE_PERIPH_GPIO_IRQ
        esp_sleep_enable_gpio_wakeup();
        for (unsigned i = 0; i < GPIO_PIN_NUMOF; i++) {
            const struct _rtc_gpio_t* rtc =
                (_gpio_to_rtc[i] != -1) ? &_rtc_gpios[_gpio_to_rtc[i]] : NULL;

            if (gpio_int_enabled_table[i] && GPIO.pin[i].int_type) {
                gpio_int_saved_type[i] = GPIO.pin[i].int_type;
                switch (GPIO.pin[i].int_type) {
                    case GPIO_FALLING:
                        GPIO.pin[i].int_type = GPIO_LOW;
                        DEBUG("%s gpio=%u GPIO_LOW\n", __func__, i);
                        break;
                    case GPIO_RISING:
                        GPIO.pin[i].int_type = GPIO_HIGH;
                        DEBUG("%s gpio=%u GPIO_HIGH\n", __func__, i);
                        break;
                    case GPIO_BOTH:
                        DEBUG("%s gpio=%u GPIO_BOTH not supported\n",
                               __func__, i);
                        break;
                    default:
                        break;
                }
                if (rtc) {
                    RTCIO.pin[rtc->num].wakeup_enable = 1;
                    RTCIO.pin[rtc->num].int_type = GPIO.pin[i].int_type;
                }
            }
        }
#endif
    }
}

void gpio_pm_sleep_exit(uint32_t cause)
{
    (void)cause;
#if MODULE_PERIPH_GPIO_IRQ
    DEBUG("%s\n", __func__);
    for (unsigned i = 0; i < GPIO_PIN_NUMOF; i++) {
        if (gpio_int_enabled_table[i]) {
            GPIO.pin[i].int_type = gpio_int_saved_type[i];
        }
    }
#endif
}

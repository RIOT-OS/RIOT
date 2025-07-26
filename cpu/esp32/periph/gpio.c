/*
 * Copyright (C) 2022 Gunar Schorcht
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
 * @brief       GPIO driver implementation for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#include <assert.h>
#include <stdbool.h>

#include "log.h"
#include "periph/gpio.h"    /* RIOT gpio.h */
#if IS_USED(MODULE_GPIO_LL)
#include "periph/gpio_ll_arch.h"
#endif

#include "driver/rtc_io.h"
#include "esp/common_macros.h"
#include "esp_intr_alloc.h"
#include "driver/rtc_io.h"
#include "hal/gpio_hal.h"
#include "hal/gpio_types.h"
#include "hal/rtc_io_types.h"
#include "rom/ets_sys.h"
#include "soc/gpio_reg.h"
#include "soc/gpio_sig_map.h"
#include "soc/gpio_struct.h"
#include "soc/io_mux_reg.h"
#if !CPU_FAM_ESP32H2 && !CPU_FAM_ESP32C6
#include "soc/rtc_cntl_reg.h"
#endif
#include "soc/rtc_io_periph.h"

#if __xtensa__
#include "xtensa/xtensa_api.h"
#endif

#include "bitarithm.h"
#include "bitfield.h"
#include "board.h"
#include "esp_common.h"
#include "esp_sleep.h"
#include "gpio_arch.h"
#include "irq_arch.h"
#include "syscalls.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#if SOC_PM_SUPPORT_EXT1_WAKEUP
#if CPU_FAM_ESP32
#define ESP_PM_WUP_PINS_ANY_HIGH    ESP_EXT1_WAKEUP_ANY_HIGH
#define ESP_PM_WUP_PINS_ANY_LOW     -1
#define ESP_PM_WUP_PINS_ALL_LOW     ESP_EXT1_WAKEUP_ALL_LOW
#else /* CPU_FAM_ESP32 */
#define ESP_PM_WUP_PINS_ANY_HIGH    ESP_EXT1_WAKEUP_ANY_HIGH
#define ESP_PM_WUP_PINS_ANY_LOW     ESP_EXT1_WAKEUP_ANY_LOW
#define ESP_PM_WUP_PINS_ALL_LOW     -1
#endif /* CPU_FAM_ESP32 */
#elif SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP
#define ESP_PM_WUP_PINS_ANY_HIGH    ESP_GPIO_WAKEUP_GPIO_HIGH
#define ESP_PM_WUP_PINS_ANY_LOW     ESP_GPIO_WAKEUP_GPIO_LOW
#define ESP_PM_WUP_PINS_ALL_LOW     -1
#else
#define ESP_PM_WUP_PINS_ANY_HIGH    -1
#define ESP_PM_WUP_PINS_ANY_LOW     -1
#define ESP_PM_WUP_PINS_ALL_LOW     -1
#endif

#if CPU_FAM_ESP32
#if ESP_PM_WUP_LEVEL == ESP_PM_WUP_PINS_ANY_LOW
#error "ESP_PM_WUP_PINS_ANY_LOW is not allowed as ESP_PM_WUP_LEVEL."
#endif
#else
#if ESP_PM_WUP_LEVEL == ESP_PM_WUP_PINS_ALL_LOW
#error "ESP_PM_WUP_PINS_ALL_LOW is not allowed as ESP_PM_WUP_LEVEL."
#endif
#endif

#ifndef ESP_PM_WUP_LEVEL
#define ESP_PM_WUP_LEVEL            ESP_PM_WUP_PINS_ANY_HIGH
#endif

#if ESP_PM_WUP_LEVEL == -1
#error "ESP32x variant does not support this ESP_PM_WUP_LEVEL"
#endif

#define GPIO_PRO_CPU_INTR_ENA       (BIT(2))

/* architecture specific tables */
extern gpio_pin_usage_t _gpio_pin_usage[GPIO_PIN_NUMOF];

_Static_assert(ARRAY_SIZE(_gpio_pin_usage) == SOC_GPIO_PIN_COUNT,
               "size of _gpio_pin_usage does not match SOC_GPIO_PIN_COUNT");

#if !IS_USED(MODULE_ESP_IDF_GPIO_HAL)
extern const uint32_t _gpio_to_iomux_reg[GPIO_PIN_NUMOF];

_Static_assert(ARRAY_SIZE(_gpio_to_iomux_reg) == SOC_GPIO_PIN_COUNT,
               "size of _gpio_to_iomux_reg does not match SOC_GPIO_PIN_COUNT");
#endif

/* String representation of usage types */
const char* _gpio_pin_usage_str[] =
{
    "GPIO", "ADC", "CAN", "DAC", "EMAC", "I2C", "PWM", "SPI", "SPI Flash", "UART", "N/A"
};

#ifdef ESP_PM_WUP_PINS
/* for saving the pullup/pulldown settings of wakeup pins in deep sleep mode */
bool _gpio_pin_pu[GPIO_PIN_NUMOF] = { };
bool _gpio_pin_pd[GPIO_PIN_NUMOF] = { };
#endif

#if SOC_GPIO_PIN_COUNT > 32

static inline int _gpio_reg_in_get(gpio_t pin)
{
    return (pin < 32) ? (GPIO.in >> pin) & 1 : (GPIO.in1.val >> (pin - 32)) & 1;
}

static inline int _gpio_reg_out_get(gpio_t pin)
{
    return (pin < 32) ? (GPIO.out >> pin) & 1 : (GPIO.out1.val >> (pin - 32)) & 1;
}

static inline void _gpio_reg_out_set(gpio_t pin)
{
    if (pin < 32) {
        GPIO.out_w1ts = BIT(pin);
    }
    else {
        GPIO.out1_w1ts.val = BIT(pin - 32);
    }
}

static inline void _gpio_reg_out_clr(gpio_t pin)
{
    if (pin < 32) {
        GPIO.out_w1tc = BIT(pin);
    }
    else {
        GPIO.out1_w1tc.val = BIT(pin - 32);
    }
}

static inline void _gpio_reg_out_xor(gpio_t pin)
{
    if (pin < 32) {
        GPIO.out ^=  BIT(pin);
    }
    else {
        GPIO.out1.val ^=  BIT(pin - 32);
    }
}

#elif SOC_GPIO_PIN_COUNT < 32

static inline int _gpio_reg_in_get(gpio_t pin)
{
    return (GPIO.in.val >> pin) & 1;
}

static inline int _gpio_reg_out_get(gpio_t pin)
{
    return (GPIO.out.val >> pin) & 1;
}

static inline void _gpio_reg_out_set(gpio_t pin)
{
    GPIO.out_w1ts.val = BIT(pin);
}

static inline void _gpio_reg_out_clr(gpio_t pin)
{
    GPIO.out_w1tc.val = BIT(pin);
}

static inline void _gpio_reg_out_xor(gpio_t pin)
{
    GPIO.out.val ^=  BIT(pin);
}

#else
#error "Platform implementation is missing"
#endif

#ifndef NDEBUG
int _gpio_init_mode_check(gpio_t pin, gpio_mode_t mode)
{
    assert(pin < GPIO_PIN_NUMOF);

    /* check if the pin can be used as GPIO or if it is used for something else */
    if (_gpio_pin_usage[pin] != _GPIO) {
        LOG_TAG_ERROR("gpio", "GPIO%d is already used as %s signal\n", pin,
                      _gpio_pin_usage_str[_gpio_pin_usage[pin]]);
        return -1;
    }
    return 0;
}
#endif

int gpio_init(gpio_t pin, gpio_mode_t mode)
{
    DEBUG("%s: gpio=%d mode=%d\n", __func__, pin, mode);
    assert(_gpio_init_mode_check(pin, mode) == 0);
    assert(pin < GPIO_PIN_NUMOF);

    gpio_config_t cfg = { };

    cfg.pin_bit_mask = (1ULL << pin);

    switch (mode) {
    case GPIO_IN:
    case GPIO_IN_PD:
    case GPIO_IN_PU:
        cfg.mode = GPIO_MODE_DEF_INPUT;
        break;
    case GPIO_IN_OD:
    case GPIO_IN_OD_PU:
        cfg.mode = GPIO_MODE_DEF_INPUT | GPIO_MODE_DEF_OUTPUT | GPIO_MODE_DEF_OD;
        break;
    case GPIO_IN_OUT:
        cfg.mode = GPIO_MODE_DEF_INPUT | GPIO_MODE_DEF_OUTPUT;
        break;
    case GPIO_OUT:
        cfg.mode = GPIO_MODE_DEF_OUTPUT;
        break;
    case GPIO_OD:
    case GPIO_OD_PU:
        cfg.mode = GPIO_MODE_DEF_OUTPUT | GPIO_MODE_DEF_OD;
        break;
    }

    /* digital GPIO configuration */
    cfg.pull_up_en = ((mode == GPIO_IN_PU) ||
                      (mode == GPIO_OD_PU) ||
                      (mode == GPIO_IN_OD_PU)) ? GPIO_PULLUP_ENABLE
                                               : GPIO_PULLUP_DISABLE;
    cfg.pull_down_en = (mode == GPIO_IN_PD) ? GPIO_PULLDOWN_ENABLE
                                            : GPIO_PULLDOWN_DISABLE;
    cfg.intr_type = GPIO_INTR_DISABLE;

#ifdef ESP_PM_WUP_PINS
    /* for saving the pullup/pulldown settings of wakeup pins in deep sleep mode */
    _gpio_pin_pu[pin] = cfg.pull_up_en;
    _gpio_pin_pd[pin] = cfg.pull_down_en;
#if SOC_RTCIO_HOLD_SUPPORTED
    /* disable the RTCIO hold function for the case we come from deep sleep */
    if (rtc_gpio_is_valid_gpio(pin)) {
        rtc_gpio_hold_dis(pin);
    }
#endif /* SOC_RTCIO_HOLD_SUPPORTED */
#endif /* ESP_PM_WUP_PINS */

#ifdef ESP_PM_GPIO_HOLD
#if SOC_RTCIO_HOLD_SUPPORTED
    /* disable the RTCIO hold function for the case we come from deep sleep */
    rtc_gpio_force_hold_dis_all();
#endif
#endif

    return (gpio_config(&cfg) == ESP_OK) ? 0 : -1;
}

#if MODULE_PERIPH_GPIO_IRQ

/* interrupt enabled state is required for sleep modes */
bool gpio_int_enabled_table[GPIO_PIN_NUMOF] = { };
bool gpio_isr_service_installed = false;

int gpio_init_int(gpio_t pin, gpio_mode_t mode, gpio_flank_t flank,
                  gpio_cb_t cb, void *arg)
{
    DEBUG("%s: gpio=%d mode=%d flank=%d\n", __func__, pin, mode, flank);
    assert(_gpio_init_mode_check(pin, mode) == 0);

    if (gpio_init(pin, mode) != 0) {
        return -1;
    }

    gpio_int_type_t type = GPIO_INTR_DISABLE;

    switch (flank) {
    case GPIO_NONE:
        type = GPIO_INTR_DISABLE;
        break;
    case GPIO_RISING:
        type = GPIO_INTR_POSEDGE;
        break;
    case GPIO_FALLING:
        type = GPIO_INTR_NEGEDGE;
        break;
    case GPIO_BOTH:
        type = GPIO_INTR_ANYEDGE;
        break;
    case GPIO_LOW:
        type = GPIO_INTR_LOW_LEVEL;
        break;
    case GPIO_HIGH:
        type = GPIO_INTR_HIGH_LEVEL;
        break;
    }

    /* install GPIO ISR of ESP-IDF if not yet done */
    if (!gpio_isr_service_installed &&
        gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1) != ESP_OK) {
        return -1;
    }
    gpio_isr_service_installed = true;

    /* set the interrupt type for the pin */
    if (gpio_set_intr_type(pin, type) != ESP_OK) {
        return -1;
    }

    /* unmask and clear pending interrupts for the pin */
    if (gpio_isr_handler_add(pin, cb, arg) != ESP_OK) {
        return -1;
    }

    gpio_irq_enable(pin);

    return (gpio_int_enabled_table[pin]) ? 0 : -1;
}

void gpio_irq_enable(gpio_t pin)
{
    DEBUG("%s: gpio=%d\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);

    if (gpio_intr_enable(pin) == ESP_OK) {
        gpio_int_enabled_table[pin] = true;
    }
}

void gpio_irq_disable(gpio_t pin)
{
    DEBUG("%s: gpio=%d\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);

    if (gpio_intr_disable(pin) == ESP_OK) {
        gpio_int_enabled_table[pin] = false;
    }
}

#endif /* MODULE_PERIPH_GPIO_IRQ */

#if IS_USED(MODULE_ESP_IDF_GPIO_HAL)

static gpio_hal_context_t _gpio_hal_ctx = {
    .dev = GPIO_HAL_GET_HW(0)
};

/*
 * Since `gpio_hal_get_level` returns the current pin level and not the value
 * last written to the output, we have to handle the state of each pin in a
 * separate static variable.
 */
static BITFIELD(_output, GPIO_PIN_NUMOF);

bool gpio_read(gpio_t pin)
{
    assert(pin < GPIO_PIN_NUMOF);

    /* if the pin is not an input, it always returns 0 */
    /* TODO: not really clear whether it should return the last written value
     *       in this case. */
    int value = gpio_hal_get_level(&_gpio_hal_ctx, pin);

    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    return value;
}

void gpio_write(gpio_t pin, bool value)
{
    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    assert(pin < GPIO_PIN_NUMOF);

    if (value) {
        bf_set(_output, pin);
    }
    else {
        bf_unset(_output, pin);
    }
    gpio_hal_set_level(&_gpio_hal_ctx, pin, value);
}

void gpio_set(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);

    bf_set(_output, pin);
    gpio_hal_set_level(&_gpio_hal_ctx, pin, 1);
}

void gpio_clear(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);

    bf_unset(_output, pin);
    gpio_hal_set_level(&_gpio_hal_ctx, pin, 0);
}

void gpio_toggle(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);

    bf_toggle(_output, pin);
    gpio_hal_set_level(&_gpio_hal_ctx, pin, bf_isset(_output, pin) ? 1 : 0);
}

#else /* IS_USED(MODULE_ESP_IDF_GPIO_HAL) */

bool gpio_read(gpio_t pin)
{
    assert(pin < GPIO_PIN_NUMOF);

    int value;

    if (REG_GET_BIT(_gpio_to_iomux_reg[pin], FUN_IE)) {
        /* in case the pin is any kind of input, read from input register */
        value = _gpio_reg_in_get(pin);
    }
    else {
        /* otherwise read the last value written to the output register */
        value = _gpio_reg_out_get(pin);
    }
    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    return value;
}

void gpio_write(gpio_t pin, bool value)
{
    DEBUG("%s gpio=%u val=%d\n", __func__, pin, value);
    assert(pin < GPIO_PIN_NUMOF);
    if (value) {
        _gpio_reg_out_set(pin);
    }
    else {
        _gpio_reg_out_clr(pin);
    }
}

void gpio_set(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);
    _gpio_reg_out_set(pin);
}

void gpio_clear(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);
    _gpio_reg_out_clr(pin);

}

void gpio_toggle(gpio_t pin)
{
    DEBUG("%s gpio=%u\n", __func__, pin);
    assert(pin < GPIO_PIN_NUMOF);
    _gpio_reg_out_xor(pin);
}

#endif /* IS_USED(MODULE_ESP_IDF_GPIO_HAL) */

int gpio_set_pin_usage(gpio_t pin, gpio_pin_usage_t usage)
{
    assert(pin < GPIO_PIN_NUMOF);
    _gpio_pin_usage[pin] = usage;
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

#if MODULE_PERIPH_GPIO_IRQ
static uint32_t gpio_int_saved_type[GPIO_PIN_NUMOF];
#endif

void gpio_pm_sleep_enter(unsigned mode)
{
    if (mode == ESP_PM_DEEP_SLEEP) {
#ifdef ESP_PM_GPIO_HOLD
#if SOC_RTCIO_HOLD_SUPPORTED
        rtc_gpio_force_hold_en_all();
#if CPU_FAM_ESP32
        /* isolating GPIO12 from external circuits is especially recommended for
         * ESP32-WROVER that have an external pullup on GPIO12 */
        rtc_gpio_isolate(GPIO_NUM_12);
#elif CPU_FAM_ESP32H2
        /* On ESP32H2 rtc_gpio_force_hold_en_all doesn't enable the hold
         * function for all RTC GPIOs, so we have to do it for each pin. */
        for (unsigned i = 0; i < SOC_GPIO_PIN_COUNT; i++) {
            if (rtc_gpio_is_valid_gpio(i)) {
                rtc_gpio_hold_en(i);
            }
        }
#endif
#else
#  error "ESP32x variant does not support hold feature in deep sleep";
#endif
#endif

#ifdef ESP_PM_WUP_PINS
        static const gpio_t wup_pins[] = { ESP_PM_WUP_PINS };
        /*
         * Prepare the wake-up pins if a single pin or a comma-separated list of
         * pins is defined for wake-up.
         */
        uint64_t wup_pin_mask = 0;
        for (unsigned i = 0; i < ARRAY_SIZE(wup_pins); i++) {
            wup_pin_mask |= 1ULL << wup_pins[i];

            /* ensure that valid GPIOs are used as wake-up source */
#if SOC_PM_SUPPORT_EXT1_WAKEUP
            if (rtc_io_num_map[wup_pins[i]] < 0) {
                LOG_ERROR("GPIO%u is not a valid wake-up source, valid GPIOs are:",
                          wup_pins[i]);
                for (unsigned j = 0; j < SOC_GPIO_PIN_COUNT; j++) {
                    if (rtc_io_num_map[j] != -1) {
                        printf(" GPIO%u", j);
                    }
                }
                printf("\n");
                assert(false);
            }
#elif SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK
            if (((1ULL << wup_pins[i]) & SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK) == 0) {
                LOG_ERROR("GPIO%u is not a valid wake-up source, valid GPIOs are:",
                          wup_pins[i]);

                unsigned valid_num = bitarithm_bits_set(SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK);
                uint32_t valid_mask = SOC_GPIO_DEEP_SLEEP_WAKE_VALID_GPIO_MASK;
                for (unsigned j = 0; j < valid_num ; j++) {
                    if (valid_mask & 1) {
                        printf(" GPIO%u ", j);
                    }
                    valid_mask = valid_mask >> 1;
                }
                printf("\n");
                assert(false);
            }
#endif

#if SOC_RTCIO_INPUT_OUTPUT_SUPPORTED
            /* If internal pullups/pulldowns are used, they have to be
               activated also in deep sleep mode in RTC power domain */
            bool pu_pd_used = false;

            assert(rtc_io_num_map[wup_pins[i]] >= 0);
            if (_gpio_pin_pu[wup_pins[i]]) {
                pu_pd_used = true;
                rtc_gpio_pullup_en(wup_pins[i]);
                rtc_gpio_pulldown_dis(wup_pins[i]);
            }
            else if (_gpio_pin_pd[wup_pins[i]]) {
                pu_pd_used = true;
                rtc_gpio_pullup_dis(wup_pins[i]);
                rtc_gpio_pulldown_en(wup_pins[i]);
            }
            if (pu_pd_used) {
#if SOC_PM_SUPPORT_RTC_PERIPH_PD
                /* If internal pullups/pulldowns are used, the RTC power domain
                 * must remain active in deep sleep mode if supported */
                esp_sleep_pd_config(ESP_PD_DOMAIN_RTC_PERIPH, ESP_PD_OPTION_ON);
#endif
            }
#elif SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && SOC_DEEP_SLEEP_SUPPORTED
            if (_gpio_pin_pu[wup_pins[i]]) {
                gpio_pullup_en(wup_pins[i]);
                gpio_pulldown_dis(wup_pins[i]);
            }
            else if (_gpio_pin_pd[wup_pins[i]]) {
                gpio_pullup_dis(wup_pins[i]);
                gpio_pulldown_en(wup_pins[i]);
            }
#elif SOC_GPIO_SUPPORT_HOLD_IO_IN_DSLP
            rtc_gpio_hold_en(wup_pins[i]);
#endif
        }
#if SOC_PM_SUPPORT_EXT1_WAKEUP
        /* ESP_PM_WUP_PINS_ALL_LOW or ESP_PM_WUP_PINS_ANY_HIGH */
        esp_sleep_enable_ext1_wakeup_io(wup_pin_mask, ESP_PM_WUP_LEVEL);
#elif SOC_GPIO_SUPPORT_DEEPSLEEP_WAKEUP && SOC_DEEP_SLEEP_SUPPORTED
        /* ESP_PM_WUP_PINS_ANY_LOW or ESP_PM_WUP_PINS_ANY_HIGH */
        esp_deep_sleep_enable_gpio_wakeup(wup_pin_mask, ESP_PM_WUP_LEVEL);
#else
        #error "ESP32x variant doesn't allow to define GPIOs for wake-up from deep sleep"
#endif
#endif /* ESP_PM_WUP_PINS */
    }
    else {
#if MODULE_PERIPH_GPIO_IRQ
        for (unsigned i = 0; i < GPIO_PIN_NUMOF; i++) {
            if (gpio_int_enabled_table[i] && GPIO.pin[i].int_type) {
                gpio_int_saved_type[i] = GPIO.pin[i].int_type;
                switch (gpio_int_saved_type[i]) {
                    case GPIO_LOW:
                    case GPIO_FALLING:
                        gpio_wakeup_enable(i, GPIO_INTR_LOW_LEVEL);
                        DEBUG("%s gpio=%u GPIO_LOW\n", __func__, i);
                        break;
                    case GPIO_HIGH:
                    case GPIO_RISING:
                        gpio_wakeup_enable(i, GPIO_INTR_HIGH_LEVEL);
                        DEBUG("%s gpio=%u GPIO_HIGH\n", __func__, i);
                        break;
                    case GPIO_BOTH:
                        DEBUG("%s gpio=%u GPIO_BOTH not supported\n",
                               __func__, i);
                        break;
                    default:
                        break;
                }
            }
        }
        esp_sleep_enable_gpio_wakeup();
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

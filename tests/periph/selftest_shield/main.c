/*
 * Copyright (C) 2023 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for Peripheral Self-Testing using the
 *              Peripheral Selftest Shield
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "architecture.h"
#include "arduino_iomap.h"
#include "macros/units.h"
#include "macros/utils.h"
#include "modules.h"
#include "pcf857x.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/pwm.h"
#include "periph/spi.h"
#include "periph/timer.h"
#include "periph/uart.h"
#include "stdio_uart.h" /* for STDIO_UART_DEV */
#include "tiny_strerror.h"

/* BEGIN: controls of the behavior of the testing app: */
#ifndef STOP_ON_FAILURE
#define STOP_ON_FAILURE         0
#endif

#ifndef DETAILED_OUTPUT
#define DETAILED_OUTPUT         0
#endif
/* END: controls of the behavior of the testing app: */

/* In order to run the periph_uart test all of the following needs to be true:
 * - periph_uart needs to be used
 * - an I/O mapping for the UART at D0/D1 needs to be provided
 * - this UART dev is not busy with stdio (either not using `stdio_uart` or a different UART is used for stdio)
 */
#if defined(ARDUINO_UART_D0D1) && defined(MODULE_PERIPH_UART)
#  if MODULE_STDIO_UART
#    define ENABLE_UART_TEST (STDIO_UART_DEV != ARDUINO_UART_D0D1)
#  else
#    define ENABLE_UART_TEST 1
#  endif
#  define UART_TEST_DEV ARDUINO_UART_D0D1
#else
#  define ENABLE_UART_TEST 0
#endif

/* In order to run the periph_pwm test, all of the following needs to be true:
 * - periph_pwm needs to be used (so that we actually have something to test)
 * - periph_adc needs to be used (so that we can validate the output duty cycle)
 * - At least one of:
 *     - Arduino I/O mapping for PWM on D5 *AND* analog pin A2 is provided
 *     - Arduino I/O mapping for PWM on D6 *AND* analog pin A1 is provided
 */
#if defined(MODULE_PERIPH_PWM) && defined(MODULE_PERIPH_ADC)
#  if defined(ARDUINO_PIN_5_PWM_DEV) && defined(ARDUINO_A2)
#    define ENABLE_PWM_TEST_D5  1
#  else
#    define ENABLE_PWM_TEST_D5  0
#  endif
#  if defined(ARDUINO_PIN_6_PWM_DEV) && defined(ARDUINO_A1)
#    define ENABLE_PWM_TEST_D6  1
#  else
#    define ENABLE_PWM_TEST_D6  0
#  endif
#  define ENABLE_PWM_TEST       (ENABLE_PWM_TEST_D5 || ENABLE_PWM_TEST_D6)
#else
#  define ENABLE_PWM_TEST       0
#  define ENABLE_PWM_TEST_D5    0
#  define ENABLE_PWM_TEST_D6    0
#endif

/* In order to run the periph_adc test, we need:
 * - periph_adc support (so that we have something to test)
 * - Arduino I/O mapping for ADC (so that we know which line to test)
 * - The PCF857x driver (so that we can control the R-2R resistor ladder
 *   connected to the GPIO expander).
 */
#if defined(MODULE_PERIPH_ADC) && defined(ARDUINO_A0) && defined(MODULE_PCF857X)
#  define ENABLE_ADC_TEST       1
#else
#  define ENABLE_ADC_TEST       0
#endif

/* We want the code to be compile-tested even when tests are disabled. We
 * provide dummy parameters for the tests when they are off. The actual
 * values don't matter, as the tests will be disabled anyway when the
 * parameters are missing. But having the compiler checking the syntax and
 * doing static analysis is useful in any case. */
#ifndef ARDUINO_PIN_5_PWM_DEV
#  define ARDUINO_PIN_5_PWM_DEV     -1
#  define ARDUINO_PIN_5_PWM_CHAN    -1
#endif
#ifndef ARDUINO_PIN_6_PWM_DEV
#  define ARDUINO_PIN_6_PWM_DEV     -1
#  define ARDUINO_PIN_6_PWM_CHAN    -1
#endif
#ifndef ARDUINO_A0
#  define ARDUINO_A0                -1
#endif
#ifndef ARDUINO_A1
#  define ARDUINO_A1                -1
#endif
#ifndef ARDUINO_A2
#  define ARDUINO_A2                -1
#endif
#ifndef UART_TEST_DEV
#  define UART_TEST_DEV UART_DEV(0)
#endif
#ifndef TIMER
#  if IS_USED(MODULE_ZTIMER_PERIPH_TIMER) && CONFIG_ZTIMER_USEC_DEV == TIMER_DEV(0)
#    define TIMER         TIMER_DEV(1)
#  else
#    define TIMER         TIMER_DEV(0)
#  endif
#endif

#if IS_USED(MODULE_ZTIMER_PERIPH_TIMER)
#  if CONFIG_ZTIMER_USEC_DEV == TIMER
#    error "Same timer used for ztimer and test"
#  endif
#endif

/* A higher clock frequency is beneficial in being able to actually measure the
 * difference of half a SPI clock cycle when clock phase is 1, compared to
 * clock phase being 0. Most MCUs can clock their timers from the core clock
 * directly, so the CPU clock is the highest clock frequency available. But
 * some can't, so we handle them here explicitly. */
#ifndef TIMER_FREQ_SPI_TEST
#  if defined(CPU_SAM3) || defined(CPU_STM32)
#    define TIMER_FREQ_SPI_TEST     CLOCK_CORECLOCK / 4
#  elif defined(CPU_NRF52) || defined(CPU_NRF51)
#    define TIMER_FREQ_SPI_TEST     MHZ(16)
#  else
#    define TIMER_FREQ_SPI_TEST     CLOCK_CORECLOCK
#  endif
#endif

/* for the UART test a slower frequency is more beneficial. We assume the
 * timer to be 16 bit (tossing away the upper 16 bit of 32 bit timers) to
 * ease the test. But the duration for transferring 8 bytes of UART data in
 * ticks easily overflows 16 bit at higher frequencies, so we just go for
 * a lower frequency instead. */
#ifndef TIMER_FREQ_UART_TEST
#  if defined(__AVR__)
#    define TIMER_FREQ_UART_TEST    CLOCK_CORECLOCK / 64
#  else
#    define TIMER_FREQ_UART_TEST    MHZ(1)
#  endif
#endif

static const char testdata[8] = "Selftest";
static const spi_t spi_buses[] = {
#ifdef ARDUINO_SPI_D11D12D13
    ARDUINO_SPI_D11D12D13,
#endif
#ifdef ARDUINO_SPI_ISP
    ARDUINO_SPI_ISP,
#endif
};
static const gpio_t spi_clk_check_pins[] = {
#ifdef ARDUINO_SPI_D11D12D13
    PCF857X_GPIO_PIN(0, 2),
#endif
#ifdef ARDUINO_SPI_ISP
    PCF857X_GPIO_PIN(0, 3),
#endif
};

static struct {
    char data[8];
    uint8_t pos;
} serial_buf;

/* This module is only used when both periph_i2c and the Arduino I/O
 * mapping are present. If this module is not used, this test is optimized
 * out as dead branch. We still want the compile test, though. */
static const pcf857x_params_t params = {
#ifdef MODULE_PCF857X
    .dev = ARDUINO_I2C_UNO,
    .exp = PCF857X_EXP_PCF8574,
#endif
};

static pcf857x_t egpios;

/* Some tests may pass due to luck (e.g. when expecting a pull up but input
 * is floating, it will still get lucky from time to time). We repeat those
 * in a loop to have some confidence that the test is consistently passing */
static const unsigned flaky_test_repetitions = 100;

/* We are trying to not spent much memory on error message for compatibility
 * with as many boards as possible. The idea is that the line number where a
 * test failed and careful commenting in the code provides the same level of
 * insight with only a little inconvenience of having to open the editor. */
static void print_test_failed(uint16_t line)
{
    printf("FAILURE in " __FILE__ ":%" PRIu16 "\n", line);
}

static bool do_test(bool failed, uint16_t line)
{
    if (failed) {
        print_test_failed(line);
        if (STOP_ON_FAILURE) {
            printf("Stopping, as STOP_ON_FAILURE==1\n");
            ARCHITECTURE_BREAKPOINT(1);
            while (1) {
                /* stop */
            }
        }
    }

    return failed;
}

static void MAYBE_UNUSED do_assert(bool failed, uint16_t line)
{
    if (failed) {
        printf("CRITICAL ");
        print_test_failed(line);
        ARCHITECTURE_BREAKPOINT(1);
        while (1) {
            /* stop */
        }
    }
}

static void do_assert_no_error(int retval, uint16_t line)
{
    if (retval != 0) {
        printf("ERROR in " __FILE__ ":%" PRIu16 " with code %s\n",
               line, tiny_strerror(retval));
        ARCHITECTURE_BREAKPOINT(1);
        while (1) {
            /* stop */
        }
    }
}

static void print_result(bool failed)
{
    if (failed) {
        printf("[FAILED]\n");
    }
    else {
        printf("[OK]\n");
    }
}

static void print_skipped(void)
{
    printf("(skipped)\n");
}

static void _print_start(const char *name, const char *detail, uint16_t line)
{
    if (DETAILED_OUTPUT) {
        printf("Starting test for %s (%s) at " __FILE__ ":%" PRIu16"\n", name,
                detail, line);
    }
    else {
        printf("Starting test for %s at " __FILE__ ":%" PRIu16"\n", name,
                line);
    }
}

#if DETAILED_OUTPUT
#  define print_start(name, detail) _print_start(name, detail, __LINE__)
#else
#  define print_start(name, detail) _print_start(name, NULL, __LINE__)
#endif

/**
 * @brief   Expression @p x must evaluate, otherwise fail but continue other
 *          tests
 *
 * @pre     The test is safe to continue even if the test fails
 */
#define TEST(x) do_test(!(x), __LINE__)
/**
 * @brief   Expression @p x must evaluate, otherwise fail and abort
 *
 * @pre     The test is ***NOT*** safe to continue if the test fails
 */
#define ASSERT(x) do_assert(!(x), __LINE__)
/**
 * @brief   The expression @p must return 0, otherwise abort
 *
 * @pre     The return value will be a positive or negative errno code, if it
 *          is not zero
 *
 * This prints the errno code and aborts if @p x is not evaluating to zero
 */
#define ASSERT_NO_ERROR(x) do_assert_no_error(x, __LINE__)

static void stupid_delay(unsigned count)
{
    while (count > 0) {
        /* tell optimizer that the value of `count` is used and changed, so
         * that the down-counting loop is not detected as dead code */
        __asm__ volatile (
            ""
            /* outputs: */
            : "+r"(count)
            /* inputs: */
            :
            /* clobbers: */
        );
        count--;
    }
}

static void brief_delay(void)
{
    stupid_delay(100);
}

static void long_delay(void)
{
    stupid_delay(10000);
}

static bool periph_gpio_test_push_pull(void)
{
    bool failed = false;
    print_start("GPIO", "push-pull");
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_3, GPIO_IN));
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));

    gpio_clear(ARDUINO_PIN_4);
    for (unsigned i = 0; i < flaky_test_repetitions; i++) {
        failed |= TEST(gpio_read(ARDUINO_PIN_3) == 0);
    }

    gpio_set(ARDUINO_PIN_4);
    for (unsigned i = 0; i < flaky_test_repetitions; i++) {
        failed |= TEST(gpio_read(ARDUINO_PIN_3) != 0);
    }

    print_result(failed);

    return failed;
}

static bool periph_gpio_test_input_pull_up(void)
{
    bool failed = false;
    print_start("GPIO", "input pull-up");
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_IN));
    if (gpio_init(ARDUINO_PIN_3, GPIO_IN_PU) == 0) {
        /* give pull resistor a little time to pull */
        brief_delay();
        /* pull up should pull both D3 and D4 up, as D4 is connected to D3 via
         * the testing shield */
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_3) != 0);
            failed |= TEST(gpio_read(ARDUINO_PIN_4) != 0);
        }

        /* push/pull on D4 should still be able to force down D3 */
        ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));
        gpio_clear(ARDUINO_PIN_4);
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_3) == 0);
        }
        print_result(failed);
    }
    else {
        print_skipped();
    }

    return failed;
}

static bool periph_gpio_test_input_pull_down(void)
{
    bool failed = false;
    print_start("GPIO", "input pull-down");
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_IN));
    if (gpio_init(ARDUINO_PIN_3, GPIO_IN_PD) == 0) {
        /* give pull resistor a little time to pull */
        brief_delay();
        /* pull down should pull both D3 and D4 down, as D4 is connected to D3
         * via the testing shield */
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_3) == 0);
            failed |= TEST(gpio_read(ARDUINO_PIN_4) == 0);
        }

        /* push/pull on D4 should still be able to force up D3 */
        ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));
        gpio_set(ARDUINO_PIN_4);
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_3) != 0);
        }
        print_result(failed);
    }
    else {
        print_skipped();
    }

    return failed;
}

static bool periph_gpio_test_open_drain_pull_up(void)
{
    bool failed = false;
    print_start("GPIO", "open-drain pull-up");

    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_IN));
    if (gpio_init(ARDUINO_PIN_3, GPIO_OD_PU) == 0) {
        gpio_set(ARDUINO_PIN_3);
        /* give pull resistor a little time to pull */
        brief_delay();
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_4) != 0);
        }
        gpio_clear(ARDUINO_PIN_3);
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_4) == 0);
        }
        print_result(failed);
    }
    else {
        print_skipped();
    }

    return failed;
}

static bool periph_gpio_test_open_drain_no_pull_up(void)
{
    bool failed = false;
    print_start("GPIO", "open-drain no-pull-up");

    /* we cannot test without pull up, but the input pin may have the pull
     * up just as well */
    if ((gpio_init(ARDUINO_PIN_3, GPIO_OD) == 0)
            && (gpio_init(ARDUINO_PIN_4, GPIO_IN_PU))) {
        gpio_set(ARDUINO_PIN_3);
        /* give pull resistor a little time to pull */
        brief_delay();
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_4) != 0);
        }
        gpio_clear(ARDUINO_PIN_3);
        for (unsigned i = 0; i < flaky_test_repetitions; i++) {
            failed |= TEST(gpio_read(ARDUINO_PIN_4) == 0);
        }
        print_result(failed);
    }
    else {
        print_skipped();
    }

    return failed;
}

static bool periph_gpio_test(void)
{
    bool failed = false;

    failed |= periph_gpio_test_push_pull();
    failed |= periph_gpio_test_input_pull_up();
    failed |= periph_gpio_test_input_pull_down();
    failed |= periph_gpio_test_open_drain_pull_up();
    failed |= periph_gpio_test_open_drain_no_pull_up();

    return failed;
}

static void gpio_cb(void *arg)
{
    atomic_uint *cnt = arg;
    atomic_fetch_add(cnt, 1);
}

static bool periph_gpio_irq_test_falling(void)
{
    bool failed = false;
    print_start("GPIO-IRQ", "falling-edge");
    atomic_uint cnt = 0;
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));
    gpio_clear(ARDUINO_PIN_4);
    ASSERT_NO_ERROR(gpio_init_int(ARDUINO_PIN_3, GPIO_IN, GPIO_FALLING, gpio_cb, &cnt));

    /* no stray IRQ */
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 0);

    /* no IRQ on false edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 0);

    /* one IRQ on matching edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 1);

    /* still no IRQ on false edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 1);

    /* another IRQ on matching edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* no IRQs while disabled */
    gpio_irq_disable(ARDUINO_PIN_3);
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* no stray IRQs when re-enabled */
    gpio_irq_enable(ARDUINO_PIN_3);
    failed |= TEST(atomic_load(&cnt) == 2);

    /* still no IRQ on false edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* another IRQ on matching edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 3);

    /* disable IRQ again to not have side effects */
    gpio_irq_disable(ARDUINO_PIN_3);

    print_result(failed);
    return failed;
}

static bool periph_gpio_irq_test_rising(void)
{
    bool failed = false;
    print_start("GPIO-IRQ", "rising-edge");
    atomic_uint cnt = 0;
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));
    gpio_set(ARDUINO_PIN_4);
    ASSERT_NO_ERROR(gpio_init_int(ARDUINO_PIN_3, GPIO_IN, GPIO_RISING, gpio_cb, &cnt));

    /* no stray IRQ */
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 0);

    /* no IRQ on false edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 0);

    /* one IRQ on matching edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 1);

    /* still no IRQ on false edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 1);

    /* another IRQ on matching edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* no IRQs while disabled */
    gpio_irq_disable(ARDUINO_PIN_3);
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* no stray IRQs when re-enabled */
    gpio_irq_enable(ARDUINO_PIN_3);
    failed |= TEST(atomic_load(&cnt) == 2);

    /* still no IRQ on false edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* another IRQ on matching edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 3);

    /* disable IRQ again to not have side effects */
    gpio_irq_disable(ARDUINO_PIN_3);

    print_result(failed);
    return failed;
}

static bool periph_gpio_irq_test_both(void)
{
    bool failed = false;
    print_start("GPIO-IRQ", "both-edges");
    atomic_uint cnt = 0;
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_4, GPIO_OUT));
    gpio_set(ARDUINO_PIN_4);
    ASSERT_NO_ERROR(gpio_init_int(ARDUINO_PIN_3, GPIO_IN, GPIO_BOTH, gpio_cb, &cnt));

    /* no stray IRQ */
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 0);

    /* IRQ on falling edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 1);

    /* another IRQ on rising edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 2);

    /* and another IRQ on falling edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 3);

    /* and another IRQ on rising edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 4);

    /* no IRQs while disabled */
    gpio_irq_disable(ARDUINO_PIN_3);
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 4);

    /* no stray IRQs when re-enabled */
    gpio_irq_enable(ARDUINO_PIN_3);
    failed |= TEST(atomic_load(&cnt) == 4);

    /* an IRQ on falling edge */
    gpio_clear(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 5);

    /* another IRQ on rising edge */
    gpio_set(ARDUINO_PIN_4);
    brief_delay();
    failed |= TEST(atomic_load(&cnt) == 6);

    print_result(failed);
    return failed;
}

static bool periph_gpio_irq_test(void)
{
    bool failed = false;

    failed |= periph_gpio_irq_test_falling();
    failed |= periph_gpio_irq_test_rising();
    failed |= periph_gpio_irq_test_both();

    return failed;
}

static bool periph_i2c_test(void)
{
    bool failed = false;
    print_start("I2C", "GPIO extender");
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_8, GPIO_IN));
    ASSERT_NO_ERROR(gpio_init(ARDUINO_PIN_9, GPIO_OUT));
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 0), GPIO_OUT));
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 1), GPIO_IN));

    for (unsigned i = 0; i < flaky_test_repetitions; i++) {
        gpio_set(ARDUINO_PIN_9);
        failed |= TEST(pcf857x_gpio_read(&egpios, PCF857X_GPIO_PIN(0, 1)) != 0);
        gpio_clear(ARDUINO_PIN_9);
        failed |= TEST(pcf857x_gpio_read(&egpios, PCF857X_GPIO_PIN(0, 1)) == 0);
    }

    for (unsigned i = 0; i < flaky_test_repetitions; i++) {
        pcf857x_gpio_set(&egpios, PCF857X_GPIO_PIN(0, 0));
        failed |= TEST(gpio_read(ARDUINO_PIN_8) != 0);
        pcf857x_gpio_clear(&egpios, PCF857X_GPIO_PIN(0, 0));
        failed |= TEST(gpio_read(ARDUINO_PIN_8) == 0);
    }

    print_result(failed);
    return failed;
}

static void uart_rx_cb(void *arg, uint8_t data)
{
    (void)arg;
    if (serial_buf.pos < sizeof(serial_buf.data)) {
        serial_buf.data[serial_buf.pos] = data;
    }
    serial_buf.pos++;
}

static bool periph_uart_rxtx_test(uint32_t symbolrate, uint32_t timer_freq)
{
    bool failed = 0;
    uint16_t duration_ticks = 0;
    uint16_t bit_ticks = 0;
    uint16_t start;
    memset(&serial_buf, 0, sizeof(serial_buf));

    ASSERT_NO_ERROR(uart_init(UART_TEST_DEV, symbolrate, uart_rx_cb, NULL));

    if (IS_USED(MODULE_PERIPH_TIMER)) {
        bit_ticks = timer_freq / symbolrate;
        duration_ticks = 9ULL * sizeof(testdata) * bit_ticks;
    }

    /* test that data send matches data received */
    if (IS_USED(MODULE_PERIPH_TIMER)) {
        start = timer_read(TIMER);
    }
    uart_write(UART_TEST_DEV, (void *)testdata, sizeof(testdata));
    if (IS_USED(MODULE_PERIPH_TIMER)) {
        uint16_t stop = timer_read(TIMER);
        /* expecting actual duration within 75% to 200% of the expected. */
        failed |= TEST(stop - start > duration_ticks - (duration_ticks >> 2));
        failed |= TEST(stop - start < (duration_ticks << 1));
        if (failed && DETAILED_OUTPUT) {
            printf("%" PRIu32 " Bd, expected %" PRIu16 " ticks, got %" PRIu16
                  " ticks\n",
                  symbolrate, duration_ticks, (uint16_t)(stop - start));
        }
    }
    long_delay();
    failed |= TEST(memcmp(testdata, serial_buf.data, sizeof(serial_buf.data)) == 0);
    failed |= TEST(serial_buf.pos == sizeof(testdata));

    /* test that no data is received when UART is off */
    uart_poweroff(UART_TEST_DEV);
    uart_write(UART_TEST_DEV, (void *)testdata, sizeof(testdata));
    long_delay();
    failed |= TEST(serial_buf.pos == sizeof(testdata));

    /* test that data is received again when UART is back on */
    memset(&serial_buf, 0, sizeof(serial_buf));
    uart_poweron(UART_TEST_DEV);
    long_delay();
    /* no data expected yet */
    failed |= TEST(serial_buf.pos == 0);
    /* now writing with device back online */
    uart_write(UART_TEST_DEV, (void *)testdata, sizeof(testdata));
    long_delay();
    failed |= TEST(memcmp(testdata, serial_buf.data, sizeof(serial_buf.data)) == 0);
    failed |= TEST(serial_buf.pos == sizeof(testdata));

    /* disable UART again, in case it is a shared peripheral */
    uart_poweroff(UART_TEST_DEV);

    return failed;
}

static bool periph_uart_test_slow(uint32_t timer_freq)
{
    bool failed = false;
    print_start("UART", "slow");
    failed |= periph_uart_rxtx_test(9600, timer_freq);
    print_result(failed);
    return failed;
}

static bool periph_uart_test_fast(uint32_t timer_freq)
{
    bool failed = false;
    print_start("UART", "fast");
    failed |= periph_uart_rxtx_test(115200, timer_freq);
    print_result(failed);
    return failed;
}

static bool periph_uart_test(void)
{
    bool failed = false;

    uint32_t timer_freq = TIMER_FREQ_UART_TEST;

    /* Select a frequency >= TIMER_FREQ_UART_TEST that is closest to it. If no
     * such exists, select the highest supported frequency instead. */
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        timer_freq = timer_query_freqs(TIMER, 0);
        for (uword_t i = 0; i < timer_query_freqs_numof(TIMER); i++) {
            uint32_t tmp = timer_query_freqs(TIMER, i);
            if (tmp < TIMER_FREQ_UART_TEST) {
                break;
            }
            timer_freq = tmp;
        }
    }

    if (IS_USED(MODULE_PERIPH_TIMER)) {
        ASSERT_NO_ERROR(timer_init(TIMER, timer_freq, NULL, NULL));
        timer_start(TIMER);
    }

    failed |= periph_uart_test_slow(timer_freq);
    failed |= periph_uart_test_fast(timer_freq);
    return failed;
}

static bool periph_spi_rxtx_test(spi_t bus, spi_mode_t mode, spi_clk_t clk,
                                 uint32_t clk_hz, gpio_t clk_check, bool idle_level,
                                 const char *test_in_detail,
                                 uint32_t timer_freq)
{
    (void)test_in_detail;
    bool failed = false;
    bool transfer_too_fast = false;
    print_start("SPI", test_in_detail);
    uint16_t byte_transfer_ticks = 0;
    memset(&serial_buf, 0, sizeof(serial_buf));

    if (IS_USED(MODULE_PERIPH_TIMER)) {
        byte_transfer_ticks = 8ULL * timer_freq / clk_hz;
    }

    /* D10 is C̅S̅, D7 is connected to C̅S̅ */
    spi_cs_t cs = ARDUINO_PIN_10;
    gpio_t cs_check = ARDUINO_PIN_7;
    gpio_init(cs_check, GPIO_IN);
    spi_init_cs(bus, cs);

    spi_acquire(bus, cs, mode, clk);

    if (IS_USED(MODULE_PCF857X)) {
        failed |= TEST(idle_level == (bool)pcf857x_gpio_read(&egpios, clk_check));
    }

    /* C̅S̅ should still be HIGH while no chip is selected */
    failed |= TEST(gpio_read(cs_check) != 0);

    uint16_t byte_time;
    for (uint8_t i = 0; i < UINT8_MAX; i++) {
        uint16_t start = 0;
        if (IS_USED(MODULE_PERIPH_TIMER)) {
            start = timer_read(TIMER);
        }
        uint8_t received = spi_transfer_byte(bus, cs, true, i);
        uint16_t stop = 0;
        if (IS_USED(MODULE_PERIPH_TIMER)) {
            stop = timer_read(TIMER);
        }
        failed |= TEST(received == i);
        if (IS_USED(MODULE_PERIPH_TIMER)) {
            byte_time = (uint16_t)(stop - start);
            /* We allow the actual SPI clock to be slower than requested, but
             * not faster. So the transfer needs to take *at least* the
             * theoretical time. Given the overhead of, this already has some
             * room for error */
            transfer_too_fast |= (byte_time < byte_transfer_ticks);
        }
        /* C̅S̅ should be still LOW while chip is selected */
        failed |= TEST(gpio_read(cs_check) == 0);
    }

    if (DETAILED_OUTPUT && transfer_too_fast) {
        printf("Ticks expected to transfer byte: >= %" PRIu16 ", but got: %"
               PRIu16 "\n",
               byte_transfer_ticks, byte_time);
    }

    failed |= TEST(!transfer_too_fast);
    failed |= TEST(spi_transfer_byte(bus, cs, false, UINT8_MAX) == UINT8_MAX);
    /* C̅S̅ should be again HIGH while now that no chip is selected */
    failed |= TEST(gpio_read(cs_check) != 0);

    /* no also test for different sizes */
    for (unsigned i = 1; i <= sizeof(testdata); i++) {
        uint8_t target[sizeof(testdata) + 4];
        memset(target, 0x55, sizeof(target));
        /* C̅S̅ should be HIGH before chip is selected */
        failed |= TEST(gpio_read(cs_check) != 0);
        spi_transfer_bytes(bus, cs, false, testdata, target, i);
        /* C̅S̅ should be HIGH again after transfer */
        failed |= TEST(gpio_read(cs_check) != 0);

        /* first part of data read should be the test data */
        for (unsigned j = 0; j < i; j++) {
            failed |= TEST(target[j] == testdata[j]);
        }

        /* rest of the target should be the canary value */
        for (unsigned j = i; j < sizeof(testdata); j++) {
            failed |= TEST(target[j] == 0x55);
        }
    }

    if (IS_USED(MODULE_PCF857X)) {
        failed |= TEST(idle_level == (bool)pcf857x_gpio_read(&egpios, clk_check));
    }

    spi_release(bus);
    print_result(failed);

    return failed;
}

static bool periph_spi_test(void)
{

    uint32_t timer_freq = TIMER_FREQ_SPI_TEST;

    /* Select a frequency >= TIMER_FREQ_SPI_TEST that is closest to it. If no
     * such exists, select the highest supported frequency instead. */
    if (IS_USED(MODULE_PERIPH_TIMER_QUERY_FREQS)) {
        timer_freq = timer_query_freqs(TIMER, 0);
        for (uword_t i = 0; i < timer_query_freqs_numof(TIMER); i++) {
            uint32_t tmp = timer_query_freqs(TIMER, i);
            if (tmp < TIMER_FREQ_SPI_TEST) {
                break;
            }
            timer_freq = tmp;
        }
    }

    if (IS_USED(MODULE_PERIPH_TIMER)) {
        ASSERT_NO_ERROR(timer_init(TIMER, timer_freq, NULL, NULL));
        timer_start(TIMER);
    }

    bool failed = false;
    static const spi_clk_t clocks[] = { SPI_CLK_400KHZ, SPI_CLK_1MHZ, SPI_CLK_10MHZ };
    static const uint32_t clk_hzs[] = { KHZ(400), MHZ(1), MHZ(10) };

    if (IS_USED(MODULE_PCF857X)) {
        for (int i = 0; i < (int)ARRAY_SIZE(spi_clk_check_pins); i++) {
            ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, spi_clk_check_pins[i], GPIO_IN));
        }
    }

    /* using a signed comparison here to also compile when no SPI buses are
     * available for testing */
    for (int i = 0; i < (int)ARRAY_SIZE(spi_buses); i++) {
        spi_t bus = spi_buses[i];
        gpio_t clk_check = spi_clk_check_pins[i];
        for (unsigned j = 0; j < ARRAY_SIZE(clocks); j++) {
            spi_clk_t clk = clocks[j];
            uint32_t clk_hz = clk_hzs[j];
            if (DETAILED_OUTPUT) {
                printf("SPI CLK %" PRIu32 " Hz\n", clk_hz);
            }
            failed |= periph_spi_rxtx_test(bus, SPI_MODE_0, clk, clk_hz, clk_check, false, "mode 0", timer_freq);
            failed |= periph_spi_rxtx_test(bus, SPI_MODE_1, clk, clk_hz, clk_check, false, "mode 1", timer_freq);
            failed |= periph_spi_rxtx_test(bus, SPI_MODE_2, clk, clk_hz, clk_check, true, "mode 2", timer_freq);
            failed |= periph_spi_rxtx_test(bus, SPI_MODE_3, clk, clk_hz, clk_check, true, "mode 3", timer_freq);
        }
    }
    return failed;
}

static bool periph_pwm_test_chan(pwm_t pwm_dev, uint8_t pwm_chan, pwm_mode_t pwm_mode, adc_t adc_line)
{
    bool failed = false;
    print_start("PWM", "duty cycle (via ADC)");

    failed |= TEST(pwm_init(pwm_dev, pwm_mode, CLOCK_CORECLOCK, 256) > 0);
    if (failed) {
        /* pwm mode not supported, most likely */
        return failed;
    }

    ASSERT_NO_ERROR(adc_init(adc_line));

    for (uint16_t i = 0; i <= UINT8_MAX; i++) {
        pwm_set(pwm_dev, pwm_chan, i);
        /* give voltage at ADC some time to settle */
        brief_delay();
        uint16_t sample = adc_sample(adc_line, ADC_RES_10BIT);
        uint16_t expected = i << 2;

        /* let's allow for quite some error here */
        const uint16_t delta = 64;
        uint16_t lower = expected <= delta ? 0 : expected - delta;
        uint16_t upper = MIN(1023, expected + delta);
        bool test_failed = TEST((lower <= sample) && (upper >= sample));
        if (test_failed) {
            printf("%" PRIu16 " <= %" PRIu16 " <= %" PRIu16": FAILED\n",
                   lower, sample, upper);
        }
        failed |= test_failed;
    }

    print_result(failed);
    return failed;
}

static bool periph_pwm_test(void)
{
    bool failed = false;
    static const pwm_mode_t modes[] = { PWM_LEFT, PWM_RIGHT, PWM_CENTER };

    for (unsigned i = 0; i < ARRAY_SIZE(modes); i++) {
        if (ENABLE_PWM_TEST_D5) {
            failed |= periph_pwm_test_chan(ARDUINO_PIN_5_PWM_DEV, ARDUINO_PIN_5_PWM_CHAN, modes[i], ARDUINO_A2);
        }
        if (ENABLE_PWM_TEST_D6) {
            failed |= periph_pwm_test_chan(ARDUINO_PIN_6_PWM_DEV, ARDUINO_PIN_6_PWM_CHAN, modes[i], ARDUINO_A1);
        }
    }

    return failed;
}

static void r_2r_dac_init(void)
{
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 4), GPIO_OUT));
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 5), GPIO_OUT));
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 6), GPIO_OUT));
    ASSERT_NO_ERROR(pcf857x_gpio_init(&egpios, PCF857X_GPIO_PIN(0, 7), GPIO_OUT));
}

static void r_2r_dac_write(uint8_t val)
{
    pcf857x_gpio_write(&egpios, PCF857X_GPIO_PIN(0, 4), val & (1U << 3));
    pcf857x_gpio_write(&egpios, PCF857X_GPIO_PIN(0, 5), val & (1U << 2));
    pcf857x_gpio_write(&egpios, PCF857X_GPIO_PIN(0, 6), val & (1U << 1));
    pcf857x_gpio_write(&egpios, PCF857X_GPIO_PIN(0, 7), val & (1U << 0));
}

static bool periph_adc_test(void)
{
    bool failed = false;
    print_start("ADC", "sample R2R DAC output");

    adc_init(ARDUINO_A0);
    r_2r_dac_init();

    for (uint8_t i = 0; i < 16; i++) {
        r_2r_dac_write(i);
        uint16_t sample = adc_sample(ARDUINO_A0, ADC_RES_10BIT);
        uint16_t expected = i << 6;

        /* The resistors on board v0.3 are not that accurate, so allow for 10%
         * error margin */
        const uint16_t delta = 1024 / 10;
        uint16_t lower = expected <= delta ? 0 : expected - delta;
        uint16_t upper = MIN(1023, expected + delta);
        bool test_failed = TEST((lower <= sample) && (upper >= sample));
        if (test_failed) {
            printf("%" PRIu16 " <= %" PRIu16 " <= %" PRIu16": FAILED\n",
                   lower, sample, upper);
        }
        failed |= test_failed;
    }

    print_result(failed);
    return failed;
}

int main(void)
{
    bool failed = false;

    printf("self-testing peripheral drivers\n"
           "===============================\n");

    /* the GPIO extender is used by the I2C test and the ADC test, so only
     * initialize it once here */
    if (IS_USED(MODULE_PCF857X)) {
        ASSERT_NO_ERROR(pcf857x_init(&egpios, &params));
    }

    if (IS_USED(MODULE_PERIPH_GPIO)) {
        failed |= periph_gpio_test();
    }

    if (IS_USED(MODULE_PERIPH_GPIO_IRQ)) {
        failed |= periph_gpio_irq_test();
    }

    if (IS_USED(MODULE_PCF857X) && IS_USED(MODULE_PERIPH_GPIO)) {
        failed |= periph_i2c_test();
    }

    if (ENABLE_UART_TEST) {
        failed |= periph_uart_test();
    }

    if (IS_USED(MODULE_PERIPH_SPI)) {
        failed |= periph_spi_test();
    }

    if (ENABLE_PWM_TEST) {
        failed |= periph_pwm_test();
    }

    if (ENABLE_ADC_TEST) {
        failed |= periph_adc_test();
    }

    if (failed) {
        printf("\n\nSOME TESTS FAILED\n");
        return 1;
    }

    printf("\n\nALL TESTS SUCCEEDED\n");
    return 0;
}

/*
 * Copyright 2019 Marian Buschsieweke
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test application for GPIO Advanced Bitbaning Capabilities (ABC)
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 *
 * @}
 */

#include <inttypes.h>
#include <stdatomic.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "irq.h"
#include "periph/gpio_abc.h"
#include "shell.h"
#include "xtimer.h"

#define N_LOOPS                     (100U)

static int gpio_pin(int argc, char **argv);
static int gpio_abc(int argc, char **argv);
static int detect(int argc, char **argv);
static int params(int argc, char **argv);

static const shell_command_t shell_commands[] = {
    { "pin", "Select the pin to test", gpio_pin },
    { "abc", "Toggles the GPIO 100 times with the given nano-second delay", gpio_abc },
    { "detect", "Tries to detect the delay parameters", detect },
    { "params", "Prints/sets the delay parameters", params },
    { NULL, NULL, NULL }
};

static _Atomic gpio_t _gpio = GPIO_UNDEF;
static atomic_int _ticks = -1;
static char toggler_stack[THREAD_STACKSIZE_DEFAULT];
static uint16_t gpio_abc_overhead_ns = GPIO_ABC_OVERHEAD_NS;
static unsigned gpio_abc_tick_cycles = GPIO_ABC_TICK_CYCLES;

/**
 * @brief   Same as @ref gpio_abc_ticks, but with custom delay parameters
 *
 * @param   nanosecs    Length of the delay loop in nanoseconds
 * @param   overhead_ns Overhead of a call to @ref gpio_set_for in nanoseconds
 * @param   tick_cycles @ref GPIO_ABC_CLOCK cycles a delay loop spin takes
 *
 * This function allows to calculate delays with custom parameters, instead of
 * the hard wired parameters provided by `cpu_gpio_abc.h`
 */
static int _gpio_abc_ticks(uint16_t nanosecs, uint16_t overhead_ns, unsigned tick_cycles)
{
    if (nanosecs + GPIO_ABC_MIN_ACCURACY < overhead_ns) {
        return -1;
    }

    if (nanosecs < overhead_ns) {
        return 0;
    }

    uint64_t result = nanosecs - overhead_ns;
    result *= GPIO_ABC_CLOCK;
    result /= 1000000000ULL * (uint64_t)tick_cycles;

    return (int)result;
}

static void *toggler(void *arg)
{
    (void)arg;
    while (1) {
        gpio_t gpio = atomic_load(&_gpio);
        int ticks = atomic_load(&_ticks);

        if ((gpio != GPIO_UNDEF) && (ticks != -1)) {
            gpio_abc_begin();
            for (unsigned i = 0; i < N_LOOPS; i++) {
                gpio_set_for(gpio, ticks);
                gpio_clear_for(gpio, ticks);
            }
            gpio_abc_end();
        }
        xtimer_usleep(100);
    }

    return NULL;
}

static int gpio_pin(int argc, char **argv)
{
    int port, pin;
    gpio_t gpio;

    if (argc != 3) {
        printf("Usage: %s <port> <pin>\n", argv[0]);
        return EXIT_FAILURE;
    }

    if ((*argv[1] >= 'A') && (*argv[1] <= 'Z')) {
        port = (int)*argv[1] - (int)'A';
    }
    else if ((*argv[1] >= 'a') && (*argv[1] <= 'z')) {
        port = (int)*argv[1] - (int)'a';
    }
    else {
        port = atoi(argv[1]);
    }

    pin = atoi(argv[2]);

    gpio = GPIO_PIN(port, pin);

    if (gpio_init(gpio, GPIO_OUT)) {
        printf("Error to initialize P%c%d / P%d.%d\n",
               (char)('A' + port), pin, port, pin);
        return EXIT_FAILURE;
    }

    gpio_clear(gpio);
    atomic_store(&_gpio, gpio);

    return EXIT_SUCCESS;
}

static int gpio_abc(int argc, char **argv)
{
    if (argc != 2) {
        printf("Usage: %s <duration (in ns)>\n", argv[0]);
        return EXIT_FAILURE;
    }

    gpio_t gpio = atomic_load(&_gpio);
    if (gpio == GPIO_UNDEF) {
        puts("Run command \"pin\" first to select the pin to toggle");
        return EXIT_FAILURE;
    }

    uint16_t duration = (uint16_t)atoi(argv[1]);
    int ticks = _gpio_abc_ticks(duration, gpio_abc_overhead_ns,
                                gpio_abc_tick_cycles);

    if (ticks == -1) {
        printf("A pulse length of %" PRIu16 " is too short for your board\n",
               duration);
        return EXIT_FAILURE;
    }

    printf("Toggling now with pulse lengths of %" PRIu16 "ns (ticks = %d)\n",
           duration, ticks);
    printf("Expected pulse length: %uns\n",
           (unsigned)(gpio_abc_overhead_ns + (1000000000ULL * gpio_abc_tick_cycles * (uint64_t)ticks) / GPIO_ABC_CLOCK));
    atomic_store(&_ticks, ticks);

    return EXIT_SUCCESS;
}

static int detect(int argc, char **argv)
{
    static const uint32_t repetitions1 = 10000000;
    static const uint32_t repetitions2 = 10000;
    static const int ticks_array[] = { 33, 67, 100 };
    uint64_t tmp;
    uint32_t start, stop, overhead_ns, tick_cycles[3];
    if (argc != 1) {
        printf("Usage: %s\n", argv[0]);
        return EXIT_FAILURE;
    }

    gpio_t gpio = _gpio;
    if (gpio == GPIO_UNDEF) {
        puts("Run command \"pin\" first to select the pin to toggle");
        return EXIT_FAILURE;
    }

    gpio_abc_begin();
    start = xtimer_now_usec();
    for (uint32_t i = 0; i < repetitions1; i++) {
        gpio_set_for(gpio, 0);
        gpio_clear_for(gpio, 0);
    }
    stop = xtimer_now_usec();
    gpio_abc_end();

    tmp = (stop - start);
    tmp *= NS_PER_US;
    tmp = (tmp + repetitions1) / (2 * repetitions1);
    overhead_ns = tmp;

    for (unsigned i = 0; i < ARRAY_SIZE(ticks_array); i++) {
        gpio_abc_begin();
        start = xtimer_now_usec();
        int ticks = ticks_array[i];
        for (uint32_t j = 0; j < repetitions2; j++) {
            gpio_set_for(gpio, ticks);
            gpio_clear_for(gpio, ticks);
        }
        stop = xtimer_now_usec();
        gpio_abc_end();
        tmp = (stop - start);
        tmp *= NS_PER_US;
        tmp = (tmp + repetitions2) / (2 * repetitions2);
        tmp -= overhead_ns;
        tmp = (tmp * GPIO_ABC_CLOCK + 500000000ULL) / 1000000000ULL;
        tmp = (tmp + ticks / 2) / ticks;
        tick_cycles[i] = (int)tmp;
    }

    uint32_t overhead_cycles = (overhead_ns * GPIO_ABC_CLOCK + 500000000ULL)
                             / 1000000000ULL;
    puts("Detected values:");
    printf("GPIO_ABC_OVERHEAD_NS =      %" PRIu32 "\n"
           "GPIO_ABC_OVERHEAD_CYCLES =  %" PRIu32 "\n",
           overhead_ns, overhead_cycles);
    printf("GPIO_ABC_TICK_CYCLES =      %" PRIu32 "\n", tick_cycles[0]);

    if (gpio_abc_overhead_ns != overhead_ns) {
        int32_t diff = (int32_t)overhead_ns - (int32_t)gpio_abc_overhead_ns;
        printf("GPIO_ABC_OVERHEAD_NS to be adjusted by %" PRIi32 "ns\n", diff);
    }
    else {
        puts("GPIO_ABC_OVERHEAD_NS is up to date");
    }

    if (gpio_abc_tick_cycles != tick_cycles[0]) {
        int32_t diff = (int32_t)tick_cycles[0] - (int32_t)gpio_abc_tick_cycles;
        printf("GPIO_ABC_TICK_CYCLES to be adjusted by %" PRIi32 "\n", diff);
    }
    else {
        puts("GPIO_ABC_TICK_CYCLES is up to date");
    }

    if ((tick_cycles[0] != tick_cycles[1]) || (tick_cycles[1] != tick_cycles[2])) {
        puts(
            "Error: Delay does not increase exactly linear with the number of\n"
            "ticks. Check the implementation of the delay loop in\n"
            "gpio_set_for() and gpio_clear_for()\n"
        );
        printf("(Results: 1: %" PRIu32 ", 2: %" PRIu32 ", 3: %" PRIu32 ")\n",
               tick_cycles[0], tick_cycles[1], tick_cycles[2]);
        return EXIT_SUCCESS;
    }

    gpio_abc_overhead_ns = overhead_ns;
    gpio_abc_tick_cycles = tick_cycles[0];

    return EXIT_SUCCESS;
}

static int params(int argc, char **argv)
{
    if ((argc > 3) || ((argc > 1) && !strcmp("-h", argv[1]))) {
        printf("Usage: %s [GPIO_ABC_TICK_CYCLES [GPIO_ABC_OVERHEAD_NS]]\n",
               argv[0]);
        puts(
            "When called without parameters, the current values for\n"
            "GPIO_ABC_TICK_CYCLES and GPIO_ABC_OVERHEAD_NS are displayed.\n"
            "When called with one parameter, GPIO_ABC_TICK_CYCLES is updated\n"
            "to the given value.\n"
            "When called with two parameters, GPIO_ABC_TICK_CYCLES is updated\n"
            "to the value given in the first parameter and\n"
            "GPIO_ABC_OVERHEAD_NS to the value given in the second\n"
        );
        return EXIT_FAILURE;
    }

    if (argc == 3) {
        gpio_abc_overhead_ns = atoi(argv[2]);
    }

    if (argc >= 2) {
        gpio_abc_tick_cycles = atoi(argv[1]);
    }

    printf("GPIO_ABC_OVERHEAD_NS =      %" PRIu16 "\n",
           gpio_abc_overhead_ns);
    printf("GPIO_ABC_TICK_CYCLES =      %u\n", gpio_abc_tick_cycles);
    return EXIT_SUCCESS;
}

int main(void)
{
    puts(
        "GPIO Advanced Bitbanging Capabilities (ABC) Test\n"
        "================================================\n"
        "\n"
        "Prerequisites\n"
        "-------------\n"
        "\n"
        "- A digital oscilloscope or a logic analyzer with a sample rate of\n"
        "  at least 20 MHz (50ns resolution)\n"
        "- A board with GPIO ABC support\n"
        "\n"
        "Detecting GPIO ABC Parameters\n"
        "-----------------------------\n"
        "\n"
        "1. Select a GPIO pin to test using the \"pin\" command\n"
        "2. Run the \"detect\" command and wait patiently (about ten seconds\n"
        "   on an ARM board)\n"
        "    ==> The automatically detected parameters are printed\n"
        "\n"
        "Testing\n"
        "-------\n"
        "\n"
        "1. Connect the scope or the logic analyzer to an GPIO pin\n"
        "2. Setup that pin using the \"pin\" command\n"
        "3. Run \"abc\" with durations of your choosing and verify that the\n"
        "   durations match the one you specified\n"
    );

    printf("Shortest pulse:   %uns\n", (unsigned)GPIO_ABC_MIN_PULSE_LEN);
    printf("Accuracy(*):      %uns\n", (unsigned)GPIO_ABC_ACCURACY_NS);
    printf("CPU Clock:        %u %03u %03uHz\n",
           (unsigned)(CLOCK_CORECLOCK / 1000000U),
           (unsigned)((CLOCK_CORECLOCK / 1000U) % 1000U),
           (unsigned)(CLOCK_CORECLOCK % 1000000U));
    printf("GPIO ABC Clock:   %u %03u %03uHz\n",
           (unsigned)(GPIO_ABC_CLOCK / 1000000U),
           (unsigned)((GPIO_ABC_CLOCK / 1000U) % 1000U),
           (unsigned)(GPIO_ABC_CLOCK % 1000000U));
    printf("Tick Duration:    %uns\n",
           (unsigned)GPIO_ABC_TICK_NS);
    puts("\n(*) Worst case accuracy if GPIO ABC parameters are perfect");

    /* start toggling thread */
    thread_create(toggler_stack, sizeof(toggler_stack),
                  THREAD_PRIORITY_MAIN + 1, THREAD_CREATE_STACKTEST,
                  toggler, NULL, "toggler");

    /* start the shell */
    char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

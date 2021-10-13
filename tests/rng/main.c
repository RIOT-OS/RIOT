/*
 * Copyright (C) 2017 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file
 * @brief RNG testing tools.
 *
 * @author Bas Stottelaar <basstottelaar@gmail.com>
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <inttypes.h>

#include "shell.h"

#include "test.h"

/*
 * Forward declarations
 */
static int cmd_distributions(int argc, char **argv);
static int cmd_dump(int argc, char **argv);
static int cmd_entropy(int argc, char **argv);
static int cmd_fips(int argc, char **argv);
static int cmd_seed(int argc, char **argv);
static int cmd_source(int argc, char **argv);
static int cmd_speed(int argc, char **argv);

/**
 * @brief   List of command for this application.
 */
static const shell_command_t shell_commands[] = {
    { "distributions", "run distributions test", cmd_distributions },
    { "dump", "dump random numbers", cmd_dump },
    { "fips", "run FIPS 140-2 tests", cmd_fips },
    { "entropy", "calculate entropy test", cmd_entropy },
    { "seed", "set random seed", cmd_seed },
    { "source", "set randomness source", cmd_source },
    { "speed", "run speed test", cmd_speed },
    { NULL, NULL, NULL }
};

/**
 * @brief   Distributions command, which accepts one argument (samples).
 *
 * If no arguments are given, a default is used.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_distributions(int argc, char **argv)
{
    uint32_t samples = 10000;

    if (argc > 1) {
        samples = strtoul(argv[1], NULL, 0);
    }

    /* run the test */
    test_distributions(samples);

    return 0;
}

/**
 * @brief   Dump command, which accepts one argument (samples).
 *
 * If no arguments are given, a default is used.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_dump(int argc, char **argv)
{
    uint32_t samples = 100;

    if (argc < 2) {
        /* run the test */
        test_dump(samples);
    }
    else if (argc == 2) {
        samples = strtoul(argv[1], NULL, 0);
        /* run the test */
        test_dump(samples);
    }
    else if (argc == 4) {
        samples = strtoul(argv[1], NULL, 0);
        uint32_t low_thresh = strtoul(argv[2], NULL, 0);
        uint32_t high_thresh = strtoul(argv[3], NULL, 0);
        /* run the test */
        test_dump_range(samples, low_thresh, high_thresh);
    }
    else {
        printf("usage: %s [samples] [lower-bound upper-bound]\n", argv[0]);
    }

    return 0;
}

/**
 * @brief   Calculate Shannon's entropy (bits), which accepts one argument
 *          (samples).
 *
 * If no arguments are given, a default is used.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_entropy(int argc, char **argv)
{
    uint32_t samples = 10000;

    if (argc > 1) {
        samples = strtoul(argv[1], NULL, 0);
    }

    /* run the test */
    test_entropy(samples);

    return 0;
}

/**
 * @brief   Run the FIPS 140-2 tests.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_fips(int argc, char **argv)
{
    (void) argc;
    (void) argv;

    test_fips();

    return 0;
}

/**
 * @brief   Set the random seed.
 *
 * If no argument is given, the current seed is printed.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_seed(int argc, char **argv)
{
    if (argc > 1) {
        seed = strtoul(argv[1], NULL, 0);
        printf("Seed set to %" PRIu32 "\n", seed);
    }
    else {
        printf("Seed is %" PRIu32 "\n", seed);
    }

    return 0;
}

/**
 * @brief   Helper for setting the RNG source.
 *
 * If no argument is given, the list of RNG sources is printed.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_source(int argc, char **argv)
{
    if (argc > 1) {
        uint8_t raw_source = strtoul(argv[1], NULL, 0);

        if (raw_source >= RNG_NUMOF) {
            printf("Error: source must be 0-%d.\n", RNG_NUMOF - 1);
            return 1;
        }

        source = raw_source;
    }
    else {
        printf("Available sources:\n\n");

        for (int i = 0; i < RNG_NUMOF; i++) {
            printf("%d: %s", i, sources[i]);

            if ((int) source == i) {
                puts(" (selected)");
            }
            else {
                puts("");
            }
        }
    }

    return 0;
}

/**
 * @brief   Speed command, which accepts one argument (duration).
 *
 * If no argument is chosen, a default is chosen.
 *
 * @param[in] argc  Number of arguments
 * @param[in] argv  Array of arguments
 *
 * @return  0 on success
 */
static int cmd_speed(int argc, char **argv)
{
    uint32_t duration = 10;

    if (argc < 2) {
        /* run the test */
        test_speed(duration);
    }
    else if (argc == 2) {
        duration = strtoul(argv[1], NULL, 0);
        /* run the test */
        test_speed(duration);
    }
    else if (argc == 4) {
        duration = strtoul(argv[1], NULL, 0);
        uint32_t low_thresh = strtoul(argv[2], NULL, 0);
        uint32_t high_thresh = strtoul(argv[3], NULL, 0);
        /* run the test */
        test_speed_range(duration, low_thresh, high_thresh);
    }
    else {
        printf("usage: %s [duration] [lower-bound upper-bound]\n", argv[0]);
    }

    return 0;
}

int main(void)
{
    puts("Starting shell...");
    static char line_buf[SHELL_DEFAULT_BUFSIZE];
    shell_run(shell_commands, line_buf, SHELL_DEFAULT_BUFSIZE);

    return 0;
}

/*
 * Copyright (C) 2018 Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file        header
 * @brief       Implementation of getopt lib.
 *
 * This implements the getopt() and getopt_r() functionality.
 * getopt_r() has a few caveats, make sure to use it correctly.
 *
 * @author      Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
 *
 */

#ifndef GETOPT_H
#define GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Used to implement a MT safe re-entrant version of getopt().
 *
 * User can set error reporting on or off by setting opterr to 0 or 1.
 * Care must be taken to ensure that optind is initialized to atleast 1.
 * Use INIT_OPT_T or INIT_OPT_T_PTR for initialization.
 * Arguments of the option will be returned in optarg.
 */
typedef struct {
    char *optarg;
    int optind;
    int opterr;
    int optopt;
} opt_t;

/**
 * @brief Used to initialize opt_t* variable for use with getopt_r().
 *
 * Ensures that required variables pointed by opt_t* are initialized to prevent
 * undefined behavior when using getopt_r().
 */
#define INIT_OPT_T_PTR &(opt_t){ NULL, 1, 1, -1 }

/**
 * @brief Used to initialize opt_t variable for use with getopt_r().
 *
 * Ensures that required variables in opt_t are initialized to prevent
 * undefined behavior when using getopt_r().
 */
#define INIT_OPT_T { NULL, 1, 1, -1 }

extern char *optarg;
extern int optind, opterr, optopt;

/**
 * @brief      Non re-entrant version of getopt.
 *
 * @param[in]  argc           The count of the number of arguments.
 * @param[in]  argv           The array containing all the arguments.
 * @param[in]  optstring      The string of expected options.
 *
 * @return ASCII value of the option if option was successfully found.
 *         '?' or ':' when unknown option is found or argument is missing.
 */
int getopt(int argc, char *const argv[], const char *optstring);

/**
 * @brief      Re-entrant version of getopt.
 *
 * @param[in]  argc           The count of the number of arguments.
 * @param[in]  argv           The array containing all the arguments.
 * @param[in]  optstring      The string of expected options.
 * @param[out] r              The per call struct for holding return args and other options.
 *
 * @return ASCII value of the option if option was successfully found.
 *         '?' or ':' when unknown option is found or argument is missing.
 */
int getopt_r(int argc, char *const argv[], const char *optstring, opt_t *r);

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H */

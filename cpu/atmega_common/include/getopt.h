<<<<<<< HEAD
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
* This implements the getopt and getopt_r functionality. getopt_r has a few caveats, make sure to use it correctly.
*
* @author      Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
*
*/

#ifndef _GETOPT_H_
#define _GETOPT_H_

#if defined(__cplusplus)
extern "C" {
#endif

typedef struct{
	char* optarg;
	int optind;
	int opterr;
	int optopt;
} opt_t;

static const opt_t def_opt = {NULL,1,1,-1};

/*Don't forget to use this else the call to getopt_r will fail eg. opt_t x = INIT_OPT_T(x);*/
/*To change options, edit the fields of opt_t and call getopt_r*/

#define INIT_OPT_T(x) x = def_opt 

extern char* optarg;
extern int optind, opterr, optopt;

/**
* @brief      Non re-entrant version of getopt.
*
* @param[in]  argc           The count of the number of arguments.
* @param[in]  argv           The array containing all the arguments.
* @param[in]  optstring      The string of expected options.
*
* @return ASCII value of the option if option was succesfully found, '?' or ':' when unknown option is found or argument is missing.
*/

int getopt(int argc, char* const argv[], const char* optstring);

/**
* @brief      Re-entrant version of getopt.
*
* @param[in]  argc           The count of the number of arguments.
* @param[in]  argv           The array containing all the arguments.
* @param[in]  optstring      The string of expected options.
* @param[out] r              The per call struct for holding return args and other options.
*
* @return ASCII value of the option if option was succesfully found, '?' or ':' when unknown option is found or argument is missing.
*/

int getopt_r(int argc, char* const argv[], const char* optstring, opt_t* r);

#if defined(__cplusplus)
}
#endif

#endif
=======
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
* This implements the getopt and getopt_r functionality. getopt_r has a few caveats, make sure to use it correctly.
*
* @author      Viraj Sahai <vsahai@usc.edu, virajsahai32@gmail.com>
*
*/

#ifndef GETOPT_H
#define GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

typedef struct{
    char* optarg;
    int optind;
    int opterr;
    int optopt;
} opt_t;

static const opt_t def_opt = {NULL,1,1,-1};

/*Don't forget to use this else the call to getopt_r will fail eg. opt_t x = INIT_OPT_T(x);*/
/*To change options, edit the fields of opt_t and call getopt_r*/

#define INIT_OPT_T(x) x = def_opt

extern char* optarg;
extern int optind, opterr, optopt;

/**
* @brief      Non re-entrant version of getopt.
*
* @param[in]  argc           The count of the number of arguments.
* @param[in]  argv           The array containing all the arguments.
* @param[in]  optstring      The string of expected options.
*
* @return ASCII value of the option if option was succesfully found, '?' or ':' when unknown option is found or argument is missing.
*/

int getopt(int argc, char* const argv[], const char* optstring);

/**
* @brief      Re-entrant version of getopt.
*
* @param[in]  argc           The count of the number of arguments.
* @param[in]  argv           The array containing all the arguments.
* @param[in]  optstring      The string of expected options.
* @param[out] r              The per call struct for holding return args and other options.
*
* @return ASCII value of the option if option was succesfully found, '?' or ':' when unknown option is found or argument is missing.
*/

int getopt_r(int argc, char* const argv[], const char* optstring, opt_t* r);

#ifdef __cplusplus
}
#endif

#endif /* GETOPT_H */
>>>>>>> 4dc4a075b... Squashing and rebasing

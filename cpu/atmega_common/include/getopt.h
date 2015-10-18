/*
 * Copyright (C) 2015 Mohmmad Ayman <Mohmmad.Salah94@eng-st.cu.edu.eg>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_atmega_common
 * @{
 *
 * @file		header
 * @brief       Implementation of getopt lib.
 * @author      Mohmmad Ayman <Mohmmad.Salah94@eng-st.cu.edu.eg>
 *
 * @}
 */

typedef struct {
    char *optarg;
    int optind;
    char *scanptr;
} opt_r;


#ifndef _GETOPT_H
#define _GETOPT_H

#ifdef __cplusplus
extern "C" {
#endif

int getopt(int argc, char *argv[], char *optstring);

#ifdef __cplusplus
}
#endif

#endif

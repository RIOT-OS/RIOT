/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @file
 * @brief       Definitions for RIOT registry unit tests
 *
 * @author      Leandro Lanzieri <leandro.lanzieri@haw-hamburg.de>
 */
#ifndef TESTS_REGISTRY_H
#define TESTS_REGISTRY_H

#include "embUnit.h"

#ifdef __cplusplus
extern "C" {
#endif

#define TEST_APP_TREE_L1        "app"
#define TEST_APP_TREE_L2        "id"
#define TEST_APP_TREE_L3        "serial"
#define TEST_APP_TREE_LEN       3

#define TEST_SENSORS_TREE_L1       "sensors"
#define TEST_SENSORS_TREE_L2       "threshold"
#define TEST_SENSORS_TREE_LEN      2

#define TEST_SET_VALUE_VAL      "abcd"

#define TEST_EXPORT_VALUE_VAL_1 "5265676973747279"
#define TEST_EXPORT_VALUE_VAL_2 "-10930"

#define NAME_SEPARATOR "/"

#ifdef __cplusplus
}
#endif

#endif /* TESTS_REGISTRY_H */
/** @} */

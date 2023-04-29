/*
 * Copyright (C) 2023 ML!PA Consulting Gmbh
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup tests
 *
 * @{
 *
 * @file    Test configuration types
 *
 * @author  Fabian Hüßler <fabian.huessler@ml-pa.com>
 */

#ifndef PERSIST_TYPES_H
#define PERSIST_TYPES_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Food configuration item
 */
typedef struct food {
    char price[6];  /**< Food price as a string */
} food_t;

/**
 * @brief   Drinks configuration item
 */
typedef struct drink {
    char price[6];  /**< Drinks price as a string */
} drink_t;

/**
 * @brief   Order configuration item
 */
typedef struct order_item_t {
    char item[12]; /**< Ordered item name */
} order_item_t;

/**
 * @brief   Order configuration
 */
typedef struct order {
    order_item_t items[2];  /**< Ordered items */
} order_t;


#define TEST_CONFIGURATION_MAX_ORDERS   3

/**
 * @brief   Full configuration compound item
 */
struct configuration {
    struct {
        struct {
            food_t white;                           /**< location of "/food/bread/white" */
            food_t whole_grain;                     /**< location of "/food/bread/whole_grain" */
        } bread;                                    /**< location of "/food/bread" */
        struct {
            food_t cheesecake;                      /**< location of "/food/cake/cheesecake" */
            food_t donut;                           /**< location of "/food/cake/donut" */
        } cake;                                     /**< location of "/food/cake" */
    } food;                                         /**< location of "/food" */
    struct {
        drink_t coffee;                             /**< location of "/drinks/coffee" */
        drink_t tea;                                /**< location of "/drinks/tea" */
        drink_t cocoa;                              /**< location of "/drinks/cocoa" */
    } drinks;                                       /**< location of "/drinks" */
    order_t orders[TEST_CONFIGURATION_MAX_ORDERS];  /**< location of "/orders" */
};

#ifdef __cplusplus
}
#endif

#endif /* PERSIST_TYPES_H */
/** @} */

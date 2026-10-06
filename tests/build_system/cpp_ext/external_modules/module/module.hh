/*
 * SPDX-FileCopyrightText: 2019 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Sample module C++ class
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 *
 * @}
 */

#ifndef MODULE_H
#define MODULE_H

class module_class
{
public:
    /**
     * @brief constructor
     */
    module_class();

    /**
     * @brief destructor
     */
    ~module_class();

    /**
     * @brief public function
     */
    void print_hello(void);
};

/** @} */
#endif /* MODULE_H */

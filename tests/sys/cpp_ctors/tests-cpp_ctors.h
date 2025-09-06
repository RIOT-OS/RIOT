/*
 * Copyright (C) 2016-2017 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @addtogroup  unittests
 * @{
 *
 * @file
 * @brief       Unittests for C++ constructors
 *
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */

#ifdef __cplusplus
extern "C" {
    /* static test complains if this is not there */
}
#endif

#ifdef __cplusplus
namespace RIOTTestCPP {
    /**
     * @brief Simple class used for testing constructor calls
     */
    class MyClass {
    private:
        volatile long var;

    public:
        MyClass();

        explicit MyClass(long value);

        long value();

        void inc();
    };
}
#endif

/** @} */

/*
 * SPDX-FileCopyrightText: 2016-2017 Eistec AB
 * SPDX-License-Identifier: LGPL-2.1-only
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

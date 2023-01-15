/*
 * Copyright (C) 2021 Jens Wetterich <jens@wetterich-net.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */
/**
 * @ingroup     sys
 * @defgroup    unittests_cpp   C++ Unittests
 * @brief       RIOT unit tests for C++
 * @details     The C++ unit test framework syntax is loosely based on
 *              GoogleTest, but offers far less functionality.
 * For mocking the package @ref pkg_fff can be used.
 * @{
 * @file
 * @brief   RIOT unit tests for C++
 * @details The C++ unit test framework syntax is loosely based on GoogleTest,
 *          but offers far less functionality.
 * For mocking the package @ref pkg_fff can be used.
 * @author Jens Wetterich <jens@wetterich-net.de>
 *
 */
#ifndef CPPUNIT_H
#define CPPUNIT_H
#if __cplusplus >= 201103L || defined(DOXYGEN)
#include "cppunit/cppunit_base.hpp"
#include "cppunit/cppunit_expect.hpp"
#include "cppunit/cppunit_fff.hpp"
#else
#error This library needs C++11 and newer
#endif
#endif
/** @} */

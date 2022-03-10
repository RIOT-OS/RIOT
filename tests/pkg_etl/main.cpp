/*
 * Copyright (C) 2022 Jens Wetterich <jens@wetterich-net.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup    tests
 * @{
 * @file
 * @brief      Unit test to test the usage of @ref pkg_etl package
 * @author     Jens Wetterich <jens@wetterich-net.de>
 */
#include "etl/string.h"
#include "etl/to_string.h"
#include "etl/type_traits.h"
#include "etl/vector.h"
#include <stdio.h>

int main() {
    puts("Testing etl");

    /* Create test string */
    etl::string<100> teststr = "Test: is_same: ";

    /* Append some type_trait tests */
    etl::to_string(etl::is_same<bool, bool>::value, teststr, true);
    teststr += ", is_unsigned: ";
    etl::to_string(etl::is_unsigned<int>::value, teststr, true);
    teststr += ", vector: ";

    /* Do some vector testing and append to the string */
    etl::vector<char, 5> testvec;
    testvec.push_back('a');
    testvec.push_back('b');
    testvec.clear();
    testvec.push_back('c');
    testvec.push_back('d');
    testvec.push_back('e');
    testvec.push_back('a');
    testvec.push_back('b');
    teststr += testvec[0];
    teststr += testvec[1];
    teststr += testvec[2];
    teststr += testvec[3];
    teststr += testvec[4];
    teststr += ", size: ";

    /* Test string size */
    etl::to_string(teststr.size(), teststr, true);

    /* Test string compare */
    if (teststr == "Test: is_same: 1, is_unsigned: 0, vector: cdeab, size: 55") {
        teststr += " -> valid";
    } else {
        teststr += " -> false";
    }

    puts(teststr.c_str());
    return 0;
}
/** @} */

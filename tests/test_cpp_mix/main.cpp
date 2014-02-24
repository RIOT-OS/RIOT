/*
 * Copyright (C) 2014 Hamburg University of Applied Siences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @file        main.cpp
 * @brief       Demonstration of mixed c++ user application with pure c RIOT
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 */

#include <cstdio>
#include <vector>
#include "cppMix.hpp"

using namespace std;

int main()
{
    printf("Hello c++ on RIOT\n");

    cppMix oMix;

    printf("\n-= Test Overloading =-\n");
    oMix.sayHello();
    oMix.sayHello(42);
    oMix.sayHello(3.141592f);

    printf("\n-= Test namespace =-\n");
    printf("typing std::vector is obsolete when 'using namespace std;'\n");
    vector<int> vInts;
    vInts.push_back(1);
    vInts.push_back(3);
    vInts.push_back(2);
    printf("The vector vInts has been filled with %d numbers.\n", vInts.size());

    printf("\n-= Test iterator =-\n");
    printf("The content of vInts = { ");

    for (vector<int>::iterator it = vInts.begin(); it != vInts.end(); ++it) {
        printf("%d ", *(it));
    }

    printf("}\n");
}

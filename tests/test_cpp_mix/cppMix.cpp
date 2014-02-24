/*
 * Copyright (C) 2014 Hamburg University of Applied Siences (HAW)
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup tests
 * @{
 *
 * @file        cppMix.cpp
 * @brief       implementation of declared functions of object cppMix
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 *
 * @}
 */
 
#include "cppMix.hpp"

cppMix::cppMix()
{
    printf("Instanciating Object [constructor called]\n");
    greet();
}

cppMix::~cppMix()
{
    printf("Destroying Object [destructor called]\n");
    printf("Im shutting down!\n");
}

void cppMix::sayHello(void)
{
    printf("Hello!\n");
}

void cppMix::sayHello(int n)
{
    printf("Hello![int: %d]\n", n);
}

void cppMix::sayHello(float f)
{
    printf("Hello![float: %f]\n", f);
}

void cppMix::greet(void)
{
    printf("Im starting!\n");
}

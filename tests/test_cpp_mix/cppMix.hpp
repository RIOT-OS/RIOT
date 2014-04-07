/*
 * Copyright (C) 2014 Hamburg University of Applied Siences (HAW)
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup
 * @brief
 * @ingroup     tests
 * @{
 *
 * @file        cppMix.hpp
 * @brief       simple c++ object declaration with public and private functions
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 */

#ifndef __CPPMIX__
#define __CPPMIX__

#include <cstdio>

class cppMix
{
public:
    /**
     * @brief constructor
     */
    cppMix();

    /**
     * @brief destructor
     */
    ~cppMix();

    /**
     * @brief public function
     */
    void sayHello(void);

    /**
     * @brief overloaded public function with int parameter
     */
    void sayHello(int n);

    /**
     * @brief overloaded public function with float parameter
     */
    void sayHello(float f);
private:
    /**
     * @brief private function
     */
    void greet(void);
};

#endif //__CPPMIX__

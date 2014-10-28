/*
 * Copyright (C) 2014 Hamburg University of Applied Sciences (HAW)
 * Copyright (C) 2014 Ho Chi Minh city University of Technology (HCMUT)
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup
 * @brief
 * @ingroup     examples
 * @{
 *
 * @file        cpp_class.hpp
 * @brief       simple c++ object declaration with public and private functions
 *
 * @author      Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * @author      DangNhat Pham-Huu <51002279@stu.hcmut.edu.vn>
 */

#ifndef CPP_CLASS_H_
#define CPP_CLASS_H_

#include <cstdio>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
 extern "C"{
  #include "socket_base.h"
//#include "net_if.h"
#include "sixlowpan.h"
#include "board_uart0.h"
#include "ipv6.h"
#include "shell.h"
#include "shell_commands.h"
#include "posix_io.h"
//#include "transceiver.h"  
 }


#pragma once

class sixlowpan
{
public:
    sixlowpan();
    ~sixlowpan();
    void sixlowpan_init(int argc, char **argv);
    void printUsage();
    void print_ipv6_addr(const ipv6_addr_t *ipv6_addr);
    void bootstrapping(int argc, char **argv);
    void send_packet(int argc, char **argv);
    void ip(int argc, char **argv);
    void context(int argc, char **argv);
private:

};


class cpp_class
{
public:
    /**
     * @brief constructor
     */
    cpp_class();

    /**
     * @brief destructor
     */
    ~cpp_class();

    /**
     * @brief public function
     */
    void say_hello(void);

    /**
     * @brief overloaded public function with int parameter
     */
    void say_hello(int n);

    /**
     * @brief overloaded public function with float parameter
     */
    void say_hello(float f);
private:
    /**
     * @brief private function
     */
    void greet(void);
};

/** @} */
#endif //__CPP_CLASS__

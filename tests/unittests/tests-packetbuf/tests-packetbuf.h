/*
 * Copyright (C) 2014 Martin Lenders
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @addtogroup  unittests
 * @{
 *
 * @file        tests-packetbuf.h
 * @brief       Unittests for the ``sys/net/crosslayer/packetbuf`` module
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __TESTS_SYS_NET_CROSSLAYER_PACKETBUF_H_
#define __TESTS_SYS_NET_CROSSLAYER_PACKETBUF_H_
#include "embUnit/embUnit.h"

/**
 * @brief   The entry point of this test suite.
 */
void tests_packetbuf(void);

/**
 * @brief   Generates tests for packetbuf.h
 *
 * @return  embUnit tests if successful, NULL if not.
 */
Test *tests_packetbuf_tests(void);

#endif /* __TESTS_SYS_NET_CROSSLAYER_PACKETBUF_H_ */

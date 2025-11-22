/*
 * Copyright (C) 2024-2025 Carl Seifert
 * Copyright (C) 2024-2025 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit test suite for `unicoap` module
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include "tests-unicoap.h"
#include <stdio.h>

Test* tests_unicoap_options(void);
Test* tests_unicoap_pdu(void);
Test* tests_unicoap_message(void);
Test* tests_unicoap_matching(void);

void tests_unicoap(void)
{
    TESTS_RUN(tests_unicoap_pdu());
    TESTS_RUN(tests_unicoap_options());
    TESTS_RUN(tests_unicoap_message());
    TESTS_RUN(tests_unicoap_matching());
}

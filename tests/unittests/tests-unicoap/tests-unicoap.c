/*
 * SPDX-FileCopyrightText: 2024-2025 Carl Seifert
 * SPDX-FileCopyrightText: 2024-2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @file
 * @ingroup unittests
 * @brief   Unit test suite for `unicoap` module
 * @author  Carl Seifert <carl.seifert1@mailbox.tu-dresden.de>
 */

#include "tests-unicoap.h"

Test* tests_unicoap_options(void);
Test* tests_unicoap_pdu(void);
Test* tests_unicoap_message(void);

void tests_unicoap(void)
{
    TESTS_RUN(tests_unicoap_pdu());
    TESTS_RUN(tests_unicoap_options());
    TESTS_RUN(tests_unicoap_message());
}

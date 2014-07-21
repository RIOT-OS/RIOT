/*
 * Copyright (C) 2014 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

#include "tests-crypto.h"

void tests_crypto(void)
{
	TESTS_RUN(tests_crypto_aes_tests());
	TESTS_RUN(tests_crypto_3des_tests());
}


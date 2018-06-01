/*
 * Copyright (C) 2016 Mathias Tausig, FH Campus Wien
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <inttypes.h>
#include <thread.h>

#include "tfm.h"

/**
 * @brief Do some basic calculations with small numbers
 *
 */
int basic_arithmetic_tests(void)
{
    fp_int a;

    fp_read_radix(&a, "1", 10);
    fp_int b;
    fp_read_radix(&b, "2", 10);
    fp_int c;
    fp_add(&a, &b, &c);
    fp_int d;
    fp_read_radix(&d, "3", 10);

    if (fp_cmp(&c, &d) != FP_EQ) {
        printf("Calculating 1+2=3 failed.\n");
        return -1;
    }
    return 0;
}

/**
 * @brief Do one RSA decryption (based on NIST test vectors)
 *
 */
int rsa_tests(void)
{
    /* Testvectors taken from "RSADP Decryption Operation Primitive Component Testing" at http://csrc.nist.gov/groups/STM/cavp/component-testing.html#pkcs1-v1.5 */
    char mod_str[] = "d0b750c8554b64c7a9d34d068e020fb52fea1b39c47971a359f0eec5da0437ea3fc94597d8dbff5444f6ce5a3293ac89b1eebb3f712b3ad6a06386e6401985e19898715b1ea32ac03456fe1796d31ed4af389f4f675c23c421a125491e740fdac4322ec2d46ec945ddc349227b492191c9049145fb2f8c2998c486a840eac4d3";
    char priv_exp_str[] = "27b7119a09edb827c13418c820b522a1ee08de0e4bb28106db6bb91498a3b361ab293af83fefcdd8a6bd2134ca4afacf64a0e33c014f48f47530f8847cc9185cbedec0d9238c8f1d5498f71c7c0cff48dc213421742e34350ca94007753cc0e5a783264cf49ff644ffea94253cfe86859acd2a2276ca4e7215f8ebaa2f188f51";
    char cipher_text_str[] = "6cf87c6a65925df6719eef5f1262edc6f8a0a0a0d21c535c64580745d9a268a95b50ff3be24ba8b649ca47c3a760b71ddc3903f36aa1d98e87c53b3370be784bffcb5bc180dea2acc15bb12e681c889b89b8f3de78050019dcdbb68c051b04b880f0f8c4e855321ffed89767fc9d4a8a27a5d82ba450b2478c21e11843c2f539";
    char plain_text_str[] = "5c7bce723cf4da053e503147242c60678c67e8c22467f0336b6d5c31f14088cb3d6cefb648db132cb32e95092f3d9bcd1cab51e68bd3a892ab359cdff556785ae06708633d39a0618f9d6d70f6bdeb6b777e7dd9acc41f19560c71a68479c8a07b14fb9a4c765fd292ae56dd2f2143b62649cc70fb604fdc5cc1ade6e29de235";

    static fp_int modulus;

    fp_read_radix(&modulus, mod_str, 16);

    static fp_int private_exponent;
    fp_read_radix(&private_exponent, priv_exp_str, 16);

    static fp_int cipher_text;
    fp_read_radix(&cipher_text, cipher_text_str, 16);

    static fp_int plain_text;
    fp_read_radix(&plain_text, plain_text_str, 16);

    static fp_int result;

    if (FP_OKAY != fp_exptmod(&cipher_text, &private_exponent, &modulus, &result)) {
        printf("Error doing modular exponentiation.\n");
        return -1;
    }

    if (FP_EQ != fp_cmp(&result, &plain_text)) {
        printf("RSA decryption failed.\n");
        return -2;
    }

    return 0;
}

int main(void)
{
    if (0 > basic_arithmetic_tests()) {
        printf("Small tests failed.\n");
        return 1;
    }

    if (0 > rsa_tests()) {
        printf("RSA tests failed.\n");
        return 1;
    }

    printf("All tests OK.\n");
    return 0;
}

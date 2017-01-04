/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @author  Francisco Acosta <francisco.acosta@inria.fr>
 */

#include <stdio.h>
#include <string.h>

#include "tweetnacl.h"

static unsigned char server_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char server_sk[crypto_box_SECRETKEYBYTES];

/* Public Key file */
FILE *server_pkey;

/* Secret Key file */
FILE *server_skey;

static unsigned char firmware_pk[crypto_box_PUBLICKEYBYTES];
static unsigned char firmware_sk[crypto_box_SECRETKEYBYTES];

/* Public Key file */
FILE *firmware_pkey;

/* Secret Key file */
FILE *firmware_skey;

/* Keys C File */
FILE *keys;

int main(void)
{
    printf("Creating server keypair...\n");
    crypto_box_keypair(server_pk, server_sk);
    printf("Done.\n");

    printf("Server Secret Key:\n");
    for (int i = 0; i < crypto_box_SECRETKEYBYTES; i++) {
        printf("%02x", server_sk[i]);
    }
    printf("\n");

    printf("Writing server secret key to server_skey\n");
    if ((server_skey = fopen("server_skey", "w")) != NULL) {
        fwrite(server_sk, sizeof(server_sk), 1, server_skey);
        fclose(server_skey);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    printf("Server Public Key:\n");
    for (int i = 0; i < crypto_box_PUBLICKEYBYTES; i++) {
        printf("%02x", server_pk[i]);
    }
    printf("\n");

    printf("Writing server public key to keys.c\n");
    if ((keys = fopen("keys.c", "w")) != NULL) {
        fprintf(keys, "const unsigned char server_pkey[] = {");
        for (int i = 1; i <= crypto_box_PUBLICKEYBYTES; i++) {
            if (i < crypto_box_PUBLICKEYBYTES) {
                fprintf(keys, "0x%02x, ", server_pk[i-1]);
                if (i%8 == 0 && i > 0) {
                    fprintf(keys, "\n                                     ");
                }
            } else {
                fprintf(keys, "0x%02x};\n\n", server_pk[i-1]);
            }
        }
        fclose(keys);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    printf("Writing server public key to server_pkey.pub\n");
    if ((server_pkey = fopen("server_pkey.pub", "w")) != NULL) {
        fwrite(server_pk, sizeof(server_pk), 1, server_pkey);
        fclose(server_pkey);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    printf("Creating firmware keypair...\n");
    crypto_box_keypair(firmware_pk, firmware_sk);
    printf("Done.\n");

    printf("Firmware Secret Key:\n");
    for (int i = 0; i < crypto_box_SECRETKEYBYTES; i++) {
        printf("%02x", firmware_sk[i]);
    }
    printf("\n");

    printf("Writing firmware secret key to firmware_skey\n");
    if ((firmware_skey = fopen("firmware_skey", "w")) != NULL) {
        fwrite(firmware_sk, sizeof(firmware_sk), 1, firmware_skey);
        fclose(firmware_skey);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    printf("Writing firmware secret key to keys.c\n");
    if ((keys = fopen("keys.c", "a")) != NULL) {
        fprintf(keys, "const unsigned char firmware_skey[] = {");
        for (int i = 1; i <= crypto_box_PUBLICKEYBYTES; i++) {
            if (i < crypto_box_PUBLICKEYBYTES) {
                fprintf(keys, "0x%02x, ", firmware_sk[i-1]);
                if (i%8 == 0 && i > 0) {
                    fprintf(keys, "\n                                       ");
                }
            } else {
                fprintf(keys, "0x%02x};\n", firmware_sk[i-1]);
            }
        }
        fclose(keys);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    printf("Firmware Public Key:\n");
    for (int i = 0; i < crypto_box_PUBLICKEYBYTES; i++) {
        printf("%02x", firmware_pk[i]);
    }
    printf("\n");

    printf("Writing firmware public key to firmware_pkey.pub\n");
    if ((firmware_pkey = fopen("firmware_pkey.pub", "w")) != NULL) {
        fwrite(firmware_pk, sizeof(firmware_pk), 1, firmware_pkey);
        fclose(firmware_pkey);
    } else {
        printf("ERROR! Cannot write file\n");
        return -1;
    }

    return 0;
}

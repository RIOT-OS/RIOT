/*
 * asm.c - implementation of some basic encryptions using the mc1322x ASM module
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "asm.h"

/**
 * Turns on the ASM module and runs a self test.
 * The self test needs 3330 clocks.
 */
void asm_turn_on(void)
{
    ASM->CONTROL_1_bits.ON = 1;
    ASM->CONTROL_1_bits.NORMAL_MODE = 1;
    ASM->CONTROL_1_bits.SELF_TEST = 1;
    ASM->CONTROL_0_bits.START = 1;

    /* if the self test fails, this will never be true */
    while (!ASM->STATUS_bits.TEST_PASS) {
        continue;
    }

    ASM->CONTROL_1_bits.SELF_TEST = 0;
}

/**
 * Turns the ASM module off.
 */
void asm_turn_off(void)
{
    ASM->CONTROL_1_bits.ON = 0;
}

/**
 * For CTR encryption/decryption run this on each 128 bit block with the same CTR instance.
 * This function blocks until the encryption/decryption finished after 13 clocks.
 *
 * \param keys The 128 bit key. \sa asm_keys_t
 * \param data A 128 bit block to encrypt/decrypt. Holds the encrypted/decrypted
 *             data when finished \sa asm_data_t
 * \param ctr Structure holding the counter for the encryption/decrpytion. The user
 *			  is responsible to provide a decend counter function. \sa asm_ctr_t
 */
void asm_ctr_encryption_blocking(asm_keys_t *keys, asm_data_t *data, asm_ctr_t *ctr)
{
    ASM->ENC_KEY_0 = keys->value_0;
    ASM->ENC_KEY_1 = keys->value_1;
    ASM->ENC_KEY_2 = keys->value_2;
    ASM->ENC_KEY_3 = keys->value_3;

    ASM->CONTROL_1_bits.CTR = 1;

    ASM->DATA_0 = data->value_0;
    ASM->DATA_1 = data->value_1;
    ASM->DATA_2 = data->value_2;
    ASM->DATA_3 = data->value_3;

    ASM->COUNTER_0 = ctr->value_0;
    ASM->COUNTER_1 = ctr->value_1;
    ASM->COUNTER_2 = ctr->value_2;
    ASM->COUNTER_3 = ctr->value_3;

    ASM->CONTROL_0_bits.START = 1;

    while (!ASM->STATUS_bits.DONE) {
        continue;
    }

    data->value_0 = ASM->COUNTER_RESULT_0;
    data->value_1 = ASM->COUNTER_RESULT_1;
    data->value_2 = ASM->COUNTER_RESULT_2;
    data->value_3 = ASM->COUNTER_RESULT_3;
}

/**
 * Usage of the MAC functions.
 * 1. Initialize with the keys. 			( asm_cbc_mac_init(keys)   )
 * 2. Update for each 128 bit block of data ( asm_cbc_mac_update(data) )
 * 3. Finish and read the computed data 	( asm_cbc_mac_finish(data) )
 */

/**
 * Initialize a MAC stream.
 * \param keys Holding the 128 bit key \sa asm_keys_t
 */
void asm_cbc_mac_init(asm_keys_t *keys)
{
    ASM->CONTROL_0_bits.CLEAR = 1;

    ASM->ENC_KEY_0 = keys->value_0;
    ASM->ENC_KEY_1 = keys->value_1;
    ASM->ENC_KEY_2 = keys->value_2;
    ASM->ENC_KEY_3 = keys->value_3;

    ASM->CONTROL_1_bits.CBC = 1;
}

/**
 * Updates a MAC stream.
 * \param data Structure holding 128 bits of data. \sa asm_data_t
 */
void asm_cbc_mac_update_blocking(asm_data_t *data)
{
    ASM->DATA_0 = data->value_0;
    ASM->DATA_1 = data->value_1;
    ASM->DATA_2 = data->value_2;
    ASM->DATA_3 = data->value_3;

    ASM->CONTROL_0_bits.START = 1;

    while (!ASM->STATUS_bits.DONE) {
        continue;
    }
}

/**
 * Finishes a MAC stream
 * \param data Structure holding the result of the MAC computation. \sa asm_data_t
 */
void asm_cbc_mac_finish(asm_data_t *data)
{
    data->value_0 = ASM->CBC_MAC_RESULT_0;
    data->value_1 = ASM->CBC_MAC_RESULT_1;
    data->value_2 = ASM->CBC_MAC_RESULT_2;
    data->value_3 = ASM->CBC_MAC_RESULT_3;

    ASM->CONTROL_0_bits.CLEAR = 1;
}

/**
 * Initialize a MAC stream for combined mode.
 * \param keys Holding the 128 bit key \sa asm_keys_t
 */
void asm_ctr_cbc_mac_init(asm_keys_t *keys)
{
    asm_cbc_mac_init(keys);
}

/**
 * Updates a MAC stream and encrypts data.
 * \param data Structure holding 128 bits of data to use in MAC stream and to encrypt.
 			   Holds the encrypted data when finished. \sa asm_data_t
 * \param ctr  Structure holding the counter for the encryption/decrpytion. The user
 *			   is responsible to provide a decend counter function. \sa asm_ctr_t
 */
void asm_ctr_cbc_mac_update(asm_data_t *data, asm_ctr_t *ctr)
{
    ASM->DATA_0 = data->value_0;
    ASM->DATA_1 = data->value_1;
    ASM->DATA_2 = data->value_2;
    ASM->DATA_3 = data->value_3;

    ASM->COUNTER_0 = ctr->value_0;
    ASM->COUNTER_1 = ctr->value_1;
    ASM->COUNTER_2 = ctr->value_2;
    ASM->COUNTER_3 = ctr->value_3;

    ASM->CONTROL_0_bits.START = 1;

    while (!ASM->STATUS_bits.DONE) {
        continue;
    }

    data->value_0 = ASM->COUNTER_RESULT_0;
    data->value_1 = ASM->COUNTER_RESULT_1;
    data->value_2 = ASM->COUNTER_RESULT_2;
    data->value_3 = ASM->COUNTER_RESULT_3;
}
/**
 * Finishes a MAC stream for combined mode
 * \param data Structure holding the result of the MAC computation. \sa asm_data_t
 */
void asm_ctr_cbc_mac_finish(asm_data_t *data)
{
    asm_cbc_mac_finish(data);
}

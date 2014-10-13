/*
 * asm.h - defines registers for the Advanced Security Module of the mc1322x
 * Copyright (C) 2013 Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef ASM_H
#define ASM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define ASM_BASE_ADDRESS 0x80008000

struct ASM_struct {
    uint32_t ENC_KEY_0;             ///< Encryption key 0:31
    uint32_t ENC_KEY_1;             ///< Encryption key 32:63
    uint32_t ENC_KEY_2;             ///< Encryption key 64:95
    uint32_t ENC_KEY_3;             ///< Encryption key 96:127
    uint32_t DATA_0;                ///< Data 0:31
    uint32_t DATA_1;                ///< Data 32:63
    uint32_t DATA_2;                ///< Data 64:95
    uint32_t DATA_3;                ///< Data 96:127
    uint32_t COUNTER_0;             ///< Counter 0:31
    uint32_t COUNTER_1;             ///< Counter 32:63
    uint32_t COUNTER_2;             ///< Counter 64:95
    uint32_t COUNTER_3;             ///< Counter 96:127
    uint32_t COUNTER_RESULT_0;      ///< Result of CTR calculation 0:31
    uint32_t COUNTER_RESULT_1;      ///< Result of CTR calculation 32:63
    uint32_t COUNTER_RESULT_2;      ///< Result of CTR calculation 64:95
    uint32_t COUNTER_RESULT_3;      ///< Result of CTR calculation 96:127
    uint32_t CBC_MAC_RESULT_0;      ///< Result of CBC-MAC 0:31
    uint32_t CBC_MAC_RESULT_1;      ///< Result of CBC-MAC 32:63
    uint32_t CBC_MAC_RESULT_2;      ///< Result of CBC-MAC 64:95
    uint32_t CBC_MAC_RESULT_3;      ///< Result of CBC-MAC 96:127
    union {
        uint32_t CONTROL_0;         ///< Control 0 register
        struct ASM_CONTROL_0 {
            uint32_t : 24;
            uint32_t START: 1;      ///< Start calculation
            uint32_t CLEAR: 1;      ///< Clear all configurations
            uint32_t LOAD_MAC: 1;   ///< Load an initila MAC value to continue
            uint32_t : 4;
            uint32_t CLEAR_IRQ;     ///< Clear ASM IRQ bit
        } CONTROL_0_bits;
    };
    union {
        uint32_t CONTROL_1;         ///< Control 1 register
        struct ASM_CONTROL_1 {
            uint32_t ON: 1;         ///< Turn ASM module on
            uint32_t NORMAL_MODE: 1;///< exit boot mode
            uint32_t BYPASS: 1;     ///< Data passes through unmodified
            uint32_t : 21;
            uint32_t CBC: 1;        ///< enable CBC-MAC mode
            uint32_t CTR: 1;        ///< enable CTR mode
            uint32_t SELF_TEST: 1;  ///< switch to self test mode
            uint32_t : 4;
            uint32_t MASK_IRQ: 1;   ///< disables the ASM IRQ
        } CONTROL_1_bits;
    };
    union {
        uint32_t STATUS;            ///< status register
        struct ASM_STATUS {
            uint32_t : 24;
            uint32_t DONE: 1;       ///< operation is done
            uint32_t TEST_PASS: 1;  ///< self test did pass
            uint32_t : 6;
        } STATUS_bits;
    };
    uint32_t reserved;
    uint32_t CBC_MAC_0;             ///< Result of MAC calculation 0:31
    uint32_t CBC_MAC_1;             ///< Result of MAC calculation 32:63
    uint32_t CBC_MAC_2;             ///< Result of MAC calculation 64:95
    uint32_t CBC_MAC_3;             ///< Result of MAC calculation 96:127
};

static volatile struct ASM_struct *const ASM = (void *)(ASM_BASE_ADDRESS);

struct ASM_Container {
    uint32_t value_0;
    uint32_t value_1;
    uint32_t value_2;
    uint32_t value_3;
};
typedef struct ASM_Container asm_keys_t;
typedef struct ASM_Container asm_data_t;
typedef struct ASM_Container asm_ctr_t;

/* function prototypes */

void asm_turn_on(void);
void asm_turn_off(void);
void asm_ctr_encryption_blocking(asm_keys_t *keys, asm_data_t *data, asm_ctr_t *ctr);
void asm_cbc_mac_init(asm_keys_t *keys);
void asm_cbc_mac_update_blocking(asm_data_t *data);
void asm_cbc_mac_finish(asm_data_t *data);
void asm_ctr_cbc_mac_init(asm_keys_t *keys);
void asm_ctr_cbc_mac_update(asm_data_t *data, asm_ctr_t *ctr);
void asm_ctr_cbc_mac_finish(asm_data_t *data);

#ifdef __cplusplus
}
#endif

#endif /* ASM_H */

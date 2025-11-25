/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup         sys_psa_crypto
 * @defgroup        sys_psa_crypto_pers_stor PSA Crypto Persistent Storage API
 * @{
 *
 * @file        psa_crypto_persistent_storage.h
 * @brief
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PERSISTENT_STORAGE_H
#define PSA_CRYPTO_PERSISTENT_STORAGE_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa/crypto.h"

/**
 * @brief   Writes a CBOR encoded key slot to a file
 *
 * @param   id              ID of slot, used as filename
 * @param   input           Pointer to CBOR encoded data
 * @param   input_len       Length of CBOR encoded data
 * @return  psa_status_t
 */
psa_status_t psa_write_encoded_key_slot_to_file(psa_key_id_t id,
                                                     uint8_t* input,
                                                     size_t input_len);

/**
 * @brief   Reads a CBOR encoded key slot from a file
 *
 * @param   id              ID of the desired key
 * @param   output          Output buffer to write CBOR data to
 * @param   output_size     Size of output buffer
 * @param   output_data_len Actual length of CBOR encoded data
 * @return  psa_status_t
 */
psa_status_t psa_read_encoded_key_slot_from_file(psa_key_id_t id,
                                                     uint8_t *output,
                                                     size_t output_size,
                                                     size_t *output_data_len);

/**
 * @brief   Destroy a key in persistent storage
 *
 * @note    This will only remove the link to the key file without erasing the
 *          key from the flash. The key material can still be recovered by someone
 *          with access to the hardware.
 *
 * @param   key_id  ID of the key to be destroyed
 * @return  psa_status_t
 */
psa_status_t psa_destroy_persistent_key(psa_key_id_t key_id);

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PERSISTENT_STORAGE_H */
/** @} */

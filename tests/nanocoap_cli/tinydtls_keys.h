/*
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @file
 * @brief       PSK and RPK keys for the dtls-sock example.
 *
 * @author      Raul Fuentes <raul.fuentes-samaniego@inria.fr>
 *
 * @}
 */

#ifndef TINYDTLS_KEYS_H
#define TINYDTLS_KEYS_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  Default keys examples for tinyDTLS (for RIOT, Linux and Contiki)
 */
#define PSK_DEFAULT_IDENTITY "Client_identity"
#define PSK_DEFAULT_KEY "secretPSK"
#define PSK_OPTIONS "i:k:"
#define PSK_ID_MAXLEN 32
#define PSK_MAXLEN 32

#ifdef __cplusplus
}
#endif

#endif /* TINYDTLS_KEYS_H */

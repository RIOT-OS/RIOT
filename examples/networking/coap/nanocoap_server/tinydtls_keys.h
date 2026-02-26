/*
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

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
